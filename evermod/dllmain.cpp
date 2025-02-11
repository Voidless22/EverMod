/**
* Copyright (C) 2020 Elisha Riedlinger
*
* This software is  provided 'as-is', without any express  or implied  warranty. In no event will the
* authors be held liable for any damages arising from the use of this software.
* Permission  is granted  to anyone  to use  this software  for  any  purpose,  including  commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not claim that you  wrote the
*      original  software. If you use this  software  in a product, an  acknowledgment in the product
*      documentation would be appreciated but is not required.
*   2. Altered source versions must  be plainly  marked as such, and  must not be  misrepresented  as
*      being the original software.
*   3. This notice may not be removed or altered from any source distribution.
*/

#include "dinput8.h"
#include <iostream>
#include "config.h"
#include "evermod.h"
#include "detours.h"
#include "DetourManager.h"


uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

CEverQuest** ppEverQuest = (CEverQuest**)(EQAddresses[2].address);

void processHooks() {
	InterpretCmd_Hook();
	DisplayChat_Hook();
}

int EverMod::InterpretCommands(void* pChar, const char* command) {
	std::cout << "Command Intercepted: " << (command ? command : "(null)") << std::endl;

	if (!stricmp(command, "/helloworld"))
	{
		((CEverQuest*)*ppEverQuest)->DisplayChat_Detour("Hello World!", 0, false, false);
		std::cout << "Test command detected!" << std::endl;
		return 1;
	}
	return 0;
}

void CreateConsole()
{	
	if (!AllocConsole()) {
		return;
	}
	// std::cout, std::clog, std::cerr, std::cin
	FILE* fDummy;
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);


}
void processPatches() {
	if (disablePatchme) {
		bypassPatchme(baseAddress);
	}
	if (enableOldModelMounts) {
		enableOldModelHorses(baseAddress);
	}
	if (enableDoubleCombatDmgFix) {
		patchDoubleCombatDmg(baseAddress);
	}
	if (enableMQ2Mitigation)
	{
		breakMQ2(baseAddress);
	}
}

bool WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Load dll
		initPassthrough();
		if (enableDebugConsole)
		{
			CreateConsole();

		}
		std::cout << "Attached to eqgame. \n" ;
		processPatches();
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		processHooks();
		DetourTransactionCommit();
		break;

	case DLL_PROCESS_DETACH:
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourTransactionCommit();
		freePassthrough();
		break;
	}

	return true;
}


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
DWORD baseAddress = (DWORD)GetModuleHandle(NULL);


void PatchA(LPVOID address, const void* dwValue, SIZE_T dwBytes) {
	unsigned long oldProtect;
	VirtualProtect((void*)address, dwBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
	FlushInstructionCache(GetCurrentProcess(), (void*)address, dwBytes);
	memcpy((void*)address, dwValue, dwBytes);
	VirtualProtect((void*)address, dwBytes, oldProtect, &oldProtect);
}



void processPatches() {
	if (disablePatchme) {
		bypassPatchme();
	}
	if (enableOldModelMounts) {
		enableOldModelHorses();
	}
}

void bypassPatchme() {
	DWORD var = (((DWORD)0x005FE751 - 0x400000) + baseAddress);
	PatchA((DWORD*)var, "\xEB\x1C\x90\x90\x90", 5); // patchme req bypass
	std::cout << "Bypassed Patchme...\n";

}

void enableOldModelHorses() {
	DWORD var = (((DWORD)0x0058DE28 - 0x400000) + baseAddress);
	PatchA((DWORD*)var, "\x32\xC0", 2); // No mount models
	std::cout << "Enabled Old Model Mounts...\n";
}
void enableDoubleCombatDmgFix() {
	DWORD var = (((DWORD)0x0045385D - 0x400000) + baseAddress);
	PatchA((DWORD*)var, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 30); //hp damage in combat abilities fix

}


void CreateConsole()
{
    if (!AllocConsole()) {
        return;
    }
    // std::cout, std::clog, std::cerr, std::cin
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();

}
bool WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Load dll
		initPassthrough();
		//AllocConsole();
        CreateConsole();
		std::cout << "Attached to eqgame. \n" ;
		processPatches();
		break;

	case DLL_PROCESS_DETACH:
		freePassthrough();
		//FreeConsole();
		break;
	}

	return true;
}


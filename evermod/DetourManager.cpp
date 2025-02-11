#include "DetourManager.h"
#include "evermod.h"
#include "detours.h"
#include <iostream>


uintptr_t getOffset(uintptr_t address) {
	return ((address - 0x400000) + (uintptr_t)GetModuleHandle(NULL));
}

 std::vector<EQAddress> EQAddresses = {
	{"CmdInterpreter", getOffset(0x51FCE0)},
	{"DisplayChat", getOffset(0x51F1A0)},
	{"EverQuest", getOffset(0xE67CCC)},
 };

// Command Interpreter
void InterpretCmd_Hook()
{
	void** target = (void**)&CEverQuest::InterpretCmd_Trampoline_Ptr;
	*target = (void*)EQAddresses[0].address;
	auto detour = &CEverQuest::InterpretCmd_Detour;
	DetourAttach(target, *(void**)&detour);
}
void CEverQuest::InterpretCmd_Detour(void* pChar, const char* szFullLine)
{
	if (!EverMod::InterpretCommands(pChar, szFullLine)) {
		CEverQuest::InterpretCmd_Trampoline(pChar, szFullLine);

	}
}

// Display Chat
void DisplayChat_Hook() {
	void** target = (void**)&CEverQuest::DisplayChat_Trampoline_Ptr;
	*target = (void*)EQAddresses[1].address;
	auto detour = &CEverQuest::DisplayChat_Detour;
	DetourAttach(target, *(void**)&detour);
}
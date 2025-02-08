#include "evermod.h"
#include <iostream>


void PatchA(LPVOID address, const void* dwValue, SIZE_T dwBytes) {
	unsigned long oldProtect;
	VirtualProtect((void*)address, dwBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
	FlushInstructionCache(GetCurrentProcess(), (void*)address, dwBytes);
	memcpy((void*)address, dwValue, dwBytes);
	VirtualProtect((void*)address, dwBytes, oldProtect, &oldProtect);
}



void bypassPatchme(DWORD baseAddress) {
	DWORD var = (((DWORD)0x005FE751 - 0x400000) + baseAddress);
	PatchA((DWORD*)var, "\xEB\x1C\x90\x90\x90", 5); // patchme req bypass
	std::cout << "Bypassed Patchme...\n";

}

void enableOldModelHorses(DWORD baseAddress) {
	DWORD var = (((DWORD)0x0058DE28 - 0x400000) + baseAddress);
	PatchA((DWORD*)var, "\x32\xC0", 2); // No mount models
	std::cout << "Enabled Old Model Mounts...\n";
}
void patchDoubleCombatDmg(DWORD baseAddress) {
	DWORD var = (((DWORD)0x0045385D - 0x400000) + baseAddress);
	PatchA((DWORD*)var, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 30); //hp damage in combat abilities fix
	std::cout << "Patched Double Combat Damage...\n";
}

void breakMQ2(DWORD baseAddress) {
	auto charToBreak = rand();

	DWORD var = (((DWORD)0x009DD250 - 0x400000) + baseAddress);
	PatchA((DWORD*)var, (DWORD*)&charToBreak, 4);

	charToBreak = rand();
	var = (((DWORD)0x009DD254 - 0x400000) + baseAddress);
	PatchA((DWORD*)var, (DWORD*)&charToBreak, 4);

	charToBreak = rand();
	var = (((DWORD)0x009DD258 - 0x400000) + baseAddress);
	PatchA((DWORD*)var, (DWORD*)&charToBreak, 4);

	charToBreak = rand();
	var = (((DWORD)0x009DD25C - 0x400000) + baseAddress);
	PatchA((DWORD*)var, (DWORD*)&charToBreak, 4);

	charToBreak = rand();
	var = (((DWORD)0x009DD260 - 0x400000) + baseAddress);
	PatchA((DWORD*)var, (DWORD*)&charToBreak, 4);
	std::cout << "MQ2 Injection broken...\n";

}
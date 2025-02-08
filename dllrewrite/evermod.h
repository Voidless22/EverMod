#pragma once
#include <windows.h>

void bypassPatchme(DWORD baseAddress);
void enableOldModelHorses(DWORD baseAddress);
void patchDoubleCombatDmg(DWORD baseAddress);
void breakMQ2(DWORD baseAddress);
void PatchA(LPVOID address, const void* dwValue, SIZE_T dwBytes);
#pragma once

#include <windows.h>
#include <cstdint>
#include <vector>


struct EQAddress 
{
	const char* address_name;
	uintptr_t address;
};
extern std::vector<EQAddress> EQAddresses;
 void DisplayChat_Hook();
 void InterpretCmd_Hook();

class CEverQuest 
{
public:
	// Command Interpreter
	inline static void (CEverQuest::* InterpretCmd_Trampoline_Ptr)(void* pChar, const char* szFullLine) = nullptr;
	void InterpretCmd_Trampoline(void* pChar, const char* szFullLine) 
	{
		return (this->*InterpretCmd_Trampoline_Ptr)(pChar, szFullLine);
	}
	void InterpretCmd_Detour(void* pChar, const char* szFullLine);
	// Display Chat
	inline static void (CEverQuest::* DisplayChat_Trampoline_Ptr)(const char* line, int color, bool bLogIsOk, bool bConvertPercent) = nullptr;
	void DisplayChat_Trampoline(const char* line, int color, bool bLogIsOk, bool bConvertPercent)
	{
		return (this->*DisplayChat_Trampoline_Ptr)(line, color, bLogIsOk, bConvertPercent);
	}
	// Detour Func
	void DisplayChat_Detour(const char* line, int color, bool bLogIsOk, bool bConvertPercent) {
		DisplayChat_Trampoline(line, color, bLogIsOk, bConvertPercent);
	}
};


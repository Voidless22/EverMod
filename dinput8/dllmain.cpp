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

bool patchLinks = true;
bool patchmeBypass = true;

AddressLookupTable<void> ProxyAddressLookupTable = AddressLookupTable<void>();

DirectInput8CreateProc m_pDirectInput8Create;
DllCanUnloadNowProc m_pDllCanUnloadNow;
DllGetClassObjectProc m_pDllGetClassObject;
DllRegisterServerProc m_pDllRegisterServer;
DllUnregisterServerProc m_pDllUnregisterServer;
GetdfDIJoystickProc m_pGetdfDIJoystick;

void PatchA(LPVOID address, const void* dwValue, SIZE_T dwBytes) {
	unsigned long oldProtect;
	VirtualProtect((void*)address, dwBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
	FlushInstructionCache(GetCurrentProcess(), (void*)address, dwBytes);
	memcpy((void*)address, dwValue, dwBytes);
	VirtualProtect((void*)address, dwBytes, oldProtect, &oldProtect);
}

bool WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	static HMODULE dinput8dll = nullptr;
	DWORD baseAddress = (DWORD)GetModuleHandle(NULL);
	DWORD var;
	DWORD varArray = (((DWORD)0x009BFF6D - 0x400000) + baseAddress);

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Load dll
		char path[MAX_PATH];
		GetSystemDirectoryA(path, MAX_PATH);
		strcat_s(path, "\\dinput8.dll");
		dinput8dll = LoadLibraryA(path);
		// initialize our variables for link patches
		if (patchLinks) {
			// Start patching links
			var = (((DWORD)0x004ED03B - 0x400000) + baseAddress);
			PatchA((DWORD*)var, "\x4C", 1); // Link stuff
			var = (((DWORD)0x004ED051 - 0x400000) + baseAddress);
			PatchA((DWORD*)var, (DWORD*)&varArray, 4); // Link stuff
			var = (((DWORD)0x004ED072 - 0x400000) + baseAddress);
			PatchA((DWORD*)var, (DWORD*)&varArray, 4); // Link stuff
			var = (((DWORD)0x007BBC9A - 0x400000) + baseAddress);
			PatchA((DWORD*)var, (DWORD*)&varArray, 4); // Link stuff
			var = (((DWORD)0x007BBD77 - 0x400000) + baseAddress);
			PatchA((DWORD*)var, (DWORD*)&varArray, 4); // Link stuff
			var = (((DWORD)0x009BFF6D - 0x400000) + baseAddress);
			PatchA((DWORD*)var, "\x25\x30\x38\x58", 4); // Link stuff
			var = (((DWORD)0x00A1ACE0 - 0x400000) + baseAddress);
			PatchA((DWORD*)var, "\x4F", 1); // Link stuff
			var = (((DWORD)0x004ED062 - 0x400000) + baseAddress);
			PatchA((DWORD*)var, "\x08", 1); // Fix current HP cap

			var = (((DWORD)0x004ED083 - 0x400000) + baseAddress);
			PatchA((DWORD*)var, "\x08", 1); // Fix current HP cap
		}
		if (patchmeBypass) {
			var = (((DWORD)0x005FE751 - 0x400000) + baseAddress);
			PatchA((DWORD*)var, "\xEB\x1C\x90\x90\x90", 5); // patchme req bypass
		}
		// DInput8 Redirecting Stuff
		m_pDirectInput8Create = (DirectInput8CreateProc)GetProcAddress(dinput8dll, "DirectInput8Create");
		m_pDllCanUnloadNow = (DllCanUnloadNowProc)GetProcAddress(dinput8dll, "DllCanUnloadNow");
		m_pDllGetClassObject = (DllGetClassObjectProc)GetProcAddress(dinput8dll, "DllGetClassObject");
		m_pDllRegisterServer = (DllRegisterServerProc)GetProcAddress(dinput8dll, "DllRegisterServer");
		m_pDllUnregisterServer = (DllUnregisterServerProc)GetProcAddress(dinput8dll, "DllUnregisterServer");
		m_pGetdfDIJoystick = (GetdfDIJoystickProc)GetProcAddress(dinput8dll, "GetdfDIJoystick");
		break;

	case DLL_PROCESS_DETACH:
		FreeLibrary(dinput8dll);
		break;
	}

	return true;
}

HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter)
{
	if (!m_pDirectInput8Create)
	{
		return E_FAIL;
	}

	HRESULT hr = m_pDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);

	if (SUCCEEDED(hr))
	{
		genericQueryInterface(riidltf, ppvOut);
	}

	return hr;
}

HRESULT WINAPI DllCanUnloadNow()
{
	if (!m_pDllCanUnloadNow)
	{
		return E_FAIL;
	}

	return m_pDllCanUnloadNow();
}

HRESULT WINAPI DllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv)
{
	if (!m_pDllGetClassObject)
	{
		return E_FAIL;
	}

	HRESULT hr = m_pDllGetClassObject(rclsid, riid, ppv);

	if (SUCCEEDED(hr))
	{
		genericQueryInterface(riid, ppv);
	}

	return hr;
}

HRESULT WINAPI DllRegisterServer()
{
	if (!m_pDllRegisterServer)
	{
		return E_FAIL;
	}

	return m_pDllRegisterServer();
}

HRESULT WINAPI DllUnregisterServer()
{
	if (!m_pDllUnregisterServer)
	{
		return E_FAIL;
	}

	return m_pDllUnregisterServer();
}

LPCDIDATAFORMAT WINAPI GetdfDIJoystick()
{
	if (!m_pGetdfDIJoystick)
	{
		return nullptr;
	}

	return m_pGetdfDIJoystick();
}

#include "dinput8.h"

std::ofstream Log::LOG("dinput8.log");
AddressLookupTable<void> ProxyAddressLookupTable = AddressLookupTable<void>();

DirectInput8CreateProc m_pDirectInput8Create;
DllCanUnloadNowProc m_pDllCanUnloadNow;
DllGetClassObjectProc m_pDllGetClassObject;
DllRegisterServerProc m_pDllRegisterServer;
DllUnregisterServerProc m_pDllUnregisterServer;
GetdfDIJoystickProc m_pGetdfDIJoystick;
static HMODULE dinput8dll = nullptr;


void initPassthrough() {


	char path[MAX_PATH];
	GetSystemDirectoryA(path, MAX_PATH);
	strcat_s(path, "\\dinput8.dll");
	Log() << "Loading " << path;
	dinput8dll = LoadLibraryA(path);

	// Get function addresses
	m_pDirectInput8Create = (DirectInput8CreateProc)GetProcAddress(dinput8dll, "DirectInput8Create");
	m_pDllCanUnloadNow = (DllCanUnloadNowProc)GetProcAddress(dinput8dll, "DllCanUnloadNow");
	m_pDllGetClassObject = (DllGetClassObjectProc)GetProcAddress(dinput8dll, "DllGetClassObject");
	m_pDllRegisterServer = (DllRegisterServerProc)GetProcAddress(dinput8dll, "DllRegisterServer");
	m_pDllUnregisterServer = (DllUnregisterServerProc)GetProcAddress(dinput8dll, "DllUnregisterServer");
	m_pGetdfDIJoystick = (GetdfDIJoystickProc)GetProcAddress(dinput8dll, "GetdfDIJoystick");
}

void freePassthrough() {
	FreeLibrary(dinput8dll);
}
HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
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

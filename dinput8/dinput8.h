#include <windows.h>

FARPROC Real_DirectInput8Create = NULL;
FARPROC Real_DllCanUnloadNow = NULL;
FARPROC Real_DllGetClassObject = NULL;
FARPROC Real_DllRegisterServer = NULL;
FARPROC Real_DllUnregisterServer = NULL;
FARPROC Real_GetdfDIJoystick = NULL;

HMODULE Real_Module = NULL;

FARPROC WINAPI  GetAddress(LPCSTR lpProcName)
{
	FARPROC pAddress = GetProcAddress(Real_Module, lpProcName);
	if (pAddress == NULL)
	{
		MessageBoxW(NULL, L"Get address failed", L"dinput8", MB_OK);
		ExitProcess(1);
	}
	return pAddress;
}

VOID WINAPI Free()
{
	if (Real_Module != NULL)
		FreeLibrary(Real_Module);
	return;
}

VOID WINAPI Init()
{
	WCHAR real_dll_path[MAX_PATH];
	GetSystemDirectoryW(real_dll_path, MAX_PATH);
	lstrcatW(real_dll_path, L"\\dinput8.dll");

	Real_Module = LoadLibraryW(real_dll_path);
	if (Real_Module == NULL)
	{
		MessageBoxW(NULL, L"Load original dll failed", L"dinput8", MB_OK);
		ExitProcess(1);
	}

	Real_DirectInput8Create = GetAddress("DirectInput8Create");
	Real_DllCanUnloadNow = GetAddress("DllCanUnloadNow");
	Real_DllGetClassObject = GetAddress("DllGetClassObject");
	Real_DllRegisterServer = GetAddress("DllRegisterServer");
	Real_DllUnregisterServer = GetAddress("DllUnregisterServer");
	Real_GetdfDIJoystick = GetAddress("GetdfDIJoystick");
}

void Direct_DirectInput8Create()
{
	Real_DirectInput8Create();
}

void Direct_DllCanUnloadNow()
{
	Real_DllCanUnloadNow();
}

void Direct_DllGetClassObject()
{
	Real_DllGetClassObject();
}

void Direct_DllRegisterServer()
{
	Real_DllRegisterServer();
}

void Direct_DllUnregisterServer()
{
	Real_DllUnregisterServer();
}

void Direct_GetdfDIJoystick()
{
	Real_GetdfDIJoystick();
}


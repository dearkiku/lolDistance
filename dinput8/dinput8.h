#pragma once
// 从 Windows 头文件中排除极少使用的内容
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

HMODULE Real_Module = NULL;

FARPROC Real_DirectInput8Create = NULL;
FARPROC Real_DllCanUnloadNow = NULL;
FARPROC Real_DllGetClassObject = NULL;
FARPROC Real_DllRegisterServer = NULL;
FARPROC Real_DllUnregisterServer = NULL;
FARPROC Real_GetdfDIJoystick = NULL;

FARPROC WINAPI GetAddress(LPCSTR lpProcName)
{
    FARPROC pAddress = GetProcAddress(Real_Module, lpProcName);
    if (!pAddress)
    {
        MessageBoxW(NULL, L"Get address failed", L"dinput8.dll", MB_OK);
        ExitProcess(1);
    }
    return pAddress;
}

VOID WINAPI InitHijack()
{
    WCHAR real_dll_path[MAX_PATH];
    GetSystemDirectoryW(real_dll_path, MAX_PATH);
    lstrcatW(real_dll_path, L"\\dinput8.dll");

    Real_Module = LoadLibraryW(real_dll_path);
    if (!Real_Module)
    {
        MessageBoxW(NULL, L"Load original dll failed", L"dinput8.dll", MB_OK);
        ExitProcess(1);
    }

    Real_DirectInput8Create = GetAddress("DirectInput8Create");
    Real_DllCanUnloadNow = GetAddress("DllCanUnloadNow");
    Real_DllGetClassObject = GetAddress("DllGetClassObject");
    Real_DllRegisterServer = GetAddress("DllRegisterServer");
    Real_DllUnregisterServer = GetAddress("DllUnregisterServer");
    Real_GetdfDIJoystick = GetAddress("GetdfDIJoystick");
}

VOID WINAPI FreeHijack()
{
    if (Real_Module) FreeLibrary(Real_Module);
}

void Redirect_DirectInput8Create() { Real_DirectInput8Create(); }
void Redirect_DllCanUnloadNow() { Real_DllCanUnloadNow(); }
void Redirect_DllGetClassObject() { Real_DllGetClassObject(); }
void Redirect_DllRegisterServer() { Real_DllRegisterServer(); }
void Redirect_DllUnregisterServer() { Real_DllUnregisterServer(); }
void Redirect_GetdfDIJoystick() { Real_GetdfDIJoystick(); }

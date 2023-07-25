// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <Windows.h>
#include <string>
#include <vector>
#include "dllmain.h"
#include "method.h"
#include <clocale>


/*
DirectInput8Create
DllCanUnloadNow
DllGetClassObject
DllRegisterServer
DllUnregisterServer
GetdfDIJoystick
*/

using namespace std;
clientInfo client;
UINT customMsg = RegisterWindowMessageW(L"Kiku");
float distanceValue = 2250.00;//视距大小
DWORD64 RAXVAULE,TORAX;

using namespace std;

void time() {
    DWORD64 result = method::LocateSignature(client.hProcess, "F3 0F 5C 35 ?? ?? ?? ?? 0F 28 F8", client.startAddress, client.endAddress, 4);
    if (result) {
        PrintToConsole("特征码搜索到的地址0x%llX", result);
    }
    else {
        PrintToConsole("特征码没读取到地址");
        return;
    }
    DWORD64 基址, 基址值;
    ReadProcessMemory(client.hProcess, LPCVOID(result), &基址值, 4, 0);//RAX
    PrintToConsole("基址值 = 0x%llX", 基址值);
    基址 = result + 基址值 + 4;//基址的计算结果
    PrintToConsole("计算真正基址的值 = 0x%llX", 基址);
    float times;
    ReadProcessMemory(client.hProcess, LPCVOID(基址), &times, 4, 0);
    while (times <= 0)
    {
        Sleep(1314);
        ReadProcessMemory(client.hProcess, LPCVOID(基址), &times, 4, 0);
    }
    tz();
    PrintToConsole("TIME = %F", times);
}

void tz()
{
   DWORD64 result = method::LocateSignature(client.hProcess, "48 8B 05 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? BA",client.startAddress,client.endAddress,3);
   if (result) {
       PrintToConsole("特征码搜索到的地址0x%llX", result);
   }
   else {
       PrintToConsole("特征码没读取到地址");
       return;
   }
    DWORD64 RAX;
    ReadProcessMemory(client.hProcess, LPCVOID(result), &RAXVAULE, 4, 0);//RAX
    PrintToConsole("基址值 = 0x%llX", RAXVAULE);
    RAX = result + RAXVAULE + 4;//基址的计算结果
    PrintToConsole("计算真正基址的值 = 0x%X", RAX);
    PrintToConsole("起始地址 = 0x%llX 结束地址 = 0x%llX 大小 = %X", client.startAddress, client.endAddress, client.endAddress - client.startAddress);
    if (!ReadProcessMemory(client.hProcess, (LPCVOID)RAX, &RAXVAULE, 8, 0)){PrintToConsole("基址值RAX读取失败了");return;}
    PrintToConsole("基址值RAX=0x%llX", RAXVAULE);
    if (!ReadProcessMemory(client.hProcess, LPCVOID(RAXVAULE + 0x18), &RAXVAULE, 8, 0)){PrintToConsole("基址读取失败了");return;}
    TORAX = RAXVAULE;//18 + 2BC = 当前视距大小
    PrintToConsole("0x18偏移 = 0x%llX ", RAXVAULE);
    if (!ReadProcessMemory(client.hProcess, LPCVOID(RAXVAULE + 0x2A8), &RAXVAULE, 4, 0)){PrintToConsole("0X2AC偏移读取失败了");return;}
    //PrintToConsole("当前视距%F",maxx);
    PrintToConsole("0X2AC偏移 = 0x%llX", RAXVAULE);
    //distance =//新的视距大小
    WriteProcessMemory(client.hProcess, LPVOID(RAXVAULE + 0x24), &distanceValue, 4, 0);//最小视距限制RAX + 18 + 2A8 + 24
    WriteProcessMemory(client.hProcess, LPVOID(RAXVAULE + 0x28), &distanceValue, 4, 0);//最大视距限制RAX + 18 + 2A8 + 28
    PrintToConsole("修改成功！%F", distanceValue);
    //DWORD64  基址 = result, 基址值 = NULL,rax;
    //float maxx;
    //ReadProcessMemory(client.hProcess, LPCVOID(基址), &基址值, 8, 0);//RAX
    //rax =  基址 + 基址值 + 4;
    //PrintToConsole("RAX = %X", rax);

    //if (ReadProcessMemory(client.hProcess, LPCVOID(rax + 0x18), &rax, 8, 0))
    //{
    //    PrintToConsole("基址值读取失败了");
    //    return;
    //}
    //PrintToConsole("RAX+ 0x18 读取到的地址 %X ", rax);

    //if (ReadProcessMemory(client.hProcess, LPCVOID(rax + 0x2BC), &maxx, 4, 0))//当前视距RAX + 18 + 2BC
    //{
    //    PrintToConsole("0X2BC偏移读取失败了");
    //    return;
    //}
    //PrintToConsole("当前视距%F",maxx);
    /*
    //ReadProcessMemory(client.hProcess, LPCVOID(基址值 + 0x2A8), &基址值, 8, 0);

    //ReadProcessMemory(client.hProcess, LPCVOID(基址值 + 0x24), &基址值, 4, 0);//最小视距限制RAX + 18 + 2A8 + 24
    //ReadProcessMemory(client.hProcess, LPCVOID(基址值 + 0x28), &基址值, 4, 0);//最大视距限制RAX + 18 + 2A8 + 28

    //max = 4500;
    //WriteProcessMemory(client.hProcess, LPVOID(基址值 + 0x2BC), &max, 4, 0);
    //OutputDebug(L"地址%X  读到的数据 %F 偏移地址%X", 基址值, max);

*/



/*
//"F3 0F 5C 35 ?? ?? ?? ?? 0F 28" //时间
//"48 8B 05 ? ? ? ? 8B 0D ? ? ? ? BA ? ? ? ?" //视距
//PrintChat = 0x81B210                         //13.11.513.4169 "E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8B D0 45 33 C0"
//GetAttackCastDelay = 0x38D280                //13.11.513.4169 "E8 ? ? ? ? 48 8B CE F3 0F 11 83 ? ? ? ?"
//GetAttackDelay = 0x38D380                    //13.11.513.4169 "E8 ? ? ? ? 33 C0 F3 0F 11 83 ? ? ? ?"
//IsInhib = 0x247789                           //13.11.513.4169 "0F 8C ? ? ? ? 48 8B 6C 24 ? 4C 8B 7C 24 ? 4C 8B 74 24 ? 4C 8B 6C 24 ? 4C 8B 64 24 ? 48 3B FD 74 33 48 8D 77 10"
//IsHero = 0x2407C0                            //13.11.513.4169 "E8 ? ? ? ? 0F B6 B4 24 ? ? ? ?"
//IsTurret = 0x2406C0                          //13.11.513.4169 "40 53 48 83 EC 20 48 8B D9 48 85 C9 74 27"
//IsMinion = 0x240820                          //13.11.513.4169 "E8 ? ? ? ? 48 8B 0B F3 0F 10 41 ?"
//IsMissile = 0x240850                         //13.11.513.4169 "E8 ? ? ? ? 84 C0 74 0C 48 8B 17"
//IsAlive = 0x22A7C0                           //13.11.513.4169 "E8 ? ? ? ? 84 C0 74 35 48 8D 8F ? ? ? ?"
//GetBoundingRadius  = 0x1E6D90                //13.11.513.4169 "E8 ? ? ? ? 0F 28 F8 48 8B D3 48 8B CE"
//HudCastSpell  = 0x84CDE0                     //13.11.513.4169 "E8 ?? ?? ?? ?? 48 8B 5C 24 ?? C6 87 ?? ?? ?? ?? ?? C6 87 ?? ?? ?? ?? ??"
//HudIssueOrder  = 0x871000                    //13.11.513.4169 "44 88 44 24 ? 55 56 41 54"
//GetSpellState  = 0x6CB620                    //13.11.513.4169 "E8 ? ? ? ? 48 8B CE 44 8B F8"
//HeroManager = 0x21239E8                      //13.11.513.4169 "48 8B 0D ? ? ? ? 45 39 AE ? ? ? ?"
//HudInstance = 0x21238D8                      //13.11.513.4169 "48 8B 05 ? ? ? ? 48 69 D3 ? ? ? ?"
//TurretManager = 0x51CA860                    //13.11.513.4169 "48 89 0D ? ? ? ? 33 C9"
//MinionManager = 0x397E278                    //13.11.513.4169 "48 8B 0D ? ? ? ? E8 ? ? ? ? EB 07"
//MissileList = 0x51DC608                      //13.11.513.4169 "48 8B 0D ? ? ? ? 48 8D 54 24 ? E8 ? ? ? ? 48 8B 7C 24 "
//NetClient = 0x51E43C0                        //13.11.513.4169 "48 8B 0D ? ? ? ? 48 8D 55 D0 41 B0 01"
//ViewProjMatrix = 0x521BA80                   //13.11.513.4169 "48 8D 0D ? ? ? ? 0F 10 00"
//GameTime = 0x51CE730                         //13.11.513.4169 "F3 0F 5C 35 ?? ?? ?? ?? 0F 28 F8"
//ObjectManager = 0x21238C8                    //13.11.513.4169 "89 57 10 48 8B 0D ? ? ? ?"
//MinimapObject = 0x51CE790                    //13.11.513.4169  "48 8B 0D ? ? ? ? 0F 57 C0 4C 8B F2"
//Renderer = 0x5223F28                         //13.11.513.4169 "48 8B 0D ? ? ? ? 4C 8D 44 24 ? 48 8B D0"
//UnderMouseObject = 0x397E468                 //13.11.513.4169 "48 89 0D ? ? ? ? 48 8D 05 ? ? ? ? 48 89 01 33 D2"
//LocalPlayer = 0x51DC530                      //13.11.513.4169 "48 8B 3D ? ? ? ? 48 3B CF"
//*/
}

void WriteDistance(float distanceValue)
{
    //最大视距限制RAX + 18 + 2A8 + 28
    if (!WriteProcessMemory(client.hProcess, LPVOID(RAXVAULE + 0x28), &distanceValue, 4, 0))
    {
        PrintToConsole("最大视距限制修改失败", distanceValue);
        return;

    }
    //最小视距限制RAX + 18 + 2A8 + 24
    if (!WriteProcessMemory(client.hProcess, LPVOID(RAXVAULE + 0x24), &distanceValue, 4, 0))
    {
        PrintToConsole("最小视距限制修改失败", distanceValue);
        return ;
    }
    float SEE;
    
    if (!WriteProcessMemory(client.hProcess, LPVOID(TORAX + 0x2BC), &distanceValue, 4, 0))
    {
        PrintToConsole("当前视距限制修改失败", distanceValue);
        return;
    }
    ReadProcessMemory(client.hProcess, LPVOID(TORAX + 0x2BC), &SEE, 4,0);
    PrintToConsole("修改成功，当前 = %F 实际 = %F", distanceValue, SEE);
}

LRESULT APIENTRY NewProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes 虚拟键码
    if (uMsg == 261 && wParam == 115)//ALT + F4 立即终止程序
    {
        PrintToConsole("Alt + F4");
        //exit(0);  //退出
        abort();    //强制退出
        return TRUE;
    }
    if (uMsg == customMsg)
    {  
        PrintToConsole("自定义消息被触发 %d", wParam);
        switch (wParam)
        {
        case 1:

            break;

        default:
            break;
        }
        return TRUE;
    }
    //if ((wParam == VK_CONTROL) && (lParam & 0x80000000))//Ctrl被按下
    //{
    //    WPARAM key = (lParam >> 16) & 0xFF;//获取一起按下的键值
    //    PrintToConsole("CTRL+ %x",key);
    //    switch (key)
    //    {
    //    case 0x5A://Z
    //        PrintToConsole("CTRL + Z 扩大120");
    //        return TRUE;
    //        break;
    //    case 0x58://X
    //        PrintToConsole("CTRL + X 缩小120");
    //        return TRUE;
    //        break;
    //    default:
    //        break;
    //    }
    //    return CallWindowProc(client.oldProc, hwnd, uMsg, wParam, lParam);
    //}
    if (uMsg == WM_KEYDOWN)//256 = 按下某键
    {	
        if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)//和CTRL一起按下
        {
            switch (wParam)
            {
            case 0x5A://Z
                PrintToConsole("CTRL + Z = 正在扩大视距 当前 %F ", distanceValue);
                distanceValue += 120;
                //PrintToConsole("扩大为 %F", distanceValue);
                WriteDistance(distanceValue);
                return TRUE;
                break;
            case 0x58://X
                PrintToConsole("CTRL + X = 正在缩小视距 当前 %F", distanceValue);
                distanceValue -= 120;
                //PrintToConsole("缩小为 %F", distanceValue);
                WriteDistance(distanceValue);
                //PrintToConsole("CTRL + X 缩小120 = %F", distanceValue);
                return TRUE;
                break;
            case 0x53://S
                char valueString[64];
                sprintf_s(valueString, sizeof(valueString), "%.2f", distanceValue);
                WritePrivateProfileStringA("distance", "Value", valueString, ".\\distance.ini");
                PrintToConsole("CTRL + S 保存视距 = %F", distanceValue);
                return TRUE;
                break;
            default:
                break;
            }
        }
        else //正常按下
        {
            switch (wParam)//wParam = 按键代码
            {
            case VK_HOME://HOME
                tz();
                PrintToConsole("checkhome");
                break;
            case VK_F1:
                //LoadLibraryA("D://Program Files (x86)//LOL(26)//Game//R3nzSkin.dll");
                break;
            case VK_END:
                time();
                break;
            default:
                break;
            }
        }

    }
    //if (uMsg == 522)
    //{
    //    PrintToConsole("鼠标滚轮滚动");
    //    if (wParam > 0)
    //    {
    //        maxx -= 120;
    //        WriteProcessMemory(client.hProcess, LPVOID(see + 0x24), &maxx, 4, 0);//最小视距限制RAX + 18 + 2A8 + 24
    //        WriteProcessMemory(client.hProcess, LPVOID(see + 0x28), &maxx, 4, 0);//最大视距限制RAX + 18 + 2A8 + 28
    //    }
    //    else {
    //        maxx += 120;
    //        WriteProcessMemory(client.hProcess, LPVOID(see + 0x24), &maxx, 4, 0);//最小视距限制RAX + 18 + 2A8 + 24
    //        WriteProcessMemory(client.hProcess, LPVOID(see + 0x28), &maxx, 4, 0);//最大视距限制RAX + 18 + 2A8 + 28
    //    }
    //}
    return CallWindowProc(client.oldProc, hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI start(LPVOID lpParam)
{
    //获取设定的视距大小并赋值给distanceValue
    TCHAR buffer[256];
    GetPrivateProfileStringW(L"distance",L"Value",L"2250", buffer,256,L".\\distance.ini");
    wstring value(buffer);
    distanceValue = stof(std::wstring(value));
    PrintToConsole("MIN value%f", distanceValue);
    //wchar_t wPath[MAX_PATH];
    //GetCurrentDirectoryW(MAX_PATH, wPath);
    //runPath = wPath;
    //PrintToConsole("运行路径！%S", runPath.c_str());
    //加载换肤dll
    //https://api.github.com/repos/R3nzTheCodeGOD/R3nzSkin/releases/latest 
    //获取最新版本的R3NZ和本地做对比，如果不一样就下载新的替换旧的（尚不支持，因为可能需要翻墙，还需要进行解压缩操作）
    //JSON browser_download_url = 下载地址 "tag_name": "v3.0.9" =版本号
    method::loadR3nz();
    //寻找客户端窗口句柄
    Sleep(3344);
    while (client.hWnd == NULL)
    {
        if (client.hWnd != NULL){break;}
        else
        {
            client.hWnd = FindWindowW(L"RiotWindowClass", NULL);//"RiotWindowClass","League of Legends (TM) Client"
            PrintToConsole("Finding = %d", client.hWnd);
            Sleep(1314);
        }
    }
    //获取程序ID和句柄
    client.pid = GetCurrentProcessId();
    client.hProcess = GetCurrentProcess();
    PrintToConsole("hWnd == %d  ProcessID = %d", client.hWnd, client.pid);
    //子类化窗口 自定义处理消息
    client.oldProc = (WNDPROC)SetWindowLongPtrW(client.hWnd, GWL_WNDPROC, (LRESULT)NewProc);
    if (client.oldProc == NULL)
    {
        PrintToConsole("SetWindowLong Error = %d", GetLastError());
        //return 0;
    }
    else
    {
        PrintToConsole("SetWindowLong oid = %d", client.oldProc);
    }
    
    //获得模块起始地址和结束地址
    pair<DWORD64, DWORD64> addressRange = method::GetModuleAddressRange(GetModuleHandleW(nullptr));
    client.startAddress = addressRange.first;
    client.endAddress = addressRange.second;
    //PostMessage(client.hWnd, customMsg, 1, 0);
    //const string timeTeZhengMa = "F3 0F 5C 35 ?? ?? ?? ?? 0F 28 F8";
    DWORD64 result = method::LocateSignature(client.hProcess, "F3 0F 5C 35 ?? ?? ?? ?? 0F 28 F8", client.startAddress, client.endAddress, 4);
    if (result) {
        PrintToConsole("TIME-特征码搜索到的地址0x%llX", result);
    }
    else {
        PrintToConsole("TIME-特征码没读取到地址");
        return 0 ;
    }
    DWORD64 基址, 基址值;
    ReadProcessMemory(client.hProcess, LPCVOID(result), &基址值, 4, 0);//RAX
    PrintToConsole("TIME-基址值 = 0x%llX", 基址值);
    基址 = result + 基址值 + 4;//基址的计算结果
    PrintToConsole("TIME-计算真正基址的值 = 0x%llX", 基址);
    float times;
    ReadProcessMemory(client.hProcess, LPCVOID(基址), &times, 4, 0);
    while (times <= 0)
    {
        Sleep(1314);
        ReadProcessMemory(client.hProcess, LPCVOID(基址), &times, 4, 0);
    }
    //寻找视距基址
    //DWORD64 result = NULL;// = method::LocateSignature(client.hProcess, "48 8B 05 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? BA", client.startAddress, client.endAddress, 3);
    //while (result == NULL)
    //{
    //    Sleep(2488);
    //    result = method::LocateSignature(client.hProcess, "48 8B 05 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? BA", client.startAddress, client.endAddress, 3);
    //}
    //计算基址值
    //DWORD64 RAX;
    //ReadProcessMemory(client.hProcess, LPCVOID(result), &RAXVAULE, 4, 0);//RAX
    //PrintToConsole("基址值 = 0x%llX", RAXVAULE);
    //RAX = result + RAXVAULE + 4;//基址的计算结果
    //PrintToConsole("计算真正基址的值 = 0x%X", RAX);
    ////PrintToConsole("起始地址 = 0x%llX 结束地址 = 0x%llX 大小 = %X", client.startAddress, client.endAddress, client.endAddress - client.startAddress);
    //if (!ReadProcessMemory(client.hProcess, (LPCVOID)RAX, &RAXVAULE, 8, 0))
    //{
    //    PrintToConsole("基址值RAX读取失败了");
    //    return 0;
    //}
    //PrintToConsole("基址值RAX=0x%llX", RAXVAULE);
    //if (!ReadProcessMemory(client.hProcess, LPCVOID(RAXVAULE + 0x18), &RAXVAULE, 8, 0))
    //{
    //    PrintToConsole("基址读取失败了");
    //    return 0;
    //}
    //PrintToConsole("0x18偏移 = 0x%llX ", RAXVAULE);
    //if (!ReadProcessMemory(client.hProcess, LPCVOID(RAXVAULE + 0x2A8), &RAXVAULE, 4, 0))
    //{
    //    PrintToConsole("0X2AC偏移读取失败了");
    //    return 0;
    //}
    //PrintToConsole("0X2AC偏移 = 0x%llX", RAXVAULE);
    //WriteProcessMemory(client.hProcess, LPVOID(RAXVAULE + 0x24), &distanceValue, 4, 0);//最小视距限制RAX + 18 + 2A8 + 24
    //WriteProcessMemory(client.hProcess, LPVOID(RAXVAULE + 0x28), &distanceValue, 4, 0);//最大视距限制RAX + 18 + 2A8 + 28
    //PrintToConsole("修改成功！%F CTRL +Z放大 +X缩小 +S保存", distanceValue);
    PrintToConsole("TIME = %f", times);
    tz();
    //DWORD64 timeAddress = time();
    //while (timeAddress == NULL)
    //{
    // Sleep(1314);
    // timeAddress = time();
    //}
    //float times;
    //ReadProcessMemory(client.hProcess, LPCVOID(timeAddress), &times, 4, 0);
    //PrintToConsole("游戏载入 运行时间 = %f", time);
    //while (times < 1)
    //{
    // PrintToConsole("等待游戏载入 %F", times);
    // Sleep(1314);
    // ReadProcessMemory(client.hProcess, LPCVOID(timeAddress), &times, 4, 0);
    //}
    //PrintToConsole("游戏载入 运行时间 = %f", time);
    //tz();

    return 0;
}

// 原始函数指针类型
HMODULE dinput;
typedef void (*OriginalFunctionType)();
OriginalFunctionType DirectInput8, DllCanUnload, DllGetClass, DllRegister, DllUnregister, GetdfDIJoy;
string fixedPath;
BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hModule);
	//setlocale(LC_ALL, ".utf8");
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // 获取系统目录路径
        char systemDir[MAX_PATH];
        if (GetSystemDirectoryA(systemDir, MAX_PATH) == 0) {fixedPath = "C:\\Windows\\System32\\dinput8.dll"; /* 获取失败，使用固定路径*/}
        else{fixedPath = string(systemDir) + "\\dinput8.dll";/* 获取成功，拼接路径*/}
        dinput = LoadLibraryA(fixedPath.c_str());// 加载目标 DLL
        if (dinput == NULL){break;}
        DirectInput8 = reinterpret_cast<OriginalFunctionType>(GetProcAddress(dinput,"DirectInput8Create"));DllCanUnload = reinterpret_cast<OriginalFunctionType>(GetProcAddress(dinput, "DllCanUnloadNow"));DllGetClass = reinterpret_cast<OriginalFunctionType>(GetProcAddress(dinput, "DllCanUnloadNow"));DllRegister = reinterpret_cast<OriginalFunctionType>(GetProcAddress(dinput, "DllCanUnloadNow"));DllUnregister = reinterpret_cast<OriginalFunctionType>(GetProcAddress(dinput, "DllCanUnloadNow"));GetdfDIJoy = reinterpret_cast<OriginalFunctionType>(GetProcAddress(dinput, "DllCanUnloadNow"));
        CreateConsole();
        CreateThread(NULL, NULL, start, NULL, NULL, NULL);
        PrintToConsole("dinput8.dll劫持成功");
        break;
    case DLL_PROCESS_DETACH:
        // DLL 卸载时释放控制台资源
        if (hConsole != nullptr)
        {
            CloseHandle(hConsole);
            hConsole = nullptr;
        }
        break;
    }
    return TRUE;
}

#define Direct DirectInput8Create
EXTERN_C __declspec(dllexport)void  Direct() {
    DirectInput8();
    return;
}
//#define UnLoad DllCanUnloadNow
#pragma comment(linker, "/EXPORT:DllCanUnloadNow=Dilraba,PRIVATE")
EXTERN_C __declspec(dllexport)void  Dilraba() {
    DllCanUnload();
    return;
}
//#define GetClass DllGetClassObject
#pragma comment(linker, "/EXPORT:DllGetClassObject=Kiku,PRIVATE")
EXTERN_C __declspec(dllexport)void  Kiku() {
    DllGetClass();
    return;
}
#define RegServer DllRegisterServer
EXTERN_C __declspec(dllexport)void  RegServer() {
    DllRegister();
    return;
}
#define UnServer DllUnregisterServer
EXTERN_C __declspec(dllexport)void  UnServer() {
    DllUnregister();
    return;
}
#define Joystick GetdfDIJoystick
EXTERN_C __declspec(dllexport)void  Joystick() {
    GetdfDIJoy();
    return;
}

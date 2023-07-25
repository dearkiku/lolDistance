#pragma once
#include <string>
using namespace std;

#define GWL_WNDPROC (-4)
static HANDLE hConsole = nullptr;
static wstring runPath;
struct clientInfo {
    DWORD pid;
    HWND hWnd;
    WNDPROC oldProc;
    HANDLE hProcess;
    DWORD64 startAddress;
    DWORD64 endAddress;
};
//格式化并将传递进来的参数用字符串方式输出
void PrintToConsole(const char* format, ...);
void CreateConsole();
namespace method {
    BOOL WriteAddressBin(HANDLE hProcess, DWORD64 lpBaseAddress, DWORD64 lpBuffer);
    //读取指定内存区域数据 返回是否成功
    //打开的进程句柄 需要读的内存地址 存储读到数据的指针 读的地址大小(可空) 偏移...(可空 或一个及以上)
    BOOL ReadAddressBin(HANDLE hProcess, LPCVOID lpBaseAddress, LPCVOID lpBuffer, size_t size, int offset, ...);

    //载入当前目录下的R3nzSkin.dll
    void loadR3nz();
    //void findMemory(HANDLE hProcess,char*Code,DWORD64 Start,DWORD64 End);
    //获取模块的范围
    pair<DWORD64, DWORD64> GetModuleAddressRange(HMODULE hModule);
    //对比两个数组是否相同 支持用0XFF表示模糊码
    BOOL CompareArrays(const vector<BYTE>& TZM, const vector<BYTE>& NC);
    //将带空格和??的特征码转换成字节数组并用FF替代??
    vector<unsigned char> ConvertStringToByteArray(const std::string& featureCode);
    //在指定的范围内搜索特征码，并返回特征码+偏移所在的内存位置
    DWORD64 LocateSignature(HANDLE hProcess, const string& tezhengma, DWORD64 startAddress, DWORD64 endAddress, int offset);
}

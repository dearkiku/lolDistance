#include <Windows.h>
#include <winnt.h>
#include <vector>
#include <Psapi.h>
#include "method.h"

using namespace std;

void PrintToConsole(const char* format, ...)
{
    // 创建一个可变参数列表
    va_list args;
    va_start(args, format);

    // 计算格式化字符串后的长度
    int length = _vscprintf(format, args) + 5 + 1; // 添加 5 个字符的长度用于存储标记

    // 分配缓冲区来保存格式化后的字符串
    char* buffer = new char[length];

    // 添加标记到格式化后的字符串
    snprintf(buffer, length, "[101] %s", format);

    // 格式化字符串
    vsnprintf(buffer + 5, length - 5, format, args);

    // 将格式化后的字符串输出到控制台
    printf("%s\n", buffer);

    // 释放缓冲区和可变参数列表
    delete[] buffer;
    va_end(args);
}

// 创建并初始化控制台
void CreateConsole()
{
    // 分配控制台
    AllocConsole();
    // 获取标准输出流句柄
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    // 将标准输出流句柄重定向到控制台
    hConsole = CreateFileW(L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hConsole != INVALID_HANDLE_VALUE)
    {
        SetStdHandle(STD_OUTPUT_HANDLE, hConsole);

        // 使用 freopen_s 替代 freopen
        FILE* pConsole;
        freopen_s(&pConsole, "CONOUT$", "w", stdout);
        freopen_s(&pConsole, "CONOUT$", "w", stderr);
    }
}

BOOL method::WriteAddressBin(HANDLE hProcess, DWORD64 lpBaseAddress,DWORD64 lpBuffer){
   //dwProcessId 进程ID , lpBaseAddress 内存地址, lpBuffer 写入的数据
    return  WriteProcessMemory(hProcess,(LPVOID)lpBaseAddress, &lpBuffer, sizeof(lpBuffer),NULL);
}
BOOL method::ReadAddressBin(HANDLE hProcess,LPCVOID lpBaseAddress, LPCVOID lpBuffer,size_t size,int offset, ...) {
    if (size == NULL)
    {
        size = sizeof(&lpBuffer);
    }
    if (offset == NULL)//没有偏移 直接读取
    {
        return  ReadProcessMemory(hProcess, (LPCVOID)lpBaseAddress, &lpBuffer, size, NULL);
    }
    BOOL RET;//返回值
    int tp_offsets = 0;//接受可变参数的临时参数
    va_list VA_OFFSETS;//存储可变参数的列表
    LPCVOID address =  lpBaseAddress, temp = lpBaseAddress;//第一次：将基址存入 之后：将读取到的地址放到这里
    DWORD64 ofs;//存储根据地址读到的地址
    va_start(VA_OFFSETS, offset);//初始化列表
    while (tp_offsets != NULL)
    {
        //首次：读取基址+0偏移 之后 读取到的地址+偏移 循环
        RET = ReadProcessMemory(hProcess, address, &ofs, size, NULL);
        if (!RET)
        {
            //不能继续从address中获取数据了，将当前的address返回给调用者
            lpBuffer = &address;
            return FALSE;
        }
        temp = address;
        tp_offsets = va_arg(VA_OFFSETS, int);
        if (tp_offsets != NULL)
        {
            address = LPCVOID(ofs + tp_offsets);//将读到的地址+偏移赋值给 address 继续读取
        }
        else
        {
            break;
        }
    }
    va_end(VA_OFFSETS);//清理
    //最后一个偏移=temp 需要直接读地址
    return  ReadProcessMemory(hProcess, temp, &lpBuffer, size, NULL);
}

pair<DWORD64, DWORD64> method::GetModuleAddressRange(HMODULE hModule) {
    MODULEINFO moduleInfo;
    if (GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo))) {
        DWORD_PTR baseAddress = reinterpret_cast<DWORD_PTR>(moduleInfo.lpBaseOfDll);
        DWORD moduleSize = moduleInfo.SizeOfImage;

        DWORD64 startAddress = static_cast<DWORD64>(baseAddress);
        DWORD64 endAddress = startAddress + static_cast<DWORD64>(moduleSize);

        return make_pair(startAddress, endAddress);
    }

    return make_pair(0, 0);
}

BOOL method::CompareArrays(const vector<BYTE>& TZM, const vector<BYTE>& NC) {
    DWORD size = static_cast<DWORD>(TZM.size());
    for (DWORD i = 0; i < size; ++i) {
        // 检查特征码和目标数组的对应元素是否相等
        if (TZM[i] != NC[i] && TZM[i] != 0xFF) {
            // 如果不相等，进行模糊匹配的判断
            // 如果特征码不是模糊匹配的占位符(0xFF)且与目标数组不匹配，则返回FALSE
            return FALSE;
        }
    }
    return TRUE;
}

vector<unsigned char> method::ConvertStringToByteArray(const std::string& featureCode) {
    std::vector<unsigned char> byteArray;
    std::string byteStr;

    for (size_t i = 0; i < featureCode.length(); ++i) {
        if (featureCode[i] == ' ') {
            continue;  // Ignore spaces
        }
        else if (featureCode[i] == '?' && featureCode[i + 1] == '?') {
            byteArray.push_back(0xFF);
            i++;  // Skip the second '?'
        }
        else {
            byteStr = featureCode.substr(i, 2);
            unsigned char byte = std::stoi(byteStr, nullptr, 16);
            byteArray.push_back(byte);
            i++;  // Skip the second character of the byte
        }
    }

    return byteArray;
}

DWORD64 method::LocateSignature(HANDLE hProcess, const string& tezhengma, DWORD64 start, DWORD64 end, int offset) {
    vector<BYTE> tempFeature(4096, 0);
    vector<unsigned char> byteArray = ConvertStringToByteArray(tezhengma);
    DWORD size = static_cast<DWORD>(byteArray.size());
    while (start < end) {
        ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(start), tempFeature.data(), 4096, NULL);
        for (DWORD i = 0; i < 4096 - size; ++i) {
            if (CompareArrays(byteArray, vector<BYTE>(tempFeature.begin() + i, tempFeature.begin() + i + size))) {
                return  start + i + offset; // 找到匹配的特征码，返回特征码所在的内存地址+偏移
            }
        }
        start += 4096 - size; // 更新startAddress
    }
    return 0; // 如果未找到匹配的特征码，则返回0或适当的错误代码
}


HMODULE hModule = NULL;
void method::loadR3nz(){
        if (hModule == NULL)
        {
            if (GetFileAttributesW(L"R3nzSkin.dll") != INVALID_FILE_ATTRIBUTES)
            {
                hModule = LoadLibraryW(L"R3nzSkin.dll");
                PrintToConsole("R3nzSkin加载路径成功 hModule = %d",  hModule);
                return;
            }
            PrintToConsole("没有办法加载R3nzSkin.dll");
        }
}
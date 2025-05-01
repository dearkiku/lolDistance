#include <Windows.h>
#include <winnt.h>
#include <vector>
#include <Psapi.h>
#include "method.h"

/**
 * @brief 创建并初始化控制台窗口
 * @details 根据配置文件决定是否创建控制台窗口，并设置输出编码
 * @note 需要在程序初始化时调用
 */
void method::RedireceConsole()
{
	// 分配控制台
	AllocConsole();
	// 设置控制台输出编码为 UTF-8
	SetConsoleOutputCP(CP_UTF8);
	//// 输入编码
	//SetConsoleCP(CP_UTF8);

	// 获取标准输出流句柄
	// HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	// 将标准输出流句柄重定向到控制台
	hConsole = CreateFile(L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hConsole != INVALID_HANDLE_VALUE)
	{
		SetStdHandle(STD_OUTPUT_HANDLE, hConsole);
		FILE* pConsole;
		freopen_s(&pConsole, "CONOUT$", "w", stdout);
		freopen_s(&pConsole, "CONOUT$", "w", stderr);
	}
}

/**
 * @brief 打印宽字符串到控制台
 * @param format 带L前缀的格式化字符串
 * @param ... 可变参数
 * @note 自动处理控制台缓冲区大小
 */
void method::PrintToConsole(const wchar_t* format, ...)
{
	if (!Console)
	{
		return;
	}
	// 创建一个可变参数列表
	va_list args;
	va_start(args, format);

	// 计算格式化字符串后的长度
	int length = _vscwprintf(format, args) + 5 + 1; // 添加 5 个字符的长度用于存储标记

	// 分配缓冲区来保存格式化后的字符串
	wchar_t* buffer = new wchar_t[length];

	// 格式化字符串并添加标记
	swprintf(buffer, length, L"[101] ");
	vswprintf(buffer + 5, length - 5, format, args);

	// 将格式化后的字符串输出到控制台
	wprintf(L"%s\n", buffer);

	// 释放缓冲区和可变参数列表
	delete[] buffer;
	va_end(args);
}

std::pair<DWORD64, DWORD64> method::GetModuleAddressRange(HMODULE hModule) {
	MODULEINFO moduleInfo;
	if (GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo)))
	{
		DWORD_PTR baseAddress = reinterpret_cast<DWORD_PTR>(moduleInfo.lpBaseOfDll);
		DWORD moduleSize = moduleInfo.SizeOfImage;

		DWORD64 startAddress = static_cast<DWORD64>(baseAddress);
		DWORD64 endAddress = startAddress + static_cast<DWORD64>(moduleSize);

		return std::make_pair(startAddress, endAddress);
	}
	return std::make_pair(0, 0);
}

BOOL method::CompareArrays(const std::vector<BYTE>& TZM, const std::vector<BYTE>& NC) {
	DWORD size = static_cast<DWORD>(TZM.size());
	for (DWORD i = 0; i < size; ++i)
	{
		// 检查特征码和目标数组的对应元素是否相等
		if (TZM[i] != NC[i] && TZM[i] != 0xFF)
		{
			// 如果不相等，进行模糊匹配的判断
			// 如果特征码不是模糊匹配的占位符(0xFF)且与目标数组不匹配，则返回FALSE
			return FALSE;
		}
	}
	return TRUE;
}

std::vector<unsigned char> method::ConvertStringToByteArray(const std::string& featureCode) {
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

std::wstring ConfigPath = L".\\distance.ini";
DWORD64 method::LocateSignature(HANDLE hProcess, const std::string& maskedPattern, DWORD64 start, DWORD64 end, int offset) {
	std::vector<BYTE> tempFeature(4096, 0);
	std::vector<unsigned char> byteArray = ConvertStringToByteArray(maskedPattern);
	DWORD size = static_cast<DWORD>(byteArray.size());
	while (start < end) {
		ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(start), tempFeature.data(), 4096, NULL);
		for (DWORD i = 0; i < 4096 - size; ++i) {
			if (CompareArrays(byteArray, std::vector<BYTE>(tempFeature.begin() + i, tempFeature.begin() + i + size))) {
				return  start + i + offset; // 找到匹配的特征码，返回特征码所在的内存地址+偏移
			}
		}
		start += 4096 - size; // 更新startAddress
	}
	return 0; // 如果未找到匹配的特征码，则返回0或适当的错误代码
}

UINT method::GetIntPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault) {
	return GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, ConfigPath.c_str());
}

DWORD method::GetStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize) {
	return GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, ConfigPath.c_str());
}

BOOL method::WriteStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString) {
	return WritePrivateProfileString(lpAppName, lpKeyName, lpString, ConfigPath.c_str());
}

HMODULE hModule = NULL;
void method::SKINChangeDll()
{
	//判断是否不需要载入DLL
	int OffDllLoad = GetIntPrivateProfile(L"Config", L"OffDllLoad", 0);
	if (OffDllLoad == 1)
	{
		PrintToConsole(L"[提示] 已跳过DLL加载 Config.OffDllLoad = 1");
		return;
	}
	//判断句柄是否为0
	if (hModule == NULL)
	{
		//读取配置文件找到DLL
		TCHAR dllPath[MAX_PATH];
		GetStringPrivateProfile(L"Config", L"SKINChangeDll", L"R3nzSkin.dll", dllPath, MAX_PATH);
		PrintToConsole(L"[提示] DllPath = %s", dllPath);
		//判断DLL文件是否存在于相同的目录
		if (GetFileAttributes(dllPath) != INVALID_FILE_ATTRIBUTES)
		{
			hModule = LoadLibrary(dllPath);
			PrintToConsole(L"[提示] Dll加载成功 hModule = %d", hModule);
			return;
		}
		else
		{
			PrintToConsole(L"[警告] Dll指向的文件不存在，跳过加载");
		}
	}
}
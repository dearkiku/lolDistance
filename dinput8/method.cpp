#include <Windows.h>
#include <Psapi.h>
// ===================
#include "method.h"

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

void method::PrintToConsole(const wchar_t* format, ...)
{
	WCHAR buffer[512];
	WCHAR fullBuffer[512];

	va_list args;
	va_start(args, format);

	swprintf_s(fullBuffer, L"[101] ");
	_vsnwprintf_s(fullBuffer + 5, 507, _TRUNCATE, format, args);

	wprintf(L"%s\n", fullBuffer);

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

BOOL method::CompareArrays(const BYTE* source, const BYTE* target, DWORD size) {
	for (DWORD i = 0; i < size; ++i) {
		if (source[i] != target[i] && source[i] != 0xFF) return FALSE;
	}
	return TRUE;
}

int method::ConvertStringToByteArray(const std::string& featureCode, unsigned char* outBuffer, int maxSize) {
	int count = 0;
	for (size_t i = 0; i < featureCode.length() && count < maxSize; ++i) {
		if (featureCode[i] == ' ') {
			continue;
		}
		else if (featureCode[i] == '?' && featureCode[i + 1] == '?') {
			outBuffer[count++] = 0xFF;
			++i;
		}
		else {
			std::string byteStr = featureCode.substr(i, 2);
			outBuffer[count++] = (unsigned char)std::stoi(byteStr, nullptr, 16);
			++i;
		}
	}
	return count;
}

DWORD64 method::LocateSignature(HANDLE hProcess, const std::string& maskedPattern, DWORD64 start, DWORD64 end, int offset) {
	const int MAX_PATTERN_SIZE = 256;
	const int PAGE_SIZE = 4096;
	BYTE page[PAGE_SIZE] = { 0 };
	BYTE pattern[MAX_PATTERN_SIZE] = { 0 };

	int patternSize = ConvertStringToByteArray(maskedPattern, pattern, MAX_PATTERN_SIZE);
	if (patternSize <= 0) return 0;

	while (start < end) {
		ReadProcessMemory(hProcess, (LPCVOID)start, page, PAGE_SIZE, NULL);
		for (DWORD i = 0; i < PAGE_SIZE - patternSize; ++i) {
			if (CompareArrays(pattern, page + i, patternSize)) {
				return start + i + offset;
			}
		}
		start += (PAGE_SIZE - patternSize);
	}
	return 0;
}

// 配置文件路径
std::wstring distanceConfigPath = L".\\distance.ini";

UINT method::GetIntPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault) {
	return GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, distanceConfigPath.c_str());
}

DWORD method::GetStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize) {
	return GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, distanceConfigPath.c_str());
}

BOOL method::WriteStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString) {
	return WritePrivateProfileString(lpAppName, lpKeyName, lpString, distanceConfigPath.c_str());
}

// 载入的DLL句柄
HMODULE hModule = NULL;

void method::LoadDll()
{
	//判断是否不需要载入DLL 1=跳过 0=加载
	int OffDllLoad = GetIntPrivateProfile(L"Config", L"OffDllLoad", 1);
	if (OffDllLoad == 1)
	{
		PrintToConsole(L"[提示] 已跳过DLL加载 Config.OffDllLoad = 1");
		return;
	}
	//判断DLL句柄是否为0
	if (hModule == NULL)
	{
		//读取配置文件找到DLL
		TCHAR dllPath[MAX_PATH];
		GetStringPrivateProfile(L"Config", L"LoadDll", L"R3nzSkin.dll", dllPath, MAX_PATH);
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
#include <Windows.h>
#include <Psapi.h>
// ===================
#include "method.h"
static bool console = false;

void method::RedireceConsole()
{
	if (console)
	{
		// �����ظ���ʼ��
		return;
	}

	// �������̨
	AllocConsole();
	// ���ÿ���̨�������Ϊ UTF-8
	SetConsoleOutputCP(CP_UTF8);
	//// �������
	//SetConsoleCP(CP_UTF8);

	// ��ȡ��׼��������
	// HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	
	// ����׼���������ض��򵽿���̨
	hConsole = CreateFile(L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hConsole != INVALID_HANDLE_VALUE)
	{
		SetStdHandle(STD_OUTPUT_HANDLE, hConsole);
		FILE* pConsole;
		freopen_s(&pConsole, "CONOUT$", "w", stdout);
		freopen_s(&pConsole, "CONOUT$", "w", stderr);
		// ���������ն���ɫ
		//DWORD consoleMode;
		//GetConsoleMode(hConsole, &consoleMode);
		//SetConsoleMode(hConsole, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		console = true;
	}
}

void method::PrintToConsole(const wchar_t* format, ...)
{
	if (!console)
	{
		return;
	}
	// ��ɫ
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	WCHAR buffer[512]{};
	WCHAR fullBuffer[512];

	va_list args;
	va_start(args, format);

	swprintf_s(fullBuffer, L"[101] ");
	_vsnwprintf_s(fullBuffer + 5, 507, _TRUNCATE, format, args);

	wprintf(L"%s\n", fullBuffer);

	va_end(args);
	// �ָ�Ϊ��ɫ
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

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

BOOL method::CompareArrays(const BYTE* source, const BYTE* target, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		if (source[i] != target[i] && source[i] != 0xFF) return FALSE;
	}
	return TRUE;
}

size_t method::ConvertStringToByteArray(const std::string& featureCode, unsigned char* outBuffer, size_t maxSize) {
	size_t count = 0;
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
	// ���ģʽ����
	constexpr size_t MAX_PATTERN_SIZE = 256;
	// �ڴ�ҳ��С
	constexpr size_t PAGE_SIZE = 4096;
	// ��������ʼ��
	BYTE page[PAGE_SIZE] = { 0 };
	BYTE pattern[MAX_PATTERN_SIZE] = { 0 };
	// ���ַ���ģʽת��Ϊ�ֽ�����
	const size_t patternSize = ConvertStringToByteArray(maskedPattern, pattern, MAX_PATTERN_SIZE);
	// ���ģʽ��Ч��
	if (patternSize <= 0) 
	{
		return 0;
	}
	// ��ҳ�����ڴ�
	while (start < end) {
		// ��ȡ��ǰ�ڴ�ҳ
		ReadProcessMemory(hProcess, (LPCVOID)start, page, PAGE_SIZE, NULL);
		// �ڵ�ǰҳ��ƥ���ֽ�
		for (size_t i = 0; std::cmp_less(i, PAGE_SIZE - patternSize); ++i) {
			if (CompareArrays(pattern, page + i, patternSize)) {
				// ƥ��ɹ�������ƥ���ַ+ƫ����
				return start + i + offset;
			}
		}
		// �ƶ�����һҳ
		start += (PAGE_SIZE -patternSize);
	}
	return 0;
}

// �����ļ�·��
const WCHAR* distanceConfigPath = L".\\distance.ini";

UINT method::GetIntPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault) {
	return GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, distanceConfigPath);
}

DWORD method::GetStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize) {
	return GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, distanceConfigPath);
}

BOOL method::WriteStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString) {
	return WritePrivateProfileString(lpAppName, lpKeyName, lpString, distanceConfigPath);
}

// �����DLL���
HMODULE hModule = NULL;

void method::LoadDll()
{
	//�ж��Ƿ���Ҫ����DLL 1=���� 0=����
	int OffDllLoad = GetIntPrivateProfile(L"Config", L"OffDllLoad", 1);
	if (OffDllLoad == 1)
	{
		PrintToConsole(L"[��ʾ] ������DLL���� Config.OffDllLoad = 1");
		return;
	}
	//�ж�DLL����Ƿ�Ϊ0
	if (hModule == NULL)
	{
		//��ȡ�����ļ��ҵ�DLL
		TCHAR dllPath[MAX_PATH];
		GetStringPrivateProfile(L"Config", L"LoadDll", L"R3nzSkin.dll", dllPath, MAX_PATH);
		PrintToConsole(L"[��ʾ] DllPath = %s", dllPath);
		//�ж�DLL�ļ��Ƿ��������ͬ��Ŀ¼
		if (GetFileAttributes(dllPath) != INVALID_FILE_ATTRIBUTES)
		{
			hModule = LoadLibrary(dllPath);
			PrintToConsole(L"[��ʾ] Dll���سɹ� hModule = %d", hModule);
			return;
		}
		else
		{
			PrintToConsole(L"[����] Dllָ����ļ������ڣ���������");
		}
	}
}

void method::exit() {
	if (hModule)
	{
		FreeLibrary(hModule);
	}
	if (console)
	{
		FreeConsole();
	}
}
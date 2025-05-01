#include <Windows.h>
#include <winnt.h>
#include <vector>
#include <Psapi.h>
#include "method.h"

/**
 * @brief ��������ʼ������̨����
 * @details ���������ļ������Ƿ񴴽�����̨���ڣ��������������
 * @note ��Ҫ�ڳ����ʼ��ʱ����
 */
void method::RedireceConsole()
{
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
	}
}

/**
 * @brief ��ӡ���ַ���������̨
 * @param format ��Lǰ׺�ĸ�ʽ���ַ���
 * @param ... �ɱ����
 * @note �Զ��������̨��������С
 */
void method::PrintToConsole(const wchar_t* format, ...)
{
	if (!Console)
	{
		return;
	}
	// ����һ���ɱ�����б�
	va_list args;
	va_start(args, format);

	// �����ʽ���ַ�����ĳ���
	int length = _vscwprintf(format, args) + 5 + 1; // ��� 5 ���ַ��ĳ������ڴ洢���

	// ���仺�����������ʽ������ַ���
	wchar_t* buffer = new wchar_t[length];

	// ��ʽ���ַ�������ӱ��
	swprintf(buffer, length, L"[101] ");
	vswprintf(buffer + 5, length - 5, format, args);

	// ����ʽ������ַ������������̨
	wprintf(L"%s\n", buffer);

	// �ͷŻ������Ϳɱ�����б�
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
		// ����������Ŀ������Ķ�ӦԪ���Ƿ����
		if (TZM[i] != NC[i] && TZM[i] != 0xFF)
		{
			// �������ȣ�����ģ��ƥ����ж�
			// ��������벻��ģ��ƥ���ռλ��(0xFF)����Ŀ�����鲻ƥ�䣬�򷵻�FALSE
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
				return  start + i + offset; // �ҵ�ƥ��������룬�������������ڵ��ڴ��ַ+ƫ��
			}
		}
		start += 4096 - size; // ����startAddress
	}
	return 0; // ���δ�ҵ�ƥ��������룬�򷵻�0���ʵ��Ĵ������
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
	//�ж��Ƿ���Ҫ����DLL
	int OffDllLoad = GetIntPrivateProfile(L"Config", L"OffDllLoad", 0);
	if (OffDllLoad == 1)
	{
		PrintToConsole(L"[��ʾ] ������DLL���� Config.OffDllLoad = 1");
		return;
	}
	//�жϾ���Ƿ�Ϊ0
	if (hModule == NULL)
	{
		//��ȡ�����ļ��ҵ�DLL
		TCHAR dllPath[MAX_PATH];
		GetStringPrivateProfile(L"Config", L"SKINChangeDll", L"R3nzSkin.dll", dllPath, MAX_PATH);
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
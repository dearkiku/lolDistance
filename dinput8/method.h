#pragma once
#include <string>

inline BOOL Console = false;
static HANDLE hConsole = nullptr;
static std::wstring runPath;
/*
 * @brief �ͻ�����Ϣ�ṹ�壬�洢����Ϸ�ͻ�����صĸ��־���͵�ַ
 */
struct clientInfo {
	DWORD pid;                  // ����pid
	HWND hWnd;                  // ���ھ��
	WNDPROC lpPrevWndFunc;      // �ɵĴ��ڹ���
	HANDLE hProcess;            // ���̾��
	DWORD64 startAddress;       // ��ʼ��ַ
	DWORD64 endAddress;         // ������ַ
	// ----------------------------------
	HDC hDcid;                  // DC���
	int ThreadId;				// �߳�ID
};
//struct coordinates {
//	int x;
//	int y;
//};
//struct CardColer {
//	int Ready;	// ����
//	int Gold;
//	int Blue;
//	int Red;
//	int Curent; // ��ǰ
//	int Target; // Ŀ��
//	coordinates Coord;
//};
//struct KeyCode {
//	int vKey;	// �������
//	int sKey;	// ���ܼ�
//	int Gold;
//	int Blue;
//	int Red;
//	bool Lock;	// ѭ����
//};

namespace method {
	void PrintToConsole(const wchar_t* format, ...);
	void RedireceConsole();
	//���뵱ǰĿ¼�µ�ָ��dll
	void SKINChangeDll();
	//void findMemory(HANDLE hProcess,char*Code,DWORD64 Start,DWORD64 End);
	//��ȡģ��ķ�Χ
	std::pair<DWORD64, DWORD64> GetModuleAddressRange(HMODULE hModule);
	//�Ա����������Ƿ���ͬ ֧����0XFF��ʾģ����
	BOOL CompareArrays(const std::vector<BYTE>& TZM, const std::vector<BYTE>& NC);
	//�����ո��??��������ת�����ֽ����鲢��FF���??
	std::vector<unsigned char> ConvertStringToByteArray(const std::string& featureCode);
	// ��ָ���ķ�Χ�����������룬������������+ƫ�����ڵ��ڴ�λ��
	// ���̾�� �������ı� ��ʼ��ַ ������ַ ƫ��
	DWORD64 LocateSignature(HANDLE hProcess, const std::string& maskedPattern, DWORD64 startAddress, DWORD64 endAddress, int offset);
	UINT GetIntPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault);
	DWORD GetStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize);
	BOOL WriteStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString);
}

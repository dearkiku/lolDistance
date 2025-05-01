#pragma once
#include <string>

inline BOOL Console = false;
static HANDLE hConsole = nullptr;
static std::wstring runPath;
/*
 * @brief 客户端信息结构体，存储与游戏客户端相关的各种句柄和地址
 */
struct clientInfo {
	DWORD pid;                  // 进程pid
	HWND hWnd;                  // 窗口句柄
	WNDPROC lpPrevWndFunc;      // 旧的窗口过程
	HANDLE hProcess;            // 进程句柄
	DWORD64 startAddress;       // 起始地址
	DWORD64 endAddress;         // 结束地址
	// ----------------------------------
	HDC hDcid;                  // DC句柄
	int ThreadId;				// 线程ID
};
//struct coordinates {
//	int x;
//	int y;
//};
//struct CardColer {
//	int Ready;	// 就绪
//	int Gold;
//	int Blue;
//	int Red;
//	int Curent; // 当前
//	int Target; // 目标
//	coordinates Coord;
//};
//struct KeyCode {
//	int vKey;	// 虚拟键码
//	int sKey;	// 技能键
//	int Gold;
//	int Blue;
//	int Red;
//	bool Lock;	// 循环锁
//};

namespace method {
	void PrintToConsole(const wchar_t* format, ...);
	void RedireceConsole();
	//载入当前目录下的指定dll
	void SKINChangeDll();
	//void findMemory(HANDLE hProcess,char*Code,DWORD64 Start,DWORD64 End);
	//获取模块的范围
	std::pair<DWORD64, DWORD64> GetModuleAddressRange(HMODULE hModule);
	//对比两个数组是否相同 支持用0XFF表示模糊码
	BOOL CompareArrays(const std::vector<BYTE>& TZM, const std::vector<BYTE>& NC);
	//将带空格和??的特征码转换成字节数组并用FF替代??
	std::vector<unsigned char> ConvertStringToByteArray(const std::string& featureCode);
	// 在指定的范围内搜索特征码，并返回特征码+偏移所在的内存位置
	// 进程句柄 特征码文本 起始地址 结束地址 偏移
	DWORD64 LocateSignature(HANDLE hProcess, const std::string& maskedPattern, DWORD64 startAddress, DWORD64 endAddress, int offset);
	UINT GetIntPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault);
	DWORD GetStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize);
	BOOL WriteStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString);
}

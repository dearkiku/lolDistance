#pragma once
#include <string>
// 控制台句柄
static HANDLE hConsole = nullptr;
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
};

namespace method {

	/**
	* @brief 打印宽字符串到控制台
	* @param format 带L前缀的格式化字符串
	* @param ... 可变参数
	* @note 自动处理控制台缓冲区大小
	*/
	void PrintToConsole(const wchar_t* format, ...);

	/**
	* @brief 创建并初始化控制台窗口
	* @details 根据配置文件决定是否创建控制台窗口，并设置输出编码
	* @note 需要在程序初始化时调用
	*/
	void RedireceConsole();

	/*
	* @brief 载入指定的DLL文件
	* @details 读取配置文件中的DLL路径，如果DLL存在则加载，否则输出警告信息
	*/
	void LoadDll();

	void exit();
	
	/*
	* @brief 获取模块地址范围
	* @param hModule 模块句柄
	* @return 返回一个包含模块起始地址和结束地址的pair
	*/
	std::pair<DWORD64, DWORD64> GetModuleAddressRange(HMODULE hModule);
	
	/*
	* @brief 对比两个字节数组是否相同
	* @param source 来源数组
	* @param target 目标数组
	* @param size 数组大小
	* @return 如果两个数组相同或特征码为模糊匹配(0xFF)，返回TRUE；否则返回FALSE
	*/
	BOOL CompareArrays(const BYTE* a, const BYTE* b, size_t size);
	
	/*
	* @brief 将特征码字符串转换为字节数组
	* @param featureCode 特征码字符串，格式为 "XX XX ?? ??"
	* @param outBuffer 输出的字节数组缓冲区
	* @param maxSize 输出缓冲区的最大大小
	* @return 转换后的字节数组长度
	* @note 忽略空格和??，将??转换为0xFF
	*/	
	size_t ConvertStringToByteArray(const std::string& featureCode, unsigned char* outBuffer, size_t maxSize);
	
	/*
	* @brief 在指定的内存范围内查找特征码
	* @param hProcess 目标进程句柄
	* @param maskedPattern 特征码字符串，格式为 "XX XX ?? ??"
	* @param startAddress 起始地址
	* @param endAddress 结束地址
	* @param offset 偏移量
	* @return 如果找到特征码，返回特征码所在的内存地址+偏移；否则返回0
	*/
	DWORD64 LocateSignature(HANDLE hProcess, const std::string& maskedPattern, DWORD64 startAddress, DWORD64 endAddress, int offset);
	
	/*
	* @brief 从配置文件中读取整数值
	* @param lpAppName 节名称
	* @param lpKeyName 键名称
	* @param nDefault 默认值，如果键不存在则返回此值
	* @return 返回读取的整数值，如果读取失败则返回nDefault
	*/
	UINT GetIntPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault);

	/*
	* @brief 从配置文件中读取字符串
	* @param lpAppName 节名称
	* @param lpKeyName 键名称
	* @param lpDefault 默认值，如果键不存在则返回此值
	* @param lpReturnedString 用于存储返回的字符串
	* @param nSize lpReturnedString的大小
	* @return 返回实际读取的字符数，如果读取失败则返回0
	*/
	DWORD GetStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize);
	
	/*
	* @brief 写入字符串到配置文件
	* @param lpAppName 节名称
	* @param lpKeyName 键名称
	* @param lpString 要写入的字符串
	* @return 如果成功则返回非零值，否则返回零
	*/
	BOOL WriteStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString);
}

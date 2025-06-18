// dllmain.cpp : 定义 DLL 应用程序的入口点。
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
#include <Windows.h>
// ===================
#include <string>
#include <vector>
// ===================
#include <chrono>
#include <thread>
#include "method.h"
#include "Constant.h"

// 客户端结构
clientInfo client;
//当前视距大小
float distanceValue;
DWORD64 RCX, DIstanceOffsetAddress;
// 计时器
PTP_TIMER g_timer = nullptr;
/**
 * @brief 获取RCX寄存器的基地址
 * @return true 获取成功, false 获取失败
 * @note 通过特征码定位并计算RCX寄存器的值
 */
static bool GetRcxAddress() {

	// RCX特征码定位到地址
	DWORD64 rcxByteCode;
	rcxByteCode = method::LocateSignature(client.hProcess, RCX_SIGNATURE_CODE, client.startAddress, client.endAddress, 0);

	if (rcxByteCode) {
		method::PrintToConsole(L"[信息] RCX-特征码搜索到的地址：0x%llX", rcxByteCode);
	}
	else {
		method::PrintToConsole(L"[信息] RCX-特征码没读取到地址，可能特征码已过期");
		return false;
	}

	// 定位到mov指令地址
	DWORD64 movInstructionAddr = rcxByteCode + 0x3;
	method::PrintToConsole(L"[信息] 定位-> mov rcx,[0x%llX] ", movInstructionAddr);

	// 读取相对偏移量 RIP
	INT32 ripRelativeOffset = 0;
	if (!ReadProcessMemory(client.hProcess, (LPCVOID)(movInstructionAddr), &ripRelativeOffset, sizeof(ripRelativeOffset), 0)) {
		method::PrintToConsole(L"[信息] RCX-读取相对偏移量失败");
		return false;
	}
	method::PrintToConsole(L"[信息] RCX-读取到的相对偏移量 0x%X", ripRelativeOffset);

	// 计算下一条指令地址（当前指令地址+7字节）
	DWORD64 nextInstructionAddr = rcxByteCode + 0x7;
	method::PrintToConsole(L"[信息] RCX-下一条指令地址 0x%llX", nextInstructionAddr);

	// 计算目标地址
	DWORD64 targetAddr = nextInstructionAddr + ripRelativeOffset;
	method::PrintToConsole(L"[信息] RCX-计算得到的目标地址 0x%llX", targetAddr);

	// 读取RCX指针的值
	DWORD64 rcxValue = 0;
	if (!ReadProcessMemory(client.hProcess, (LPCVOID)targetAddr, &rcxValue, sizeof(rcxValue), 0)) {
		method::PrintToConsole(L"[信息] RCX-读取目标地址失败");
		return false;
	}
	method::PrintToConsole(L"[信息] RCX-指针值 0x%llX", rcxValue);

	// 获取最终RCX值（指针+0x18）
	if (!ReadProcessMemory(client.hProcess, (LPCVOID)(rcxValue + RCX_OFFSET_DEFAULT), &RCX, sizeof(RCX), 0)) {
		method::PrintToConsole(L"[信息] RCX-读取RCX+0x18失败");
		return false;
	}
	method::PrintToConsole(L"[信息] RCX-最终值 0x%llX", RCX);

	// RCX + 300
	if (!ReadProcessMemory(client.hProcess, LPCVOID(RCX + RCX_OFFSET_POINTER), &DIstanceOffsetAddress, sizeof(DIstanceOffsetAddress), 0))
	{
		method::PrintToConsole(L"[警告] DIstanceOffistBaseAddress获取失败");
		return false;
	}
	method::PrintToConsole(L"[信息] DIstanceOffistBaseAddress = 0x%llx", DIstanceOffsetAddress);
	return true;
}

/**
 * @brief 设置视距值
 * @param type 视距类型 (0=当前, 1=最大, 2=最小)
 * @param value 要设置的值
 * @return true 成功, false 失败
 */
 //static bool SetDistanceValue(int type, float value) {
 //	DWORD64 offset;
 //	switch (type) {
 //	case 0:
 //		offset = RCX_OFFSET_CURRENT;
 //		break;
 //	case 1:
 //		offset = RCX_OFFSET_MAX;
 //		break;
 //	case 2:
 //		offset = RCX_OFFSET_MIN;
 //		break;
 //	default: return false;
 //	}
 //	return WriteProcessMemory(client.hProcess, LPVOID(DIstanceOffsetAddress + offset), &value, sizeof(value), 0);
 //}

 /**
* @brief 读取视距值
* @param type 视距类型 (0=当前, 1=最大, 2=最小)
* @return 当前数值
*/
//static float GetDistanceValue(int type) {
//	float value = 0.0f;
//	DWORD64 offset;
//	switch (type) {
//	case 0:
//		offset = RCX_OFFSET_CURRENT;
//		break;
//	case 1:
//		offset = RCX_OFFSET_MAX;
//		break;
//	case 2:
//		offset = RCX_OFFSET_MIN;
//		break;
//	default: return value;
//	}
//	if (ReadProcessMemory(client.hProcess, LPVOID(DIstanceOffsetAddress + offset), &value, sizeof(value), 0))
//	{
//		method::PrintToConsole(L"[错误] 获取当前视距值失败 (错误代码: %d)", GetLastError());
//	}
//	return value;
//}

/**
* @brief 批量写入视距数据
* @param distanceValue 基础视距值
* @note 会同时设置最小视距、最大视距和当前视距
*/
static void WriteInsightData(const float& value)
{
	//最小视距限制
	if (!WriteProcessMemory(client.hProcess, LPVOID(DIstanceOffsetAddress + RCX_OFFSET_MIN), &value, sizeof(value), 0))
	{
		method::PrintToConsole(L"[错误] 最小视距限制修改失败 (错误代码: %d)", GetLastError());
	}
	else
	{
		method::PrintToConsole(L"[成功] 最小视距限制修改成功：", value);
	}
	// 将最大视距限制改为传参 + ZOOM_VALUE
	float maxDistance = value;// +ZOOM_VALUE;
	if (!WriteProcessMemory(client.hProcess, LPVOID(DIstanceOffsetAddress + RCX_OFFSET_MAX), &maxDistance, sizeof(maxDistance), 0))
	{
		method::PrintToConsole(L"[错误] 最大视距限制修改失败 (错误代码: %d)", GetLastError());
	}
	else
	{
		method::PrintToConsole(L"[成功] 最大视距限制修改成功：", maxDistance);
	}
	if (!WriteProcessMemory(client.hProcess, LPVOID(RCX + RCX_OFFSET_CURRENT), &value, sizeof(value), 0))
	{
		method::PrintToConsole(L"[错误] 当前视距限制修改失败 (错误代码: %d)", GetLastError());
	}
	else
	{
		method::PrintToConsole(L"[成功] 当前视距限制修改成功：", value);
	}
	float current = 0.0;
	ReadProcessMemory(client.hProcess, LPCVOID(RCX + RCX_OFFSET_CURRENT), &current, sizeof(current), 0);
	if (value == current)
	{
		method::PrintToConsole(L"[成功] 视距修改成功 当前：%F 内存：%F", value, current);
	}
	else
	{
		method::PrintToConsole(L"[错误] 视距修改失败 (错误代码: %d) 预期数据：%F 内存数据：%F", GetLastError(), value, current);
	}
	return;
}

/**
 * @brief 窗口过程函数,子类化窗口，接收窗消息
 * @param hwnd 窗口句柄
 * @param uMsg 消息类型
 * @param wParam 附加消息参数
 * @param lParam 附加消息参数
 * @return 消息处理结果
 * @details 处理以下输入:
 *          - Ctrl+Z: 扩大视距
 *          - Ctrl+X: 缩小视距
 *          - Ctrl+S: 保存视距配置
 *          - Ctrl+鼠标滚轮: 调整视距
 */
static LRESULT APIENTRY NewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes 虚拟键码
	if (uMsg == WM_SYSKEYUP && wParam == VK_F4)
	{
		// ALT + F4 -> 立即终止程序
		method::PrintToConsole(L"[信息] Alt + F4 -> ExitProcess(1)");
		method::PrintToConsole(L"[信息] 如果是云顶之弈或斗魂竞技场，需要点击退出游戏才行，不然就要等所有人的游戏都结束才能重新开始");
		// exit(0);		// 正常终止程序，返回退出码
		// abort();		// 触发异常终止，通常生成 SIGABRT 信号，在 Windows 上表现为崩溃
		ExitProcess(1); // 强制终止当前进程（包括所有线程），并返回退出码 
		return TRUE;
	}
	// 按下键盘
	else if (uMsg == WM_KEYDOWN)
	{
		// CTRL 按下
		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			switch (wParam)
			{
			case 'Z':
			{
				method::PrintToConsole(L"[信息] CTRL + Z 扩大视距");
				distanceValue += ZOOM_VALUE;
				WriteInsightData(distanceValue);
				return TRUE;
			}
			break;
			case 'X':
			{
				method::PrintToConsole(L"[信息] CTRL + X 缩小视距");
				distanceValue -= ZOOM_VALUE;
				WriteInsightData(distanceValue);
				return TRUE;
			}
			break;
			case 'S':
			{
				// 使用 C++20的新特性 格式化float // 生成的文件体积太大，已废弃
				//std::wstring wideStr = std::format(L"{:.2f}", distanceValue);
				WCHAR wideStr[32];
				swprintf_s(wideStr, L"%.2f", distanceValue);
				if (method::WriteStringPrivateProfile(L"Config", L"DistanceValue", wideStr))
				{
					method::PrintToConsole(L"[成功] CTRL + S 保存视距 保存的值：%F  %s", distanceValue, wideStr);
				}
				else
				{
					method::PrintToConsole(L"[失败] CTRL + S 保存视距 错误代码: %d", GetLastError());
				}
				return TRUE;
			}
			break;
			case VK_HOME:
			{
				if (RCX != NULL)
				{
					break;
				}
				if (!GetRcxAddress())
				{
					break;
				}
				// 修改视距
				WriteInsightData(distanceValue);
			}
			break;
			default:
				break;
			}
		}
	}
	else if (uMsg == WM_MOUSEWHEEL)//鼠标滚轮被滚动
	{
		if (GetKeyState(VK_CONTROL) & 0x8000)//如果按下了CTRL键
		{
			// 获取滚轮滚动的值
			int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			// 鼠标向上滚动 增加视距（符合游戏中的方式: 向上滚动靠近英雄 向下滚动远离英雄）
			if (delta < 0)
			{
				// 表示滚轮向后滚动（通常是向下滚动）
				method::PrintToConsole(L"扩大视距->滚轮 当前 %F ", distanceValue);
				distanceValue += ZOOM_VALUE;
				WriteInsightData(distanceValue);
			}
			else if (delta > 0)
			{
				// 表示滚轮向前滚动（通常是向上滚动）
				method::PrintToConsole(L"缩小视距->滚轮 当前 %F ", distanceValue);
				distanceValue -= ZOOM_VALUE;
				WriteInsightData(distanceValue);
			}
		}
	}
	return CallWindowProc(client.lpPrevWndFunc, hWnd, uMsg, wParam, lParam);
}

/**
 * @brief 初始化线程函数
 * @param lpParam 线程参数(暂未使用)
 * @return 始终返回0
 * @details 执行以下初始化操作:
 *          1. 加载指定DLL
 *          2. 读取配置文件
 *          3. 查找游戏窗口
 *          4. 子类化窗口
 *          5. 定位游戏内存地址
 *          6. 设置初始视距
 */
 //static DWORD WINAPI Initialize(LPVOID lpParam)
 //{
 //	method::PrintToConsole(L"[提示] 开始初始化");
 //	// 尝试加载指定DLL
 //	// 如果OffDistance的数值为1，那么就不修改视距 默认为0
 //	int OffDistance = method::GetIntPrivateProfile(L"Config", L"OffDistance", 0);
 //	if (OffDistance == 1)
 //	{
 //		method::PrintToConsole(L"[提示] 已跳过视距修改 Config->OffDistance = 1");
 //		return 0;
 //	}
 //	// 获取设定的视距大小并赋值给distanceValue TCHAR buffer[8] 视距在7位数字以内 加上末尾\0空字符所以填8 不需要太多空间
 //	TCHAR buffer[8];
 //	method::GetStringPrivateProfile(L"Config", L"DistanceValue", L"2250", buffer, 8);
 //	// std::wstring value(buffer);
 //	WCHAR* endPtr = nullptr;
 //	float result = wcstof(buffer, &endPtr);
 //
 //	if (endPtr == buffer || *endPtr != L'\0') {
 //		distanceValue = 2250.0f;
 //		method::PrintToConsole(L"[警告] Config->DistanceValue 配置无效，使用默认值 2250.0");
 //	}
 //	else {
 //		distanceValue = result;
 //		method::PrintToConsole(L"[信息] 配置中的视距: %f", distanceValue);
 //	}
 //	// distanceValue = stof(std::wstring(value));
 //	//try {
 //	//	// 读取配置中的视距大小转成浮点数
 //	//	distanceValue = std::stof(value);
 //	//	method::PrintToConsole(L"[信息] 配置中的视距: %f{Config.DistanceValue}", distanceValue);
 //	//}
 //	//catch (const std::invalid_argument& e) {
 //	//	std::wstring errorMessage = std::wstring(e.what(), e.what() + strlen(e.what()));
 //	//	method::PrintToConsole(L"[信息] {Config.DistanceValue}是无效的参数: %s", errorMessage.c_str());
 //	//	distanceValue = 2250.0;
 //	//}
 //	//catch (const std::out_of_range& e) {
 //	//	std::wstring errorMessage = std::wstring(e.what(), e.what() + strlen(e.what()));
 //	//	method::PrintToConsole(L"[信息] {Config.DistanceValue}数值超出范围: %s", errorMessage.c_str());
 //	//	distanceValue = 2250.0;
 //	//}
 //
 //
 //
 //	// 提示视距值
 //	// 等待一下就开始循环寻找客户端窗口句柄
 //	std::this_thread::sleep_for(std::chrono::milliseconds(1314));
 //	while (client.hWnd == NULL)//如果窗口句柄是NULL就继续循环
 //	{
 //		//寻找窗口句柄
 //		client.hWnd = FindWindowW(Client_NAME, nullptr);//"RiotWindowClass","League of Legends (TM) Client"
 //		// method::PrintToConsole(L"FindWindowW = %d", client.hWnd);
 //		std::this_thread::sleep_for(std::chrono::milliseconds(1314));
 //	}
 //	// 获取程序ID和句柄
 //	client.pid = GetCurrentProcessId();
 //	client.hProcess = GetCurrentProcess();
 //	method::PrintToConsole(L"[信息] 窗口句柄: %d", client.hWnd);
 //	method::PrintToConsole(L"[信息] 进程ID: %d", client.pid);
 //	method::PrintToConsole(L"[信息] 进程句柄: %d", client.hProcess);
 //	//子类化窗口 自定义处理消息
 //	client.lpPrevWndFunc = (WNDPROC)SetWindowLongPtrW(client.hWnd, GWLP_WNDPROC, (LRESULT)NewProc);
 //	if (client.lpPrevWndFunc == NULL)
 //	{
 //		//无法接管消息 输出错误结果
 //		method::PrintToConsole(L"[错误] SetWindowLong - (错误代码：%d)", GetLastError());
 //		method::PrintToConsole(L"[错误] 无法子类化游戏客户端，快捷键已失效");
 //		//return 0;
 //	}
 //	else
 //	{
 //		method::PrintToConsole(L"[成功] SetWindowLong - %d", client.lpPrevWndFunc);
 //	}
 //	//注册一个自定义消息并输出消息值
 //	//customMsg = RegisterWindowMessageW(L"Kiku");
 //	//method::PrintToConsole(L"MessageId = %d", customMsg);
 //	// 获得模块起始地址和结束地址
 //	std::pair<DWORD64, DWORD64> addressRange = method::GetModuleAddressRange(GetModuleHandleW(nullptr));
 //	client.startAddress = addressRange.first;
 //	client.endAddress = addressRange.second;
 //	method::PrintToConsole(L"[信息] 客户端模块起始地址 = 0x%llX", client.startAddress);
 //	method::PrintToConsole(L"[信息] 客户端模块结束地址 = 0x%llX", client.endAddress);
 //	int Manual = method::GetIntPrivateProfile(L"Config", L"Manual", 0);
 //	if (Manual == 1)
 //	{
 //		method::PrintToConsole(L"[提示] 当前为手动视距修改，泉水中按下CTRL + HOME修改视距");
 //		return true;
 //	}
 //	else
 //	{
 //		method::PrintToConsole(L"[提示] 当前为自动视距修改，识别到开始时自动修改");
 //	}
 //
 //	// 定位时间地址
 //	DWORD64 timeAddress = method::LocateSignature(client.hProcess, TIME_SIGNATURE_CODE, client.startAddress, client.endAddress, 4);
 //	if (timeAddress)
 //	{
 //		method::PrintToConsole(L"[信息] 特征码搜索到的时间地址 = 0x%llX", timeAddress);
 //		DWORD64 基址, 基址值 = 0;
 //		ReadProcessMemory(client.hProcess, LPCVOID(timeAddress), &基址值, 4, 0);
 //		//输出时间基址值
 //		method::PrintToConsole(L"[信息] 特征定位时间基址 = 0x%llX", 基址值);
 //		基址 = timeAddress + 基址值 + 4;//基址的计算结果
 //		method::PrintToConsole(L"[信息] 计算真正时间基址 = 0x%llX", 基址);
 //		float times = 0.0;
 //		ReadProcessMemory(client.hProcess, LPCVOID(基址), &times, 4, 0);
 //		//循环读取时间基址，直到时间不为0
 //		while (times <= 0)
 //		{
 //			ReadProcessMemory(client.hProcess, LPCVOID(基址), &times, 4, 0);
 //			method::PrintToConsole(L"[信息] 循环读到的时间 = %f", times);
 //			std::this_thread::sleep_for(std::chrono::milliseconds(1314));
 //		}
 //		method::PrintToConsole(L"[信息] 当前游戏时间 = %f", times);
 //	}
 //	else
 //	{
 //		method::PrintToConsole(L"[警告] 特征码没读取到时间地址，可能特征码已过期，可使用手动修改");
 //		return false;
 //	}
 //	// 获取RCX地址
 //	if (!GetRcxAddress())
 //	{
 //		// 失败则无法继续
 //		return false;
 //	}
 //	WriteInsightData(distanceValue);
 //	return true;
 //}

 /**
  * @brief 定时器回调函数
  * @param instance 线程池回调实例
  * @param context 上下文参数
  * @param timer 线程池定时器
  * @details 当定时器触发时，关闭定时器并初始化
  */
static VOID CALLBACK TimerCallback(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_TIMER timer)
{
	if (g_timer) {
		CloseThreadpoolTimer(g_timer);
		g_timer = nullptr;
	}
	method::PrintToConsole(L"[提示] 开始初始化");
	// 获取设定的视距大小并赋值给distanceValue TCHAR buffer[8] 视距在7位数字以内(九百万) 加上末尾\0空字符所以填8 不需要太多空间
	TCHAR buffer[8];
	method::GetStringPrivateProfile(L"Config", L"DistanceValue", L"2250", buffer, 8);
	WCHAR* endPtr = nullptr;
	float result = wcstof(buffer, &endPtr);

	if (endPtr == buffer || *endPtr != L'\0') {
		distanceValue = 2250.0f;
		method::PrintToConsole(L"[警告] Config->DistanceValue 配置无效，使用默认值 2250.0");
	}
	else {
		distanceValue = result;
		// 提示视距值
		method::PrintToConsole(L"[信息] 配置中的视距: %f", distanceValue);
	}

	// 等待一下就开始循环寻找客户端窗口句柄
	std::this_thread::sleep_for(std::chrono::milliseconds(1314));
	while (client.hWnd == NULL)//如果窗口句柄是NULL就继续循环
	{
		//寻找窗口句柄
		client.hWnd = FindWindow(Client_NAME, NULL);
		//"RiotWindowClass","League of Legends (TM) Client"
		// method::PrintToConsole(L"FindWindowW = %d", client.hWnd);
		std::this_thread::sleep_for(std::chrono::milliseconds(1314));
	}
	// 获取程序ID和句柄
	client.pid = GetCurrentProcessId();
	client.hProcess = GetCurrentProcess();
	method::PrintToConsole(L"[信息] 窗口句柄: %d", client.hWnd);
	method::PrintToConsole(L"[信息] 进程ID: %d", client.pid);
	method::PrintToConsole(L"[信息] 进程句柄: %d", client.hProcess);
	//子类化窗口 自定义处理消息
	client.lpPrevWndFunc = (WNDPROC)SetWindowLongPtrW(client.hWnd, GWLP_WNDPROC, (LRESULT)NewProc);
	if (client.lpPrevWndFunc == NULL)
	{
		//无法接管消息 输出错误结果
		method::PrintToConsole(L"[错误] SetWindowLong - (错误代码：%d)", GetLastError());
		method::PrintToConsole(L"[错误] 无法子类化游戏客户端，快捷键已失效");
		//return 0;
	}
	else
	{
		method::PrintToConsole(L"[成功] SetWindowLong - %d", client.lpPrevWndFunc);
	}
	// 获得模块起始地址和结束地址
	std::pair<DWORD64, DWORD64> addressRange = method::GetModuleAddressRange(GetModuleHandleW(nullptr));
	client.startAddress = addressRange.first;
	client.endAddress = addressRange.second;
	method::PrintToConsole(L"[信息] 客户端模块起始地址 = 0x%llX", client.startAddress);
	method::PrintToConsole(L"[信息] 客户端模块结束地址 = 0x%llX", client.endAddress);
	int Manual = method::GetIntPrivateProfile(L"Config", L"Manual", 0);
	if (Manual == 1)
	{
		method::PrintToConsole(L"[提示] 当前为手动视距修改，泉水中按下CTRL + HOME修改视距");
		return;
	}
	else
	{
		method::PrintToConsole(L"[提示] 当前为自动视距修改，识别到进入游戏时自动修改");
	}

	// 定位时间地址
	DWORD64 timeAddress = method::LocateSignature(client.hProcess, TIME_SIGNATURE_CODE, client.startAddress, client.endAddress, 4);
	if (timeAddress)
	{
		method::PrintToConsole(L"[信息] 特征码搜索到的时间地址 = 0x%llX", timeAddress);
		DWORD64 基址, 基址值 = 0;
		ReadProcessMemory(client.hProcess, LPCVOID(timeAddress), &基址值, 4, 0);
		//输出时间基址值
		method::PrintToConsole(L"[信息] 特征定位时间基址 = 0x%llX", 基址值);
		基址 = timeAddress + 基址值 + 4;//基址的计算结果
		method::PrintToConsole(L"[信息] 计算真正时间基址 = 0x%llX", 基址);
		float times = 0.0;
		ReadProcessMemory(client.hProcess, LPCVOID(基址), &times, 4, 0);
		//循环读取时间基址，直到时间不为0
		while (times <= 0)
		{
			ReadProcessMemory(client.hProcess, LPCVOID(基址), &times, 4, 0);
			// method::PrintToConsole(L"[信息] 循环读到的时间 = %f", times);
			std::this_thread::sleep_for(std::chrono::milliseconds(1314));
		}
		method::PrintToConsole(L"[信息] 当前游戏时间 = %f", times);
	}
	else
	{
		method::PrintToConsole(L"[警告] 特征码没读取到时间地址，可能特征码已过期，可使用CTRL + HOME尝试手动修改视距(视距特征码有效的情况下)");
		return;
	}
	// 获取RCX地址
	if (!GetRcxAddress())
	{
		// 失败则无法继续
		return;
	}
	// 修改视距
	WriteInsightData(distanceValue);
	return;
}

/**
 * @brief DLL入口函数
 * @param hModule DLL模块句柄
 * @param ul_reason_for_call 调用原因
 * @param lpReserved 保留参数
 * @return TRUE表示成功
 * @details 处理DLL的加载和卸载:
 *          - 加载时: 劫持dinput8.dll并启动初始化线程，会优先尝试获取系统目录，失败则使用默认路径
 *          - 卸载时: 释放所有资源
 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		// 设置控制台输出的字符编码为UTF-8
		setlocale(LC_ALL, ".utf8");
		//禁用DLL_THREAD_ATTACH和DLL_THREAD_DETACH通知，减小程序的工作集大小
		DisableThreadLibraryCalls(hModule);
		//从配置文件判断是否需要启动控制台 1 = 开启 0 = 关闭
		int OpenConsole = method::GetIntPrivateProfile(L"Config", L"OpenConsole", 0);
		if (OpenConsole == 1)
		{
			method::RedireceConsole();
		}
		// https://learn.microsoft.com/zh-cn/windows/win32/api/threadpoolapiset/nf-threadpoolapiset-createthreadpooltimer
		// 若要编译使用此函数的应用程序，请将_WIN32_WINNT定义为 0x0600 或更高版本
		g_timer = CreateThreadpoolTimer(TimerCallback, nullptr, nullptr);
		if (g_timer)
		{
			FILETIME ft{};
			// 相对时间，单位是100ns 如果为负数，则表示相对于当前时间等待的时间量
			ULONGLONG delay = (ULONGLONG)(-5210) * 10000;
			ft.dwHighDateTime = (DWORD)(delay >> 32);
			ft.dwLowDateTime = (DWORD)(delay & 0xFFFFFFFF);
			SetThreadpoolTimer(g_timer, &ft, 0, 0);
		}
		else
		{
			method::PrintToConsole(L"[错误] 创建线程池定时器失败 (错误代码: %d)", GetLastError());
		}
		// 启动初始化线程
		// CreateThread(NULL, NULL, Initialize, NULL, NULL, NULL);
		return true;
	}
	break;
	case DLL_PROCESS_DETACH:
	{
		method::PrintToConsole(L"[信息] DLL_PROCESS_DETACH -> DLL卸载，开始清理资源");
		// 清理资源
		method::exit();
	}
	break;
	}
	return TRUE;
}


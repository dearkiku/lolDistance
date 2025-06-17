#pragma once
#include <string>
// ����̨���
static HANDLE hConsole = nullptr;
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
};

namespace method {

	/**
	* @brief ��ӡ���ַ���������̨
	* @param format ��Lǰ׺�ĸ�ʽ���ַ���
	* @param ... �ɱ����
	* @note �Զ��������̨��������С
	*/
	void PrintToConsole(const wchar_t* format, ...);

	/**
	* @brief ��������ʼ������̨����
	* @details ���������ļ������Ƿ񴴽�����̨���ڣ��������������
	* @note ��Ҫ�ڳ����ʼ��ʱ����
	*/
	void RedireceConsole();

	/*
	* @brief ����ָ����DLL�ļ�
	* @details ��ȡ�����ļ��е�DLL·�������DLL��������أ��������������Ϣ
	*/
	void LoadDll();

	void exit();
	
	/*
	* @brief ��ȡģ���ַ��Χ
	* @param hModule ģ����
	* @return ����һ������ģ����ʼ��ַ�ͽ�����ַ��pair
	*/
	std::pair<DWORD64, DWORD64> GetModuleAddressRange(HMODULE hModule);
	
	/*
	* @brief �Ա������ֽ������Ƿ���ͬ
	* @param source ��Դ����
	* @param target Ŀ������
	* @param size �����С
	* @return �������������ͬ��������Ϊģ��ƥ��(0xFF)������TRUE�����򷵻�FALSE
	*/
	BOOL CompareArrays(const BYTE* a, const BYTE* b, size_t size);
	
	/*
	* @brief ���������ַ���ת��Ϊ�ֽ�����
	* @param featureCode �������ַ�������ʽΪ "XX XX ?? ??"
	* @param outBuffer ������ֽ����黺����
	* @param maxSize ���������������С
	* @return ת������ֽ����鳤��
	* @note ���Կո��??����??ת��Ϊ0xFF
	*/	
	size_t ConvertStringToByteArray(const std::string& featureCode, unsigned char* outBuffer, size_t maxSize);
	
	/*
	* @brief ��ָ�����ڴ淶Χ�ڲ���������
	* @param hProcess Ŀ����̾��
	* @param maskedPattern �������ַ�������ʽΪ "XX XX ?? ??"
	* @param startAddress ��ʼ��ַ
	* @param endAddress ������ַ
	* @param offset ƫ����
	* @return ����ҵ������룬�������������ڵ��ڴ��ַ+ƫ�ƣ����򷵻�0
	*/
	DWORD64 LocateSignature(HANDLE hProcess, const std::string& maskedPattern, DWORD64 startAddress, DWORD64 endAddress, int offset);
	
	/*
	* @brief �������ļ��ж�ȡ����ֵ
	* @param lpAppName ������
	* @param lpKeyName ������
	* @param nDefault Ĭ��ֵ��������������򷵻ش�ֵ
	* @return ���ض�ȡ������ֵ�������ȡʧ���򷵻�nDefault
	*/
	UINT GetIntPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault);

	/*
	* @brief �������ļ��ж�ȡ�ַ���
	* @param lpAppName ������
	* @param lpKeyName ������
	* @param lpDefault Ĭ��ֵ��������������򷵻ش�ֵ
	* @param lpReturnedString ���ڴ洢���ص��ַ���
	* @param nSize lpReturnedString�Ĵ�С
	* @return ����ʵ�ʶ�ȡ���ַ����������ȡʧ���򷵻�0
	*/
	DWORD GetStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPWSTR lpReturnedString, DWORD nSize);
	
	/*
	* @brief д���ַ����������ļ�
	* @param lpAppName ������
	* @param lpKeyName ������
	* @param lpString Ҫд����ַ���
	* @return ����ɹ��򷵻ط���ֵ�����򷵻���
	*/
	BOOL WriteStringPrivateProfile(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString);
}

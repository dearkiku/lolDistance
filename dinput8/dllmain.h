#define VERSION				3.1
// RAX��λ������
//constexpr const char RCX_SIGNATURE_CODE[] = "75 ?? 48 8B 0D ?? ?? ?? ?? 48 8B D6 48 8B 49 18 E8"; 2025-05-01 ʧЧ
constexpr const char RCX_SIGNATURE_CODE[] = "48 8B 05 ?? ?? ?? ??8B 0D ?? ?? ?? ?? BA";
// TIME ʱ�䶨λ������
constexpr const char TIME_SIGNATURE_CODE[] = "F3 0F 5C 35 ?? ?? ?? ?? 0F 28 F8";
// RAXĬ��ƫ��
constexpr DWORD64 RCX_OFFSET_DEFAULT = 0x18;
// ��ǰ�Ӿ�ƫ��
constexpr DWORD64 RCX_OFFSET_CURRENT = 0x314;
// �Ӿ�����ƫ��ָ��
constexpr DWORD64 RCX_OFFSET_POINTER = 0x300;
// ��С�Ӿ�ƫ��
constexpr DWORD64 RCX_OFFSET_MIN = 0x24;
// ����Ӿ�ƫ��
constexpr DWORD64 RCX_OFFSET_MAX = 0x28;
// �������ŵ����ݱ䶯
constexpr float ZOOM_VALUE = 120;
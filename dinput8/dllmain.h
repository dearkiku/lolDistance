#define VERSION				3.1
// RAX定位特征码
//constexpr const char RCX_SIGNATURE_CODE[] = "75 ?? 48 8B 0D ?? ?? ?? ?? 48 8B D6 48 8B 49 18 E8"; 2025-05-01 失效
constexpr const char RCX_SIGNATURE_CODE[] = "48 8B 05 ?? ?? ?? ??8B 0D ?? ?? ?? ?? BA";
// TIME 时间定位特征码
constexpr const char TIME_SIGNATURE_CODE[] = "F3 0F 5C 35 ?? ?? ?? ?? 0F 28 F8";
// RAX默认偏移
constexpr DWORD64 RCX_OFFSET_DEFAULT = 0x18;
// 当前视距偏移
constexpr DWORD64 RCX_OFFSET_CURRENT = 0x314;
// 视距限制偏移指针
constexpr DWORD64 RCX_OFFSET_POINTER = 0x300;
// 最小视距偏移
constexpr DWORD64 RCX_OFFSET_MIN = 0x24;
// 最大视距偏移
constexpr DWORD64 RCX_OFFSET_MAX = 0x28;
// 单次缩放的数据变动
constexpr float ZOOM_VALUE = 120;
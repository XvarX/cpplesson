#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  fundamentals/types_literals.hpp — 基本类型、字面量与类型工具                ║
// ║  声明: 类型工具常量和辅助函数                                                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <cstdint>      // int32_t, uint64_t 等定长整数
#include <cstddef>      // std::size_t, std::ptrdiff_t
#include <limits>       // std::numeric_limits

namespace fundamentals {

// ═══════════════════════════════════════════════════════════════════════════════
// 基本算术类型参考 — 每种类型的"是什么、为什么、什么时候用"
// ═══════════════════════════════════════════════════════════════════════════════
// C++ 的内置类型直接映射到硬件，没有额外的运行时开销。这是 C++ "零开销抽象"
// 哲学的基石。你写的 int 就是 CPU 寄存器里那个 int。
//
// 类型            大小(通常)    范围 (大致)              用途
// bool            1 byte       true / false             条件判断、标志位
// char            1 byte       -128 ~ 127 (或 0~255)    单个字符、字节操作
// short           2 bytes      -32768 ~ 32767           节省内存的场景
// int             4 bytes      -21亿 ~ 21亿             最常用的整数
// long            4 或 8 bytes 平台相关                 兼容旧代码
// long long       8 bytes      极大整数                  需要 64 位整数
// float           4 bytes      约 7 位有效数字          图形、传感器数据(精度要求不高)
// double          8 bytes      约 15 位有效数字         科学计算、高精度场景
// wchar_t         2 或 4 bytes Unicode 代码点           宽字符 (Windows API 常用)
// --------------------------------------------------------------------------------
// 为什么存在这么多整数类型？ 历史原因 + 不同场景需要不同大小的整数。
// 嵌入式系统可能想用 short 省内存，科学计算需要 long long 表示大数。

// ═══════════════════════════════════════════════════════════════════════════════
// 编译期类型大小查询 (演示用 constexpr 常量)
// ═══════════════════════════════════════════════════════════════════════════════
// 这些常量在编译期就确定了，可以用在模板参数和数组大小中
inline constexpr std::size_t kSizeBool      = sizeof(bool);
inline constexpr std::size_t kSizeChar      = sizeof(char);
inline constexpr std::size_t kSizeShort     = sizeof(short);
inline constexpr std::size_t kSizeInt       = sizeof(int);
inline constexpr std::size_t kSizeLong      = sizeof(long);
inline constexpr std::size_t kSizeLongLong  = sizeof(long long);
inline constexpr std::size_t kSizeFloat     = sizeof(float);
inline constexpr std::size_t kSizeDouble    = sizeof(double);

// ═══════════════════════════════════════════════════════════════════════════════
// numeric_limits 常用极值 (便捷常量)
// ═══════════════════════════════════════════════════════════════════════════════
inline constexpr int       kIntMax      = std::numeric_limits<int>::max();
inline constexpr int       kIntMin      = std::numeric_limits<int>::min();
inline constexpr long long kLLongMax    = std::numeric_limits<long long>::max();
inline constexpr double    kDoubleMax   = std::numeric_limits<double>::max();
inline constexpr double    kDoubleMin   = std::numeric_limits<double>::min();

} // namespace fundamentals

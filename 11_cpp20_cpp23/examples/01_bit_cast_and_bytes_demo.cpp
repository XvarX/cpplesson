// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: std::bit_cast、std::byteswap、位运算工具函数                          ║
// ║  目标: 掌握安全的位级类型转换、字节翻转、高效的位运算工具                    ║
// ║                                                                            ║
// ║  学习方式: bit_utils.hpp 中声明了辅助函数，实现在 src/bit_utils.cpp。        ║
// ║  下面的代码演示 bit_cast 的安全性和位运算工具的使用。                         ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "cpp20_23/bit_utils.hpp"
#include <print>
#include <bit>       // C++20 bit_cast, byteswap(C++23), popcount, bit_ceil 等
#include <cstdint>
#include <array>
#include <cmath>

int main() {
    lesson::print_header("std::bit_cast & 位运算工具");

    // ── Part 1: bit_cast ──
    lesson::print_subtitle("Part 1: std::bit_cast<T> — 安全的位级重解释");

    std::println("  === 检查浮点数内部表示 ===");
    inspect_float_bits(1.0f);
    inspect_float_bits(-3.14f);
    inspect_float_bits(0.15625f);  // 1/4 + 1/32 → 尾数存储方式

    // 比较: reinterpret_cast 的危险
    lesson::print_separator();
    std::println("  ⚠️  reinterpret_cast<float&>(int_val) = 未定义行为! (违反严格别名)");
    std::println("  ✅  std::bit_cast<float>(int_val)        = 定义良好的行为");

    lesson::print_separator();
    std::println("  === 快速倒数平方根 (Fast Inverse Sqrt) ===");
    std::println("  1.0f / sqrt(4.0f) = {:.6f}", 1.0f / std::sqrt(4.0f));
    std::println("  fast_rsqrt(4.0f)   = {:.6f}", fast_rsqrt(4.0f));

    // ── Part 2: byteswap + endian ──
    lesson::print_subtitle("Part 2: std::byteswap (C++23) — 字节翻转");

    // 检测字节序
    std::println("  本机字节序: {}",
        host_endian() == Endian::Little ? "小端 (Little Endian)" : "大端 (Big Endian)");

    uint32_t x_before = 0x12345678;
    uint32_t x_after  = std::byteswap(x_before);
    std::println("  byteswap(0x{:08X}) = 0x{:08X}", x_before, x_after);

    uint64_t y_before = 0x1122334455667788ULL;
    uint64_t y_after  = std::byteswap(y_before);
    std::println("  byteswap(0x{:016X}) = 0x{:016X}", y_before, y_after);

    // ── Part 3: 位运算工具 ──
    lesson::print_subtitle("Part 3: <bit> 位运算工具函数 (C++20)");

    // 测试一组数字
    std::array test_values = {0u, 1u, 7u, 8u, 13u, 16u, 255u, 1024u};

    std::println("  ┌────────┬──────────┬──────────┬─────────┬─────────┬──────────┬─────────┬──────────┐");
    std::println("  │  值    │ popcount │ clz (高位)│ ctz (低位)│ 2的幂?  │ bit_ceil │bit_floor│bit_width │");
    std::println("  ├────────┼──────────┼──────────┼─────────┼─────────┼──────────┼─────────┼──────────┤");
    for (auto v : test_values) {
        std::println("  │ {:>6} │ {:>8} │ {:>8} │ {:>7} │ {:>7} │ {:>8} │ {:>7} │ {:>8} │",
            v,
            std::popcount(v),
            std::countl_zero(v),
            std::countr_zero(v),
            std::has_single_bit(v) ? "是" : "否",
            std::bit_ceil(v),
            std::bit_floor(v),
            std::bit_width(v)
        );
    }
    std::println("  └────────┴──────────┴──────────┴─────────┴─────────┴──────────┴─────────┴──────────┘");

    // 实用场景
    lesson::print_separator();
    std::println("  实际场景:");
    std::println("    bit_ceil(100) = {}  ← 分配容量时对齐到 2 的幂", std::bit_ceil(100u));
    std::println("    bit_width(100) = {}  ← 需要多少位才能表示 100", std::bit_width(100u));
    std::println("    popcount(0xFF) = {}  ← 统计权限位 / 标志位", std::popcount(0xFFu));
    std::println("    has_single_bit(64) = {} ← 验证缓存行大小是否 2 的幂", std::has_single_bit(64u));

    // ==========================================================================
    // 常见陷阱
    // ==========================================================================
    lesson::print_separator("常见陷阱");
    std::println("  1. bit_cast 要求源和目标类型大小相同，否则编译错误");
    std::println("  2. bit_cast 不改变位模式 — 不是数学意义上的类型转换");
    std::println("  3. byteswap 只对整数类型有效 (C++23)");
    std::println("  4. bit_ceil(0) = 1 (未定义行为在 C++20, C++23 定义为 0 的特殊情况)");
    std::println("  5. popcount 对负数使用需要小心 (补码表示)");

    // ==========================================================================
    // 练习
    // ==========================================================================
    lesson::print_separator("练习");
    std::println("  1. 用 bit_cast 实现一个检查 double 是否是 subnormal 值的函数");
    std::println("  2. 写一个 network_to_host 和 host_to_network 函数组合 byteswap 和 endian");
    std::println("  3. 用 bit_ceil 写一个简单的内存池分配器 (对齐到 2 的幂)");
    std::println("  4. 实现一个 popcount 基准测试，对比 std::popcount 和手写循环的性能");
}

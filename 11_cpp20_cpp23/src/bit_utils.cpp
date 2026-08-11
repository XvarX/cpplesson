// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  bit_utils 实现: std::bit_cast 位级转换和位运算辅助函数                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "cpp20_23/bit_utils.hpp"
#include <print>
#include <bit>       // C++20 bit_cast, popcount, bit_ceil 等
#include <cmath>

// ============================================================================
// Part 1: std::bit_cast — 安全的位级重新解释
// ============================================================================

void inspect_float_bits(float value) {
    uint32_t bits = std::bit_cast<uint32_t>(value);  // 安全! 没有 UB
    std::println("  float {} → 0x{:08X}", value, bits);
    std::println("    符号位: {}",   (bits >> 31) & 1);
    std::println("    指数:   {}",   (bits >> 23) & 0xFF);
    std::println("    尾数:   0x{:06X}", bits & 0x7FFFFF);
}

float fast_rsqrt(float number) {
    // 不使用 reinterpret_cast 或 union hack (都是 UB!)
    uint32_t bits = std::bit_cast<uint32_t>(number);
    bits = 0x5f3759df - (bits >> 1);         // 神奇的常数
    float y = std::bit_cast<float>(bits);
    return y * (1.5f - 0.5f * number * y * y); // 一次牛顿迭代
}

Endian host_endian() {
    if constexpr (std::endian::native == std::endian::little) return Endian::Little;
    else return Endian::Big;
}

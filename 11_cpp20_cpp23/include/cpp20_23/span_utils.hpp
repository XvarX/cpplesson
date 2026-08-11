#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: std::span 和 std::source_location                                    ║
// ║  目标: 掌握 span 替代指针+长度的安全模式，source_location 替代宏的现代方案   ║
// ║                                                                            ║
// ║  std::span<T, Extent>: 轻量级数组视图 (C++20)                               ║
// ║  ─ 不拥有数据，只引用一段连续内存                                            ║
// ║  ─ 替代 (T* ptr, size_t len) 参数对，安全且表达意图清晰                     ║
// ║  ─ Extent = dynamic_extent (默认) → 运行时大小                               ║
// ║  ─ Extent = N                  → 编译时固定大小 (性能更好)                  ║
// ║                                                                            ║
// ║  std::source_location: 编译期源代码位置 (C++20)                              ║
// ║  ─ 替代 __FILE__ / __LINE__ / __FUNCTION__ 宏                               ║
// ║  ─ 类型安全、函数参数、默认值 (宏做不到)                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <span>
#include <source_location>
#include <string_view>

// ============================================================================
// Part 1: std::span — 动态 extent (默认)
// ============================================================================
// span 比指针+长度好的理由:
//   WHY:  指针+长度是两个独立参数，不一致时产生越界 bug
//         span 将它们绑定成一个对象，编译器可检查
//   WHEN: 任何接受连续数组引用的函数都应该考虑 span

// span 接受: C数组、std::array、std::vector、其他 span
void print_span(std::span<const int> data);

// ── 修改子区间: 使用 subspan ──
void zero_first_half(std::span<int> data);

// ============================================================================
// Part 2: std::span — 静态 extent (编译期固定大小)
// ============================================================================
// 当大小在编译期已知时，指定固定 extent 可让编译器做更多优化。
template<size_t N>
auto sum_fixed(std::span<const int, N> data) -> int {
    int total = 0;
    for (auto v : data) total += v;
    return total;
}

// ============================================================================
// Part 3: std::source_location — 替代 __FILE__/__LINE__ 宏
// ============================================================================
// source_location 的核心方法:
//   .file_name()    — 文件名
//   .line()         — 行号
//   .column()       — 列号 (C++20)
//   .function_name() — 函数签名

// 经典用法: 默认参数 — 调用者无需传参，自动捕获调用位置
void log_message(
    std::string_view msg,
    std::source_location loc = std::source_location::current()
);

// ── 断言宏的现代替代 ──
void my_assert(
    bool condition,
    std::string_view expr,
    std::source_location loc = std::source_location::current()
);

// ============================================================================
// Part 4: span 在实践中的模式
// ============================================================================

// 模式1: 接受任何连续容器，但不关心具体类型
double average(std::span<const double> values);

// 模式2: 接受 span + 返回 span — 纯视图操作，零拷贝
std::span<const int> tail(std::span<const int> data, size_t n);

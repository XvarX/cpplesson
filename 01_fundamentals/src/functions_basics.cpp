// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/functions_basics.cpp — 函数声明/定义、重载、默认参数 (实现)             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "fundamentals/functions_basics.hpp"
#include <print>

namespace fundamentals {

// ═══════════════════════════════════════════════════════════════════════════════
// print_info 重载 — 同名函数，不同参数类型，不同实现
// ═══════════════════════════════════════════════════════════════════════════════
// 编译器通过"重载决议 (overload resolution)"自动选择最匹配的版本。
// 规则: 参数类型决定调用哪个版本，返回值类型不参与决议。

void print_info(int value) {
    std::println("[int]    值 = {}", value);
}

void print_info(double value) {
    std::println("[double] 值 = {:.6f}", value);
}

void print_info(std::string_view msg) {
    std::println("[string] 消息 = {}", msg);
}

// ═══════════════════════════════════════════════════════════════════════════════
// [[nodiscard]] 函数 — 返回值不应被丢弃
// ═══════════════════════════════════════════════════════════════════════════════
// 编译器会在返回值被忽略时发出警告 (warning)，但不会阻止编译。
// 要真正忽略，可以显式转换: (void)compute_important_value();
// 或者 C++17: std::ignore = compute_important_value();

[[nodiscard]] int compute_important_value() {
    return 42;  // 假设这里有非常复杂的计算，忽略返回值意味着白算了
}

[[nodiscard]] bool validate_input(int x) {
    return x > 0;  // 如果调用者不检查这个返回值，验证就没有意义
}

} // namespace fundamentals

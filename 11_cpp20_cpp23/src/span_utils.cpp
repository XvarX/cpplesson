// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  span_utils 实现: std::span 和 std::source_location 的辅助函数              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "cpp20_23/span_utils.hpp"
#include <print>

// ============================================================================
// Part 1: std::span — 动态 extent (默认)
// ============================================================================

void print_span(std::span<const int> data) {
    std::print("  [");
    for (size_t i = 0; i < data.size(); ++i) {
        std::print("{}{}", data[i], i + 1 < data.size() ? ", " : "");
    }
    std::println("]  size={}", data.size());
}

void zero_first_half(std::span<int> data) {
    auto half = data.size() / 2;
    auto first_half = data.subspan(0, half);  // 子视图，不拷贝数据
    for (auto& v : first_half) v = 0;
}

// ============================================================================
// Part 3: std::source_location — 替代 __FILE__/__LINE__ 宏
// ============================================================================

void log_message(
    std::string_view msg,
    std::source_location loc
) {
    std::println("  [{}:{}] {}: {}",
        loc.file_name(),
        loc.line(),
        loc.function_name(),
        msg
    );
}

void my_assert(
    bool condition,
    std::string_view expr,
    std::source_location loc
) {
    if (!condition) {
        std::println("  ❌ ASSERT FAILED: '{}' at {}:{} ({})",
            expr, loc.file_name(), loc.line(), loc.function_name());
    }
}

// ============================================================================
// Part 4: span 在实践中的模式
// ============================================================================

double average(std::span<const double> values) {
    if (values.empty()) return 0.0;
    double sum = 0.0;
    for (auto v : values) sum += v;
    return sum / static_cast<double>(values.size());
}

std::span<const int> tail(std::span<const int> data, size_t n) {
    if (n >= data.size()) return data;
    return data.last(data.size() - n);  // C++20: last(N) 取尾部 N 个元素
}

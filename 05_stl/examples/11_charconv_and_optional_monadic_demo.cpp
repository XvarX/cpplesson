// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  示例: 11_charconv_and_optional_monadic_demo                                 ║
// ║  主题: to_chars/from_chars (高性能字符串转换)、optional 单子操作 (C++23)      ║
// ║  演示如何调用 stl_lib 中的教学函数来学习高性能转换与函数式可选类型操作        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/charconv_and_optional_monadic.hpp"

int main() {
    lesson::print_header("11_charconv_and_optional_monadic — 高性能转换与 optional 单子");

    // Part 1: to_chars — 快速数值转字符串
    // 演示整数各种进制、浮点数各种格式的高性能转换
    stl_learn::part1_to_chars();

    // Part 2: from_chars — 快速字符串转数值
    // 演示整数/浮点数解析、进制支持、错误处理
    stl_learn::part2_from_chars();

    // Part 3: optional 单子操作 (C++23)
    // 演示 and_then(链式调用)、or_else(回退/默认)、transform(纯映射)
    stl_learn::part3_optional_monadic();

    // Part 4: charconv + optional 综合实战
    // 将 from_chars 封装为 optional，构建安全解析流水线
    stl_learn::part4_combined();

    // 常见陷阱总结
    stl_learn::charconv_and_optional_monadic_pitfalls();

    // 练习建议
    stl_learn::charconv_and_optional_monadic_exercises();
}

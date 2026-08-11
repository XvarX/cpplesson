// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/control_flow.cpp — 控制流辅助函数 (实现)                                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "fundamentals/control_flow.hpp"
#include <print>

namespace fundamentals {

// ═══════════════════════════════════════════════════════════════════════════════
// classify_number — 演示 early return 风格
// ═══════════════════════════════════════════════════════════════════════════════
// 利用 return 提前退出，避免深层嵌套的 if-else。
// 这种"卫语句 (guard clause)"风格在 C++ 中被广泛推崇:
//   - 先处理边界条件/特殊情况并 return
//   - 最后处理正常流程
// 优点: 减少缩进层次、逻辑更清晰、更易维护。
[[nodiscard]] std::string classify_number(int n) {
    if (n > 0)      return "正数";
    if (n < 0)      return "负数";
    return "零";    // 隐式的 else — 前面的 return 已经排除了 >0 和 <0
}

// ═══════════════════════════════════════════════════════════════════════════════
// day_name — 演示 switch 基础
// ═══════════════════════════════════════════════════════════════════════════════
// switch 在底层可被编译器优化为跳转表 (jump table):
//   - 连续的 case 值 → 编译器生成地址表，按索引 O(1) 跳转
//   - 稀疏的 case 值 → 编译器退化为 if-else 链或二分查找
// default 分支: 处理所有未列出的 case 值，良好的防御性编程实践。
[[nodiscard]] std::string_view day_name(int day) {
    switch (day) {
        case 1:  return "星期一";
        case 2:  return "星期二";
        case 3:  return "星期三";
        case 4:  return "星期四";
        case 5:  return "星期五";
        case 6:  return "星期六";
        case 7:  return "星期日";
        default: return "无效的日子";
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// describe_type — 演示 [[fallthrough]] (C++17)
// ═══════════════════════════════════════════════════════════════════════════════
// [[fallthrough]] 必须满足:
//   1. 放在它要穿透到的 case 之前
//   2. 它所在的 case 不能为空 (至少有一行代码或 [[fallthrough]] 本身)
//   3. 后面必须紧跟另一个 case 或 default 标签
// 注意: 第一个 case 群 (元音字母) 使用了多个 case 标签共用一个代码块，
//       这种方式不需要 [[fallthrough]]，因为 case 之间没有代码。
void describe_type(char c) {
    switch (c) {
        case 'a': case 'e': case 'i': case 'o': case 'u':
        case 'A': case 'E': case 'I': case 'O': case 'U':
            std::println("'{}' 是元音字母", c);
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            // [[fallthrough]]: 显式标记"我知道这里没有 break，我是故意的"
            // 数字字符不是元音，但我们也想用 default 的信息打印
            [[fallthrough]];
        default:
            std::println("'{}' 不是元音 (数字或其他字符)", c);
            break;
    }
}

} // namespace fundamentals

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  01_fundamentals 示例 — if/else、switch/[[fallthrough]]、循环、break/continue/ ║
// ║                         goto                                                 ║
// ║  目标: 掌握 C++ 的所有控制流结构，理解每种结构的适用场景和性能特征            ║
// ║  前置: 01_types_and_literals, 05_function_basics                              ║
// ║  对应库头文件: fundamentals/control_flow.hpp                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <string_view>
#include <vector>
#include <cstdlib>      // std::atoi
#include "shared/lesson_utils.hpp"
#include "fundamentals/control_flow.hpp"

int main() {
    lesson::print_header("06 控制流: if/else, switch, 循环, break/continue, goto");

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 1: if / else if / else — 条件分支
    // ═══════════════════════════════════════════════════════════════════════════════
    // if/else 是什么: 最基本的条件分支结构，根据布尔表达式选择执行路径。
    // 为什么是基础: 每个有用的程序都需要决策能力。
    // 性能提示:
    //   - 编译器擅长优化 if-else 链，不需要过早优化成 switch
    //   - 如果分支有大概率偏好，可以用 [[likely]] / [[unlikely]] (C++20) 提示分支预测
    //   - 过多的 else if 会降低可读性 → 考虑 switch、跳表、map
    lesson::print_subtitle("Part 1: if / else if / else");

    int score = 85;

    // ── 基本 if-else ──
    if (score >= 90) {
        std::println("成绩: {} → 优秀", score);
    } else if (score >= 80) {
        std::println("成绩: {} → 良好", score);           // ← 85 走这里
    } else if (score >= 60) {
        std::println("成绩: {} → 及格", score);
    } else {
        std::println("成绩: {} → 需要进步", score);
    }

    // ── 单行 if 可以省略大括号，但不推荐 (容易出错) ──
    if (score > 0) std::println("score 是正数");  // 可以但不推荐

    // ── 三元运算符: condition ? true_expr : false_expr ──
    // 是表达式而非语句，可以用在初始化列表中
    std::string grade = (score >= 90) ? "A" : (score >= 80) ? "B" : (score >= 60) ? "C" : "D";
    std::println("等级: {}", grade);

    // ── if 初始化语句 (C++17): if (init; condition) ──
    // 限制变量的作用域到 if 块内
    // 使用库中的 classify_number 函数 (定义在 control_flow.cpp)
    if (auto result = fundamentals::classify_number(score); result == "正数") {
        std::println("if-init: score 是正数 ({})", result);
    }

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 2: switch — 多路分支 (整数/枚举)
    // ═══════════════════════════════════════════════════════════════════════════════
    // switch 是什么: 基于整数值 (int, char, enum) 的多路分支。
    // 为什么存在: 编译器可优化为跳转表 (O(1) 而非 O(n) 的 if-else 链)。
    // 什么时候用 switch:
    //   - 判断目标是整数/枚举
    //   - case 数量较多 (3+)
    //   - 每个 case 的处理逻辑较短
    // 什么时候用 if-else:
    //   - 判断条件是复杂表达式 (如 x > 10 && y < 20)
    //   - 判断目标是字符串、浮点数 (switch 不支持)
    // 关键规则:
    //   - case 值必须是编译期常量
    //   - 每个 case 要以 break 结束 (否则会"穿透"到下一个 case)
    lesson::print_subtitle("Part 2: switch");

    std::println("day_name(1): {}", fundamentals::day_name(1));
    std::println("day_name(5): {}", fundamentals::day_name(5));
    std::println("day_name(8): {}", fundamentals::day_name(8));

    // ── switch 初始化语句 (C++17) ──
    switch (int day = 3; day) {  // day 的作用域仅限于这个 switch
        case 1: std::println("switch-init: 周一"); break;
        case 2: std::println("switch-init: 周二"); break;
        case 3: std::println("switch-init: 周三"); break;
        default: std::println("switch-init: 其他");
    }
    // day 在这里不可见

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 3: [[fallthrough]] — 有意穿透 (C++17)
    // ═══════════════════════════════════════════════════════════════════════════════
    // [[fallthrough]] 是什么: 属性标记，告诉编译器和读者"此处没有 break 是有意的"。
    // 为什么存在: 编译器对无 break 的 case 会发出警告。如果你确实想穿透，
    //             用 [[fallthrough]] 告诉编译器"别警告我，我故意的"。
    // 什么时候用: 多个 case 共享部分逻辑，但仍需在穿透前执行一些特定操作。
    // 注意: [[fallthrough]] 必须放在它要穿透到的 case 之前，且它所在的 case 不能为空。
    lesson::print_subtitle("Part 3: [[fallthrough]]");

    // 使用库中的 describe_type 函数 (定义在 control_flow.cpp)
    fundamentals::describe_type('a');   // 元音
    fundamentals::describe_type('b');   // 不是元音
    fundamentals::describe_type('5');   // 数字, 穿透到 default

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 4: for 循环 — 最常用的循环
    // ═══════════════════════════════════════════════════════════════════════════════
    // for 循环有三种形式:
    //   1) 传统 for:  for (init; condition; increment)
    //   2) range-for: for (auto x : container)      (C++11)
    //   3) range-for 带初始化: for (init; auto x : container) (C++20)
    lesson::print_subtitle("Part 4: for 循环");

    // ── 传统 for ──
    std::print("传统 for (1~5): ");
    for (int i = 1; i <= 5; ++i) {
        std::print("{} ", i);
    }
    std::println("");

    // ── Range-based for (C++11) ──
    // 遍历容器时不需要手动管理索引或迭代器，更安全、更简洁
    std::vector<int> nums = {10, 20, 30, 40, 50};
    std::print("range-for: ");
    for (int n : nums) {
        std::print("{} ", n);
    }
    std::println("");

    // ── Range-for 带初始化语句 (C++20) ──
    std::print("range-for with init: ");
    for (auto vec = std::vector{1, 2, 3}; int v : vec) {
        std::print("{} ", v);
    }
    std::println("");

    // ── 用引用修改元素 ──
    for (int& n : nums) { n *= 2; }
    std::println("翻倍后: {}", nums);  // {20, 40, 60, 80, 100}

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 5: while 和 do-while — 条件驱动循环
    // ═══════════════════════════════════════════════════════════════════════════════
    // while: 先判断条件，再执行循环体 (可能一次都不执行)
    // do-while: 先执行一次循环体，再判断条件 (至少执行一次)
    // 什么时候用 while？ 循环次数未知，取决于运行时条件
    // 什么时候用 for？   循环次数已知/由范围决定
    lesson::print_subtitle("Part 5: while 和 do-while");

    // ── while: 先判断 ──
    std::print("while 倒计时: ");
    int countdown = 5;
    while (countdown > 0) {
        std::print("{} ", countdown);
        --countdown;
    }
    std::println("发射!");

    // ── do-while: 先执行 ──
    std::print("do-while (至少执行一次): ");
    int x = 0;
    do {
        std::print("{} ", x);
        ++x;
    } while (false);  // 条件为 false, 但循环体已经执行了一次
    std::println("← 即使条件为 false 也执行了一次");

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 6: break 和 continue — 循环控制
    // ═══════════════════════════════════════════════════════════════════════════════
    // break:    立即退出当前循环 (或 switch)
    // continue: 跳过本次循环的剩余部分，进入下一次迭代
    // 多层循环时，break 只退出最内层 — 这是常见易错点
    lesson::print_subtitle("Part 6: break 和 continue");

    // ── break: 发现目标后提前退出 ──
    std::print("寻找第一个 > 50 的数: ");
    for (int n : nums) {
        if (n > 50) {
            std::println("找到 {}!", n);   // nums = {20, 40, 60...} → 60
            break;  // 立即退出循环
        }
        std::print("检查 {}, ", n);
    }

    // ── continue: 跳过偶数 ──
    std::print("只打印奇数: ");
    for (int i = 1; i <= 10; ++i) {
        if (i % 2 == 0) continue;  // 跳过偶数
        std::print("{} ", i);
    }
    std::println("");

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 7: goto — 无条件跳转 (谨慎使用)
    // ═══════════════════════════════════════════════════════════════════════════════
    // goto 是什么: 直接跳转到指定标签处。几乎被现代 C++ 抛弃。
    // 为什么还存在: 1) C 语言遗留  2) 跳出深层嵌套循环
    //               3) 错误处理中的统一清理 (C 风格, C++ 用 RAII)
    // 什么时候用: 几乎不用。深层嵌套的跳出场景可以考虑，但通常重构更好。
    // 为什么避免: 使控制流难以理解 (spaghetti code)、破坏 RAII (跳过析构函数)
    lesson::print_subtitle("Part 7: goto");

    int value = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (i * j > 6) {
                value = i * j;
                goto found;  // 跳出所有嵌套循环 (唯一合理的 goto 用法)
            }
        }
    }
    // 如果没有 goto，你需要一个额外的 bool flag 来逐层 break
found:
    std::println("goto: 第一个 i*j > 6 的结果是 {}", value);

    // ═══════════════════════════════════════════════════════════════════════════════
    // 综合演示: 把所有控制流拼起来的经典 FizzBuzz
    // ═══════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("综合: FizzBuzz");

    std::println("FizzBuzz (1~15):");
    for (int i = 1; i <= 15; ++i) {
        if (i % 15 == 0) {
            std::print("FizzBuzz ");
        } else if (i % 3 == 0) {
            std::print("Fizz ");
        } else if (i % 5 == 0) {
            std::print("Buzz ");
        } else {
            std::print("{} ", i);
        }
    }
    std::println("");

    // ── 常见陷阱 (Common Pitfalls) ────────────────────────────────────────────
    lesson::print_separator("常见陷阱");
    lesson::print_warn("陷阱1: switch 忘记 break — 默认会穿透到下一个 case! 用 [[fallthrough]] 显式标记");
    lesson::print_warn("陷阱2: if 条件中写 = 而非 == — if (x = 5) 是赋值, 总是 true");
    lesson::print_warn("陷阱3: for 循环中修改容器 (增/删元素) — 迭代器失效, 用索引或先收集后操作");
    lesson::print_warn("陷阱4: while(条件) 后多写分号 — while(x > 0); 是死循环 (空语句体)");
    lesson::print_warn("陷阱5: do-while 末尾分号 — do { ... } while(cond); ← 这个分号必须有!");
    lesson::print_warn("陷阱6: goto 跳过变量初始化 — 跳过的局部变量未初始化");
    lesson::print_warn("陷阱7: break 只跳出一层 — 嵌套循环中容易误以为退出全部循环");
    lesson::print_warn("陷阱8: range-for 中使用 auto (拷贝) 而非 auto& (引用) — 修改无效且浪费拷贝");

    // ── 练习 ─────────────────────────────────────────────────────────────────
    lesson::print_separator("练习");
    std::println("1. 用 switch + [[fallthrough]] 实现: 输入月份, 输出该月天数 (2 月默认 28)");
    std::println("2. 打印 9×9 乘法表 (双层 for)");
    std::println("3. 用 while 实现计算数字各位之和 (例如 1234 → 1+2+3+4=10)");
    std::println("4. 写一个带标志 break 的二分查找 (找到后 break)");
    std::println("5. 实验: 写一个故意忘记 break 的 switch, 观察穿透行为");
    std::println("6. 把 goto 版本换成 bool flag 版本, 体会两者的可读性差异");

    return 0;
}

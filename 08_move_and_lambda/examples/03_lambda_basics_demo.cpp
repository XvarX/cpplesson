// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 08_move_and_lambda                                                     ║
// ║  主题: Lambda 语法、捕获列表、mutable、返回类型推导、IILE                     ║
// ║  学习目标:                                                                   ║
// ║    1. 掌握 Lambda 表达式的完整语法结构                                        ║
// ║    2. 理解各种捕获方式(= & this *this)及其使用场景                           ║
// ║    3. 学会使用 mutable 让 Lambda 修改按值捕获的变量                           ║
// ║    4. 理解返回类型推导规则, 以及何时需要显式指定 -> ReturnType                ║
// ║    5. 掌握 IILE(立即调用的 Lambda 表达式)的实用技巧                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "move_lambda/lambda_basics.hpp"
#include <print>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

using namespace move_lambda;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: Lambda 语法结构 — 从头到尾拆解
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  Lambda 是一个匿名函数对象(闭包), 语法:
//        [捕获列表](参数列表) -> 返回类型 { 函数体 }
//        └──┬──┘ └──┬──┘   └──┬──┘   └─┬─┘
//       捕获外部变量  形参   可选尾置返回  代码
// WHY:   比手写函数对象简洁得多, 是 C++ 函数式编程的核心
// WHEN:   需要短小的回调、谓词、转换函数时, 就地定义

void part1_lambda_syntax() {
    lesson::print_subtitle("Part 1: Lambda 语法拆解");

    // 最简 Lambda: 无捕获, 无参数, 返回自动推导
    auto hello = [] { std::println("Hello from Lambda!"); };
    hello();

    // 参数 + 返回类型自动推导
    auto add = [](int a, int b) { return a + b; };  // 返回 int
    std::println("add(3, 5) = {}", add(3, 5));

    // 显式指定返回类型(当有多个 return 且类型不一致时需要)
    auto classify = [](int x) -> std::string {
        if (x > 0)  return "正数";
        if (x < 0)  return "负数";
        return "零";
    };
    std::println("classify(42) = {}", classify(42));
    std::println("classify(-7) = {}", classify(-7));
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 捕获列表 — = & this *this 详解
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  捕获列表决定了 Lambda 如何访问外部变量:
//        [=]      按值捕获所有(可读不可写)
//        [&]      按引用捕获所有(可读写, 注意悬垂)
//        [this]   捕获 this 指针(访问成员, C++20 前隐式)
//        [*this]  按值捕获 *this(C++17), 复制整个对象
//        [x, &y]  混合捕获: x 按值, y 按引用
// WHY:   控制 Lambda 的依赖关系和生命周期
// WHEN:   短生命周期可用 [&], 跨线程/延迟调用用 [=] 或明确捕获

void part2_capture_list() {
    lesson::print_subtitle("Part 2: 捕获列表");

    int a = 10, b = 20, c = 30;

    // [=] 按值捕获: Lambda 内部有 a,b,c 的副本
    auto byValue = [=] {
        // a = 100;  // ❌ 错误! 按值捕获不可修改
        return a + b + c;
    };
    a = 999;  // 修改外部变量不影响 Lambda 内的副本
    std::println("[=] 按值捕获: 结果是 {} (不受外部修改影响)", byValue());

    // [&] 按引用捕获: Lambda 持有 a,b,c 的引用
    auto byRef = [&] {
        a = 100;   // ✅ 可以修改(通过引用)
        return a + b + c;
    };
    std::println("[&] 按引用捕获: 结果 = {}, a 被改为 {}", byRef(), a);

    // 混合捕获: x 按值, y 按引用
    int x = 1, y = 2;
    auto mixed = [x, &y] {
        // x 不可改(按值复制), y 可改(按引用)
        y = 99;
        return x + y;
    };
    std::println("混合捕获: 结果 = {}, y 变为 {}", mixed(), y);

    // [this] 在类中使用 (见下文 part2b)
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2b: [this] 和 [*this] — 类成员捕获
// ═══════════════════════════════════════════════════════════════════════════════
void part2b_this_capture() {
    lesson::print_subtitle("Part 2b: [this] vs [*this] 捕获");

    Counter c;

    auto inc_ref = c.getIncrementer_byRef();
    inc_ref();  // count_ 从 0→1
    inc_ref();  // count_ 从 1→2
    // 修改的是原始 Counter 对象

    auto inc_copy = c.getIncrementer_byCopy();
    inc_copy(); // 修改的是副本, 不影响原对象
    inc_copy();
    // inc_ref 再调用, 还是操作原对象的 count_
    inc_ref();  // count_ 从 2→3

    std::println("总结: [this] 共享原对象, [*this] 独立副本");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: mutable Lambda — 让按值捕获可修改
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  默认情况下, Lambda 的 operator() 是 const 的 → 按值捕获的变量不可改
//        加上 mutable 关键字后, operator() 不再是 const → 可以修改内部副本
// WHY:   需要 Lambda 维护内部状态时(计数器, 缓存等)
// WHEN:   需要一个有状态的函数对象, 且状态不需要暴露到外部

void part3_mutable() {
    lesson::print_subtitle("Part 3: mutable Lambda");

    // 没有 mutable: 编译错误或无法修改
    // auto noMut = [n = 0] { return ++n; };  // ❌ n 不可改

    // 加上 mutable: ✅ 可以修改按值捕获的副本
    auto counter = [n = 0]() mutable {
        return ++n;
    };
    std::println("counter(): {}", counter());  // 1
    std::println("counter(): {}", counter());  // 2
    std::println("counter(): {}", counter());  // 3

    // 每次调用都修改的是同一个内部副本
    // 注意: 多个 Lambda 实例各有自己的状态

    // mutable 的典型用途: std::generate
    auto seq = [n = 0]() mutable { return n += 2; };
    std::vector<int> evens(5);
    std::ranges::generate(evens, seq);
    std::println("mutable 生成器: 偶数序列");
    for (auto v : evens) std::print("{} ", v);
    std::println("");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: 返回类型推导
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  C++14 起, Lambda 可以自动推导返回类型(含 auto 参数)
//        但多个 return 必须返回相同类型, 否则需要显式 -> ReturnType
// WHY:   减少样板代码, 但复杂场景需显式标注以帮助理解
// WHEN:   简单 Lambda 省略返回类型; 多种返回类型时显式写 -> ReturnType

void part4_return_type_deduction() {
    lesson::print_subtitle("Part 4: 返回类型推导");

    // ✅ 自动推导: 所有 return 都返回 int
    auto ok = [](int x) {
        if (x > 0) return 1;
        return 0;
    };
    std::println("ok(5) = {}", ok(5));

    // ❌ 编译错误(如果去掉 -> double): 类型不一致
    // auto bad = [](int x) {
    //     if (x > 0) return 1;     // int
    //     return 3.14;             // double — 冲突!
    // };

    // ✅ 显式指定: 所有 return 隐式转换到 double
    auto good = [](int x) -> double {
        if (x > 0) return 1;        // int→double
        return 3.14;                 // double
    };
    std::println("good(5) = {}, good(-1) = {}", good(5), good(-1));
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: IILE — 立即调用的 Lambda 表达式
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  Lambda 定义后立即调用: []{ ... }();
// WHY:   - 将复杂初始化逻辑封装在一个表达式内
//        - 让 const 变量可以用多步计算初始化(替代立即调用的函数)
//        - 限制临时变量的作用域
// WHEN:   需要复杂的初始化逻辑、但变量应该是 const 时

void part5_iile() {
    lesson::print_subtitle("Part 5: IILE — 立即调用的 Lambda");

    // 传统写法: 需要复杂初始化, 变量不能是 const
    // int result;
    // if (some_condition) { 多步计算... result = ... }
    // else { 多步计算... result = ... }

    // ✅ IILE 写法: const + 复杂初始化一步到位
    const int fibonacci_10th = [] {
        int a = 0, b = 1;
        for (int i = 0; i < 10; ++i) {
            int next = a + b;
            a = b;
            b = next;
        }
        return a;
    }();  // ← 注意这个 () 表示立即调用
    std::println("第 10 个斐波那契数 = {}", fibonacci_10th);

    // IILE 在实际项目中的典型用例: 复杂的 const 容器
    const std::vector<int> primes_under_30 = [] {
        std::vector<int> result;
        for (int n = 2; n < 30; ++n) {
            bool is_prime = true;
            for (int d = 2; d * d <= n; ++d) {
                if (n % d == 0) { is_prime = false; break; }
            }
            if (is_prime) result.push_back(n);
        }
        return result;
    }();
    std::print("小于 30 的质数: ");
    for (auto p : primes_under_30) std::print("{} ", p);
    std::println("");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
void pitfalls() {
    lesson::print_header("常见陷阱");

    std::println("陷阱1: 按引用捕获的悬垂引用(dangling reference)");
    std::println("  auto f = [&] {{ return x; }};  // x 的生命周期比 f 短 → 悬垂!");
    std::println("  → 用 [=] 或明确按值捕获");

    std::println("陷阱2: Lambda 的拷贝语义");
    std::println("  auto f2 = f1; // 按值捕获的变量也被拷贝, 各有一份独立状态");
    std::println("  → 这可能导致意外的独立性");

    std::println("陷阱3: 在 for 循环中按引用捕获循环变量");
    std::println("  for (int i=0; i<5; ++i) vec.push_back([&i]{{ return i; }});");
    std::println("  → 所有 Lambda 引用同一个 i, 最终都返回 5!");

    std::println("陷阱4: [=] 捕获的 const 默认性");
    std::println("  → 按值捕获的变量默认不可修改, 需要 mutable");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
void exercises() {
    lesson::print_header("练习");
    std::println("1. 写一个 Lambda, 用 IILE 计算并返回 1+2+...+100 的结果");
    std::println("2. 写一个 Counter 类, 它的 getCounter() 方法返回一个 mutable Lambda");
    std::println("3. 解释: 以下代码输出什么? 为什么?");
    std::println("   int x = 5; auto f = [x]() mutable {{ return ++x; }};");
    std::println("   auto g = f; std::println(\"{{}} {{}}\", f(), g());");
    std::println("4. 用 std::ranges::sort + Lambda 对一个 vector<int> 降序排序");
    std::println("5. 修复这个有悬垂引用的 Lambda:");
    std::println("   std::function<int()> createDangling() {{ int x=42; return [&]{{ return x; }}; }}");
}

int main() {
    lesson::print_header("03: Lambda 基础 — 语法、捕获与立即调用");

    part1_lambda_syntax();
    part2_capture_list();
    part2b_this_capture();
    part3_mutable();
    part4_return_type_deduction();
    part5_iile();
    pitfalls();
    exercises();

    return 0;
}

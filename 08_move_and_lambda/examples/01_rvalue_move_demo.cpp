// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 08_move_and_lambda                                                     ║
// ║  主题: 左值/右值/将亡值、std::move、移动构造/赋值、noexcept 移动、RVO/NRVO   ║
// ║  学习目标:                                                                   ║
// ║    1. 理解左值(lvalue)、右值(rvalue)、将亡值(xvalue)的含义与区别              ║
// ║    2. 掌握 std::move 的本质——它只是一个类型转换                               ║
// ║    3. 学会编写移动构造函数和移动赋值运算符                                    ║
// ║    4. 理解 noexcept 对移动性能的影响(如 vector 扩容策略)                      ║
// ║    5. 认识 RVO/NRVO 拷贝省略——编译器比你想象的更聪明                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "move_lambda/rvalue_move.hpp"
#include <print>
#include <string>
#include <vector>
#include <chrono>
#include <utility>   // std::move

using namespace move_lambda;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: 左值(lvalue) 与 右值(rvalue) 的基本概念
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  左值 = 有名字、有地址、可以取 & 的对象
//        右值 = 临时的、没有名字、即将消亡的对象
// WHY:   编译器用这个区分"能安全复用的资源"和"即将消失的资源"
// WHEN:  理解右值是理解移动语义的前提

void part1_lvalue_rvalue() {
    lesson::print_subtitle("Part 1: 左值 vs 右值");

    int x = 42;              // x 是左值: 有名字, 有地址
    // int* p = &42;         // ❌ 错误! 42 是右值, 没有地址
    int* p = &x;             // ✅ x 是左值, 可以取地址

    // 区分: = 左边都是左值, = 右边可能是左值也可能是右值
    int y = x;               // x 作为右值使用(读取它的值)
    int z = x + y;           // x + y 的结果是临时值(右值)

    // 左值引用 vs 右值引用
    int& lref = x;           // 左值引用: 绑定到左值
    // int& bad = 42;        // ❌ 左值引用不能绑定到右值
    const int& cref = 42;    // ✅ const 左值引用可以绑定到右值(生命周期延长)
    int&& rref = 42;         // ✅ 右值引用: 绑定到右值

    std::println("x 是左值, 地址 = {}", static_cast<void*>(p));
    std::println("lref 引用 x, 值 = {}", lref);
    std::println("rref 绑定右值 42, 值 = {}", rref);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 将亡值(xvalue) 与 std::move 的本质
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  将亡值 = 资源即将被转移的对象(std::move 的返回值就是一个将亡值)
//        std::move 只是一个 static_cast<T&&>, 不做任何实际移动操作!
// WHY:   通知编译器"这个对象我以后不用了, 你可以把它的资源拿走"
// WHEN:  需要将资源从一个对象转移到另一个对象时

void part2_xvalue_and_move() {
    lesson::print_subtitle("Part 2: 将亡值(xvalue) 与 std::move");

    std::string s1 = "Hello, World!";
    // 下面的 std::move 只是把 s1 转成 string&&, 并没有真正"移动"任何东西
    std::string s2 = std::move(s1);  // 这里才真正发生移动(调用移动构造函数)

    std::println("s1 被移动后: \"{}\" (长度={})", s1, s1.size());
    std::println("s2 获得资源: \"{}\" (长度={})", s2, s2.size());
    // 预期输出: s1=""(空或未指定), s2="Hello, World!"

    // 关键教训: std::move 只是类型转换, 不要对移动后的对象抱有期望
    // s1 处于"有效但未指定"状态, 可以安全销毁或重新赋值
    s1 = "New content";  // ✅ 可以重新赋值
    std::println("s1 重新赋值后: \"{}\"", s1);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: 自定义移动构造和移动赋值
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  移动构造: T(T&& other)        — 从 other 那里"偷走"资源
//        移动赋值: T& operator=(T&&)   — 释放自己的旧资源, 偷走 other 的
// WHY:   避免深拷贝, 对于管理堆内存的类性能提升巨大(O(n)→O(1))
// WHEN:  类管理了堆内存/文件句柄/锁等不可共享的资源时, 必须实现移动语义

void part3_move_constructor_assignment() {
    lesson::print_subtitle("Part 3: 自定义移动构造和移动赋值");

    Buffer b1(1000);           // 分配 1000 个 int
    Buffer b2 = std::move(b1); // 调用移动构造, 没有深拷贝!

    std::println("b1.empty() = {}, b2.size() = {}",
                 b1.empty(), b2.size());
    // 预期: b1.empty() = true, b2.size() = 1000

    Buffer b3(500);
    b3 = std::move(b2);        // 调用移动赋值: 释放 b3 旧资源, 偷走 b2 的
    std::println("b2.empty() = {}, b3.size() = {}",
                 b2.empty(), b3.size());
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: noexcept 移动 — 为什么它很重要
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  移动操作标记为 noexcept 是一种承诺:"移动不会抛异常"
// WHY:   std::vector 扩容时, 如果移动是 noexcept 的, 就用移动; 否则用拷贝!
//        这直接影响容器性能——尤其是 vector 的 push_back 扩容
// WHEN:   所有移动构造/赋值都应该标记 noexcept(除非确实可能抛异常)

void part4_noexcept_importance() {
    lesson::print_subtitle("Part 4: noexcept 移动的重要性");

    // 检查移动构造是否为 noexcept
    std::println("NoNoexceptMove 移动构造 noexcept?  {}",
                 std::is_nothrow_move_constructible_v<NoNoexceptMove>);
    std::println("WithNoexceptMove 移动构造 noexcept? {}",
                 std::is_nothrow_move_constructible_v<WithNoexceptMove>);
    std::println("std::string 移动构造 noexcept?       {}",
                 std::is_nothrow_move_constructible_v<std::string>);

    // vector 扩容时的行为差异:
    // - noexcept 移动 → 用移动, 快
    // - 非 noexcept 移动 → 用拷贝, 慢(保证异常安全)
    std::println("结论: 移动操作一律标记 noexcept!");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: RVO/NRVO — 拷贝(移动)省略
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  RVO(返回值优化) / NRVO(具名返回值优化):
//        编译器直接在调用方的内存位置构造返回值, 跳过拷贝/移动
// WHY:   减少不必要的对象构造, 提升性能
// WHEN:  从 C++17 开始, RVO 在某些场景是强制性的! 不要画蛇添足写 std::move

void part5_rvo_nrvo() {
    lesson::print_subtitle("Part 5: RVO/NRVO 拷贝省略");

    std::println("create_good() — 预期只有 1 次构造(NRVO):");
    Tracker a = create_good();

    std::println("create_bad() — 预期 1 次构造 + 1 次移动(std::move 阻止了 NRVO):");
    Tracker b = create_bad();

    std::println("结论: return 局部变量时, 不要写 std::move!");
    std::println("编译器比你能做更好的优化。");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
void pitfalls() {
    lesson::print_header("常见陷阱");

    // 陷阱 1: 对 const 对象使用 std::move — const T&& 无法绑定到 T&&
    std::println("陷阱1: const 对象 std::move 仍是 const&&, 移动退化为拷贝");
    const std::string cs = "const string";
    std::string copied = std::move(cs);  // cs 的类型是 const string&&, 匹配拷贝构造!
    std::println("  cs 仍在: \"{}\"", cs);  // cs 未被移动!

    // 陷阱 2: 移动后继续使用对象(未定义行为不保证, 但结果不可预测)
    std::println("\n陷阱2: 移动后使用对象");
    std::string s = "important data";
    std::string stolen = std::move(s);
    // s 处于"有效但未指定"状态, 以下操作可能意外:
    // 可以: s.clear(), s = "new", s.empty()
    // 危险: 假设 s 还保留原值
    std::println("  原字符串被移动后: \"{}\"", s);  // 很可能是空串

    // 陷阱 3: return std::move(local) — 阻止 NRVO
    std::println("\n陷阱3: return std::move(局部变量) 阻止 NRVO, 画蛇添足!");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
void exercises() {
    lesson::print_header("练习");
    std::println("1. 为 Buffer 类添加拷贝赋值运算符, 确保深拷贝语义");
    std::println("2. 写一个简单的 UniquePtr<T> 类, 只支持移动, 不支持拷贝");
    std::println("3. 验证: 对只管理指针的类, 移动比拷贝快多少? (用 lesson::time_it)");
    std::println("4. 思考: std::unique_ptr 为什么不能拷贝但可以移动?");
    std::println("5. 实验: 在函数中 return std::move(局部变量), 观察构造函数调用次数");
}

int main() {
    lesson::print_header("01: 左值/右值、移动语义与拷贝省略");

    part1_lvalue_rvalue();
    part2_xvalue_and_move();
    part3_move_constructor_assignment();
    part4_noexcept_importance();
    part5_rvo_nrvo();
    pitfalls();
    exercises();

    return 0;
}

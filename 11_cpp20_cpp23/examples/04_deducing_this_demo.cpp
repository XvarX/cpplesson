// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: Deducing This (C++23) — 显式对象参数                                 ║
// ║  目标: 掌握显式对象参数语法，简化 CRTP，实现递归 lambda，值类别感知          ║
// ║                                                                            ║
// ║  学习方式: Builder / ValueInspector / Cloneable 定义在头文件中。             ║
// ║  下面的代码展示链式调用、递归 lambda、值类别感知等核心用法。                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "cpp20_23/deducing_this.hpp"
#include <print>
#include <string>
#include <memory>

int main() {
    lesson::print_header("C++23 Deducing This");

    // ── Part 1: CRTP 简化 ──
    lesson::print_subtitle("Part 1: 简化 CRTP — 告别 static_cast<Derived*>");

    Builder b;
    // 链式调用: 每次 append 返回自身引用
    b.append("Hello").append(", ").append("World!");
    std::println("  Builder 链式调用: {}", b.get());

    b.clear().append("C++23").append(" Deducing ").append("This");
    std::println("  再次链式调用:     {}", b.get());

    lesson::print_note("旧 CRTP: 需要模板基类 + static_cast → 3 层胶水代码");
    lesson::print_note("新 Deducing This: 只需 template<typename Self> + self 参数");

    // ── Part 2: 递归 Lambda ──
    lesson::print_subtitle("Part 2: 递归 Lambda — lambda 可以在体内调用自己");

    // 计算阶乘的递归 lambda
    auto factorial = [](this auto&& self, int n) -> int {
        if (n <= 1) return 1;
        return n * self(n - 1);  // C++23: self 就是自身!
    };

    std::println("  递归 Lambda 阶乘:");
    std::println("    factorial(5) = {}", factorial(5));
    std::println("    factorial(7) = {}", factorial(7));

    // 二叉树遍历的递归 lambda
    lesson::print_separator();
    auto fibonacci_lambda = [](this auto&& self, int n) -> int {
        if (n <= 1) return n;
        return self(n - 1) + self(n - 2);
    };

    std::println("  递归 Lambda 斐波那契:");
    for (int i = 0; i <= 10; ++i)
        std::print("{} ", fibonacci_lambda(i));
    std::println("");

    // ── Part 3: 值类别感知 (Value Category Overloading) ──
    lesson::print_subtitle("Part 3: 值类别感知 — self& vs const self& vs self&&");

    ValueInspector vi{"Ecks"};
    vi.inspect();                        // 调用 const& 版本
    vi.modify("NewEcks");                // 调用 & 版本
    vi.inspect();                        // 调用 const& 版本
    ValueInspector{"临时对象"}.inspect(); // 调用 && 版本

    lesson::print_note("Deducing This 让一个函数名有多版本 — const&/&/&& 各自不同实现");

    // ── Part 4: self 返回类型 ──
    lesson::print_subtitle("Part 4: self 返回类型 — 返回与调用者相同的类型");

    Cloneable c{42};
    auto c2 = c.clone();             // 左值: 拷贝构造
    auto c3 = Cloneable{99}.clone(); // 右值: 移动构造
    std::println("  clone from lvalue: {}", c2.value);
    std::println("  clone from rvalue: {}", c3.value);

    // ==========================================================================
    // 常见陷阱
    // ==========================================================================
    lesson::print_separator("常见陷阱");
    std::println("  1. deducing this 成员函数不能同时是 static 和 virtual");
    std::println("     (虽然看起来像静态函数，实际上不是)");
    std::println("  2. 递归 lambda 的 self 参数不能省略模板声明 template<...>");
    std::println("  3. self 参数的类型推导规则与普通模板函数相同");
    std::println("     self&& 是转发引用 (万能引用)");
    std::println("  4. 使用 deducing this 后，this 指针不再隐式可用");
    std::println("     需要通过 self 参数访问成员 (self.member)");

    // ==========================================================================
    // 练习
    // ==========================================================================
    lesson::print_separator("练习");
    std::println("  1. 用 deducing this 实现一个可链式调用的 StringBuilder 类");
    std::println("  2. 写一个递归 lambda 来计算目录树的总文件大小 (模拟)");
    std::println("  3. 为某个类实现 inspect() 的三个重载 (const&, &, &&)");
    std::println("  4. 将之前学的 CRTP 模式用 deducing this 重写，对比代码行数");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: constexpr 增强 — new/delete、is_constant_evaluated、析构函数、        ║
// ║        std::unreachable                                                      ║
// ║  目标: 掌握 C++20/23 中 constexpr 能力的四大扩展                              ║
// ║                                                                            ║
// ║  学习方式: 所有核心类和函数定义在 constexpr_enhanced.hpp 中。                  ║
// ║  下面的代码演示编译期 new/delete、is_constant_evaluated 分支、                ║
// ║  constexpr 析构/RAII、以及 std::unreachable 的用法。                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "cpp20_23/constexpr_enhanced.hpp"

#include <print>
#include <string>
#include <array>
#include <vector>

// 前向声明: 运行时验证函数 (实现在 src/constexpr_enhanced.cpp 中)
namespace cpp20_23 {
    void verify_compile_time_array_sum();
    void verify_compile_time_object_lifetime();
    void demonstrate_is_constant_evaluated();
    void demonstrate_constexpr_raii();
    void demonstrate_constexpr_vector();
    void demonstrate_unreachable();
    void demonstrate_compile_vs_runtime();
}

int main() {
    lesson::print_header("C++20/23 constexpr 增强");

    // ════════════════════════════════════════════════════════════════════════════
    // Part 1: constexpr new/delete (C++20)
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 1: constexpr new/delete — 编译期动态内存分配");

    std::println("  C++20 之前, constexpr 函数只能操作栈上的值。");
    std::println("  C++20 允许在 constexpr 中使用 new/delete, 但有一项硬性规则:");
    std::println("    → 编译期分配的内存必须在同一常量求值中释放");
    std::println("    → 不允许编译期分配的指针\"逃逸\"到运行时");
    std::println("");

    // 演示 1: 编译期 new[]/delete[] 数组
    std::println("  示例 1: 编译期 new[] 分配数组并计算");
    {
        constexpr int result = cpp20_23::compile_time_array_sum(5);
        // result 在编译期就算好了: 0²+1²+2²+3²+4² = 30
        std::println("    compile_time_array_sum(5) = {} (在编译期计算!)", result);
        static_assert(result == 30, "编译期计算结果应为 30");
        std::println("    static_assert 通过 ✓");
    }

    // 演示 2: 编译期 operator new + construct_at
    std::println("");
    std::println("  示例 2: 编译期 operator new + std::construct_at");
    {
        constexpr int result = cpp20_23::compile_time_object_lifetime();
        std::println("    compile_time_object_lifetime() = {}", result);
        static_assert(result == 84, "42 * 2 = 84");
        std::println("    完整的对象生命周期在编译期完成! ✓");
    }

    // 运行时验证
    std::println("");
    cpp20_23::verify_compile_time_array_sum();
    cpp20_23::verify_compile_time_object_lifetime();

    // ════════════════════════════════════════════════════════════════════════════
    // Part 2: std::is_constant_evaluated() (C++20)
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 2: std::is_constant_evaluated — 编译期/运行时分支");

    std::println("  is_constant_evaluated() 是 C++20 引入的关键工具:");
    std::println("    编译期返回 true  → 可以使用仅在编译期合法的操作");
    std::println("    运行时返回 false → 可以使用运行时优化的实现");
    std::println("");

    // 编译期 sqrt 演示
    {
        constexpr double val = cpp20_23::fast_or_safe_sqrt(3.0);
        std::println("  fast_or_safe_sqrt(3.0) = {:.6f}  [编译期 — 牛顿迭代法]", val);
    }
    {
        double val = cpp20_23::fast_or_safe_sqrt(3.0);
        std::println("  fast_or_safe_sqrt(3.0) = {:.6f}  [运行时 — 运行时路径]", val);
    }

    // 编译期 bit_ceil
    std::println("");
    {
        constexpr size_t v = cpp20_23::bit_ceil_compile_or_runtime(100);
        std::println("  bit_ceil_compile_or_runtime(100) = {}  [编译期]", v);
        static_assert(v == 128, "下一个 2 的幂应为 128");
    }

    std::println("");
    cpp20_23::demonstrate_is_constant_evaluated();

    // ════════════════════════════════════════════════════════════════════════════
    // Part 3: constexpr 析构函数 + RAII (C++20)
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 3: constexpr 析构函数 — 编译期 RAII");

    std::println("  C++17: constexpr 对象必须具有平凡析构函数");
    std::println("         → std::vector、std::string 不能在编译期使用");
    std::println("  C++20: 允许 constexpr 析构函数");
    std::println("         → 编译期 RAII 成为现实!");
    std::println("");

    // 演示 ConstexprBuffer — 编译期 RAII
    {
        std::println("  ConstexprBuffer: 编译期管理动态数组的 RAII 类");
        std::println("    构造时 new[], 析构时自动 delete[]");
        constexpr int sum = cpp20_23::compile_time_raii_sum();
        std::println("    compile_time_raii_sum() = {}  (1+2+...+10)", sum);
        static_assert(sum == 55, "1..10 的和应为 55");
        std::println("    离开作用域时析构函数在编译期自动调用 ✓");
    }

    // 演示 ConstexprVector — 编译期动态数组
    std::println("");
    {
        std::println("  ConstexprVector<T>: 类似 std::vector 的编译期容器");
        constexpr auto [sum, sz] = cpp20_23::compile_time_vector_use();
        std::println("    compile_time_vector_use() → sum={}, size={}", sum, sz);
        std::println("    push_back 5 次, 自动扩容, 自动析构 — 全在编译期!");
        static_assert(sum == 150, "10+20+30+40+50 = 150");
        static_assert(sz == 5);
        std::println("    所有 static_assert 通过 ✓");
    }

    std::println("");
    cpp20_23::demonstrate_constexpr_raii();
    cpp20_23::demonstrate_constexpr_vector();

    // ════════════════════════════════════════════════════════════════════════════
    // Part 4: std::unreachable() (C++23)
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 4: std::unreachable — 标记不可达代码路径");

    std::println("  C++23 引入 std::unreachable() 作为标记不可达代码的标准工具:");
    std::println("    之前: __builtin_unreachable() (GCC) / __assume(false) (MSVC)");
    std::println("    现在: 标准、可移植、跨平台 ← std::unreachable()");
    std::println("");

    // 编译期验证
    {
        constexpr int r1 = cpp20_23::exhaustive_switch(1);
        constexpr int r2 = cpp20_23::positive_only(7);
        constexpr int r3 = cpp20_23::lookup_or_default(4);

        std::println("  exhaustive_switch(1) = {}  [编译期: case 1 匹配]", r1);
        std::println("  positive_only(7)    = {}  [编译期: if (x>0) 分支]", r2);
        std::println("  lookup_or_default(4) = {}  [编译期: case 4 匹配]", r3);

        static_assert(r1 == 10);
        static_assert(r2 == 14);
        static_assert(r3 == 500);
        std::println("  所有 static_assert 通过 ✓");
    }

    std::println("");
    cpp20_23::demonstrate_unreachable();

    // ════════════════════════════════════════════════════════════════════════════
    // Part 5: 综合演示
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 5: 综合演示 — 编译期构建数据并计算统计");

    std::println("  利用 constexpr new/delete + constexpr 析构 + constexpr 容器,");
    std::println("  我们可以在编译期完成以前无法想象的工作:");
    std::println("    → 构建 20 个元素的动态数组 (编译期 new[])");
    std::println("    → 使用 RAII 容器管理内存 (编译期析构)");
    std::println("    → 遍历计算统计信息 (编译期 for 循环)");
    std::println("    → 结果烧结到二进制中 (零运行时开销!)");
    std::println("");

    cpp20_23::demonstrate_compile_vs_runtime();

    // ════════════════════════════════════════════════════════════════════════════
    // 常见陷阱
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_separator("常见陷阱");

    std::println("  1. 编译期 new 的指针不能\"逃逸\"到运行时");
    std::println("     → 不能作为返回值、引用参数、全局变量传递");
    std::println("     → 否则: \"allocation performed here was not deallocated\" 编译错误");
    std::println("");
    std::println("  2. if constexpr 与 is_constant_evaluated 的陷阱");
    std::println("     ✗ if constexpr (std::is_constant_evaluated())  // 始终视为 true!");
    std::println("     ✓ if (std::is_constant_evaluated())            // 正确用法");
    std::println("     原因: if constexpr 的条件必须是编译期常量，而非函数调用结果");
    std::println("");
    std::println("  3. constexpr 析构函数 ≠ 自动让类成为 literal type");
    std::println("     → 还需要: 所有非静态成员为 literal type + 至少一个 constexpr 构造");
    std::println("");
    std::println("  4. std::unreachable() 不是错误处理机制");
    std::println("     → 它告知编译器\"这里不可达\"来优化代码");
    std::println("     → 如果运行时真的执行到 → UB (未定义行为)");
    std::println("     → 不要用它替代异常、错误码或 assert");
    std::println("");
    std::println("  5. 编译器对 constexpr 内存分配有限制");
    std::println("     → 不同编译器有不同的编译期内存上限");
    std::println("     → 大量编译期分配可能显著增加编译时间");

    // ════════════════════════════════════════════════════════════════════════════
    // 练习
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_separator("练习");

    std::println("  1. 用 constexpr new/delete 实现一个编译期链表 (单向)");
    std::println("     链表的构造、遍历求长度、求和、销毁必须全在 constexpr 函数中");
    std::println("     用 static_assert 验证计算结果");
    std::println("");
    std::println("  2. 用 is_constant_evaluated() 实现通用 abs() 函数");
    std::println("     编译期用朴素的条件判断, 运行时期望调用 std::abs");
    std::println("     对比编译期和运行时输出的差异");
    std::println("");
    std::println("  3. 扩展 ConstexprVector, 添加 pop_back() 和 clear() 方法");
    std::println("     确保在 constexpr 上下文中可以正确工作");
    std::println("     注意内存管理: delete[] 后指针置空");
    std::println("");
    std::println("  4. 实现一个 constexpr 字符串拼接函数");
    std::println("     输入: std::string_view a, b");
    std::println("     输出: 一个 std::array<char, N> (N = a.size() + b.size() + 1)");
    std::println("     提示: 利用编译期 new 做临时缓冲区, 结果用 array 传递到运行时");
    std::println("");
    std::println("  5. 找一段现有的 switch-case 代码, 分析哪些 default 分支是真正不可达的");
    std::println("     把那些不可达的 default 替换为 std::unreachable()");
    std::println("     观察: 编译器警告是否消失? 生成的汇编代码是否有变化?");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: constexpr 增强 — new/delete、is_constant_evaluated、析构函数、        ║
// ║        std::unreachable                                                      ║
// ║  目标: 掌握 C++20/23 中 constexpr 能力的四大扩展                              ║
// ║                                                                            ║
// ║  注意: MSVC 对 constexpr new/delete 支持有限 (截止 2024)，                   ║
// ║        部分编译期验证在 MSVC 下会跳过。GCC 14+ / Clang 18+ 完整支持。        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "cpp20_23/constexpr_enhanced.hpp"

#include <iostream>
#include <print>
#include <string>
#include <array>
#include <vector>

// MSVC 对 constexpr new/delete 支持不完整 — 条件跳过编译期验证
#if defined(_MSC_VER)
  #define CONSTEXPR_NEW_SUPPORTED 0
#else
  #define CONSTEXPR_NEW_SUPPORTED 1
#endif

// 直接调用 ::operator new 的编译期求值需要 C++26 运行库支持
// (__cpp_lib_constexpr_new >= 202406L — libstdc++ 只在 C++26 模式提供);
// 而 new 表达式 (new T[n]) 自 C++20 起就可用于编译期求值
#if CONSTEXPR_NEW_SUPPORTED && defined(__cpp_lib_constexpr_new) && (__cpp_lib_constexpr_new >= 202406L)
  #define CONSTEXPR_OPNEW_SUPPORTED 1
#else
  #define CONSTEXPR_OPNEW_SUPPORTED 0
#endif

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

#if !CONSTEXPR_NEW_SUPPORTED
    std::cout << "  ⚠️  检测到 MSVC 编译器 — constexpr new/delete 编译期求值暂不支持\n";
    std::cout << "      将演示运行时行为。编译期验证请使用 GCC 14+ / Clang 18+\n\n";
#endif

    // ════════════════════════════════════════════════════════════════════════════
    // Part 1: constexpr new/delete (C++20)
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 1: constexpr new/delete — 编译期动态内存分配");

    std::cout << "  C++20 之前, constexpr 函数只能操作栈上的值。\n";
    std::cout << "  C++20 允许在 constexpr 中使用 new/delete, 但有一项硬性规则:\n";
    std::cout << "    → 编译期分配的内存必须在同一常量求值中释放\n";
    std::cout << "    → 不允许编译期分配的指针\"逃逸\"到运行时\n\n";

    // 演示 1: 运行时调用 (所有编译器都支持)
    std::cout << "  示例 1: 运行时调用 constexpr 函数\n";
    {
        int result = cpp20_23::compile_time_array_sum(5);
        std::cout << "    compile_time_array_sum(5) = " << result;
#if CONSTEXPR_NEW_SUPPORTED
        constexpr int ct_result = cpp20_23::compile_time_array_sum(5);
        std::cout << "  (编译期: " << ct_result << ")";
        static_assert(ct_result == 30, "编译期计算结果应为 30");
#endif
        std::cout << "\n";
        std::cout << "    预期: 0+1+4+9+16 = 30\n";
    }

    // 演示 2: operator new + construct_at
    std::cout << "\n";
    std::cout << "  示例 2: 运行时 operator new + std::construct_at\n";
    {
        int result = cpp20_23::compile_time_object_lifetime();
        std::cout << "    compile_time_object_lifetime() = " << result;
#if CONSTEXPR_OPNEW_SUPPORTED
        constexpr int ct_result = cpp20_23::compile_time_object_lifetime();
        std::cout << "  (编译期: " << ct_result << ")";
        static_assert(ct_result == 84, "42 * 2 = 84");
#else
        std::cout << "  (编译期求值 ::operator new 需 C++26 运行库 — 仅演示运行时)";
#endif
        std::cout << "\n";
        std::cout << "    同一个函数: 编译期构造 42 → 返回 84\n";
    }

    std::cout << "\n";
    cpp20_23::verify_compile_time_array_sum();
    cpp20_23::verify_compile_time_object_lifetime();

    // ════════════════════════════════════════════════════════════════════════════
    // Part 2: std::is_constant_evaluated() (C++20)
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 2: std::is_constant_evaluated — 编译期/运行时分支");

    std::cout << "  is_constant_evaluated() 是 C++20 引入的关键工具:\n";
    std::cout << "    编译期返回 true  → 可以使用仅在编译期合法的操作\n";
    std::cout << "    运行时返回 false → 可以使用运行时优化的实现\n\n";

    // 运行时演示
    {
        double val = cpp20_23::fast_or_safe_sqrt(3.0);
        std::cout << "  fast_or_safe_sqrt(3.0) = " << val << "  [运行时]\n";
    }
#if CONSTEXPR_NEW_SUPPORTED
    {
        constexpr double val = cpp20_23::fast_or_safe_sqrt(3.0);
        std::cout << "  fast_or_safe_sqrt(3.0) = " << val << "  [编译期 — 牛顿迭代法]\n";
    }
#endif

    {
        size_t v = cpp20_23::bit_ceil_compile_or_runtime(100);
        std::cout << "\n  bit_ceil_compile_or_runtime(100) = " << v << "  [运行时]\n";
#if CONSTEXPR_NEW_SUPPORTED
        constexpr size_t cv = cpp20_23::bit_ceil_compile_or_runtime(100);
        static_assert(cv == 128, "下一个 2 的幂应为 128");
        std::cout << "  bit_ceil_compile_or_runtime(100) = " << cv << "  [编译期] ✓\n";
#endif
    }

    std::cout << "\n";
    cpp20_23::demonstrate_is_constant_evaluated();

    // ════════════════════════════════════════════════════════════════════════════
    // Part 3: constexpr 析构函数 + RAII (C++20)
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 3: constexpr 析构函数 — 编译期 RAII");

    std::cout << "  C++17: constexpr 对象必须具有平凡析构函数\n";
    std::cout << "  C++20: 允许 constexpr 析构函数 → 编译期 RAII 成为现实!\n\n";

    // ConstexprBuffer
    {
        std::cout << "  ConstexprBuffer: 编译期管理动态数组的 RAII 类\n";
        int sum = cpp20_23::compile_time_raii_sum();
        std::cout << "    compile_time_raii_sum() = " << sum << "  (1+2+...+10, 运行时)\n";
#if CONSTEXPR_NEW_SUPPORTED
        constexpr int ct_sum = cpp20_23::compile_time_raii_sum();
        static_assert(ct_sum == 55, "1..10 的和应为 55");
        std::cout << "    编译期结果: " << ct_sum << "  static_assert 通过 ✓\n";
#endif
    }

    // ConstexprVector
    std::cout << "\n";
    {
        std::cout << "  ConstexprVector<T>: 类似 std::vector 的编译期容器\n";
        auto [sum, sz] = cpp20_23::compile_time_vector_use();
        std::cout << "    compile_time_vector_use() → sum=" << sum << ", size=" << sz << "\n";
#if CONSTEXPR_NEW_SUPPORTED
        // 注意: 不用 constexpr 结构化绑定 — 同作用域先出现运行时绑定时,
        // GCC 16 会拒绝后续 constexpr 绑定参与常量表达式 (P2686 边界情况)
        constexpr auto ct = cpp20_23::compile_time_vector_use();
        static_assert(ct.first == 150, "10+20+30+40+50 = 150");
        static_assert(ct.second == 5);
        std::cout << "    编译期: static_assert 全部通过 ✓\n";
#endif
    }

    std::cout << "\n";
    cpp20_23::demonstrate_constexpr_raii();
    cpp20_23::demonstrate_constexpr_vector();

    // ════════════════════════════════════════════════════════════════════════════
    // Part 4: std::unreachable() (C++23)
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 4: std::unreachable — 标记不可达代码路径");

    std::cout << "  C++23 引入 std::unreachable() 作为标记不可达代码的标准工具\n";
    std::cout << "    之前: __builtin_unreachable() (GCC) / __assume(false) (MSVC)\n";
    std::cout << "    现在: 标准、可移植、跨平台 ← std::unreachable()\n\n";

    // 运行时验证 (std::unreachable 不需要 constexpr new，所有编译器都支持)
    {
        constexpr int r1 = cpp20_23::exhaustive_switch(1);
        constexpr int r2 = cpp20_23::positive_only(7);
        constexpr int r3 = cpp20_23::lookup_or_default(4);

        std::cout << "  exhaustive_switch(1) = " << r1 << "  [编译期: case 1 匹配]\n";
        std::cout << "  positive_only(7)    = " << r2 << "  [编译期: if (x>0) 分支]\n";
        std::cout << "  lookup_or_default(4) = " << r3 << "  [编译期: case 4 匹配]\n";

        static_assert(r1 == 10);
        static_assert(r2 == 14);
        static_assert(r3 == 500);
        std::cout << "  所有 static_assert 通过 ✓\n";
    }

    std::cout << "\n";
    cpp20_23::demonstrate_unreachable();

    // ════════════════════════════════════════════════════════════════════════════
    // Part 5: 综合演示
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 5: 综合演示 — 编译期构建数据并计算统计");

    std::cout << "  利用 constexpr new/delete + constexpr 析构 + constexpr 容器,\n";
    std::cout << "  我们可以在编译期完成以前无法想象的工作:\n";
    std::cout << "    → 构建 20 个元素的动态数组 (编译期 new[])\n";
    std::cout << "    → 使用 RAII 容器管理内存 (编译期析构)\n";
    std::cout << "    → 遍历计算统计信息 (编译期 for 循环)\n";
    std::cout << "    → 结果烧结到二进制中 (零运行时开销!)\n\n";

    cpp20_23::demonstrate_compile_vs_runtime();

    // ════════════════════════════════════════════════════════════════════════════
    // 常见陷阱
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_separator("常见陷阱");

    std::cout << "  1. 编译期 new 的指针不能\"逃逸\"到运行时\n";
    std::cout << "     → 不能作为返回值、引用参数、全局变量传递\n";
    std::cout << "     → 否则: \"allocation performed here was not deallocated\" 编译错误\n\n";
    std::cout << "  2. if constexpr 与 is_constant_evaluated 的陷阱\n";
    std::cout << "     ✗ if constexpr (std::is_constant_evaluated())  // 始终视为 true!\n";
    std::cout << "     ✓ if (std::is_constant_evaluated())            // 正确用法\n\n";
    std::cout << "  3. constexpr 析构函数 ≠ 自动让类成为 literal type\n";
    std::cout << "     → 还需要: 所有非静态成员为 literal type + 至少一个 constexpr 构造\n\n";
    std::cout << "  4. std::unreachable() 不是错误处理机制\n";
    std::cout << "     → 它告知编译器\"这里不可达\"来优化代码\n";
    std::cout << "     → 如果运行时真的执行到 → UB (未定义行为)\n";

    // ════════════════════════════════════════════════════════════════════════════
    // 练习
    // ════════════════════════════════════════════════════════════════════════════
    lesson::print_separator("练习");

    std::cout << "  1. 用 constexpr new/delete 实现一个编译期链表 (单向)\n";
    std::cout << "     链表的构造、遍历求长度、求和、销毁必须全在 constexpr 函数中\n\n";
    std::cout << "  2. 用 is_constant_evaluated() 实现通用 abs() 函数\n";
    std::cout << "     编译期用朴素的条件判断, 运行时期望调用 std::abs\n\n";
    std::cout << "  3. 扩展 ConstexprVector, 添加 pop_back() 和 clear() 方法\n";
    std::cout << "     确保在 constexpr 上下文中可以正确工作\n\n";
    std::cout << "  4. 实现一个 constexpr 字符串拼接函数\n";
    std::cout << "     输入: std::string_view a, b → 输出: 编译期拼接结果\n\n";
    std::cout << "  5. 找一段现有的 switch-case 代码, 分析哪些 default 分支是真正不可达的\n";
    std::cout << "     把那些不可达的 default 替换为 std::unreachable()\n";
}

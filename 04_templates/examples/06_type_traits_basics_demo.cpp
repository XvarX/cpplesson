// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  04_templates — 06 <type_traits> 基础与 SFINAE  演示                        ║
// ║  学习目标:                                                                    ║
// ║    1. 掌握 <type_traits> 中最常用的类型判断和类型转换工具                     ║
// ║    2. 理解 SFINAE 的核心思想：替换失败不算错误                                ║
// ║    3. 学会 std::enable_if 的条件编译技巧                                     ║
// ║    4. 理解 void_t 技巧检测类型成员                                           ║
// ║    5. 学会用 if constexpr 替代大部分 SFINAE 场景                             ║
// ║  前置: 03_specialization, 04_variadic_template                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "templates/type_traits_basics.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <vector>
#include <list>

int main() {
    lesson::print_header("04_templates — 06 type_traits 与 SFINAE");

    // ── Part 1: type_traits 常用工具 ──
    demonstrate_type_traits();

    // ── Part 2: SFINAE + enable_if ──
    lesson::print_subtitle("Part 2: SFINAE + enable_if 重载");
    {
        std::println("{}", describe_sfinae(42));        // 匹配整数重载
        std::println("{}", describe_sfinae(3.14159));    // 匹配浮点重载
        // describe_sfinae("hello");  // 编译错误：没有匹配的重载
    }

    // ── Part 3: void_t 技巧 ──
    lesson::print_subtitle("Part 3: void_t 检测成员");
    {
        std::println("vector<int> 有 iterator 吗? {}", has_iterator_v<std::vector<int>>);
        std::println("list<double> 有 iterator 吗? {}", has_iterator_v<std::list<double>>);
        std::println("int          有 iterator 吗? {}", has_iterator_v<int>);
    }

    // ── Part 4: if constexpr 替代方案 ──
    lesson::print_subtitle("Part 4: if constexpr 替代 SFINAE");
    {
        std::println("{}", describe_if_constexpr(100));
        std::println("{}", describe_if_constexpr(2.718));
        std::println("{}", describe_if_constexpr("你好"));
        std::println("{}", describe_if_constexpr(std::string("世界")));
        std::println("{}", describe_if_constexpr(std::vector{1, 2, 3}));
    }

    lesson::print_separator("常见陷阱速查");
    std::println("1. enable_if 放在返回值类型、默认模板参数或非类型模板参数位置都可以");
    std::println("2. SFINAE 只在模板参数替换时生效, 函数体内的错误照常报错");
    std::println("3. void_t 可以检测表达式有效性, 不限于类型成员");
    std::println("4. if constexpr 的条件必须是编译期常量表达式");
    std::println("5. C++20 concepts 是更好的替代方案 (见下一课), 但理解 SFINAE 帮你读懂旧代码");

    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 用 enable_if 实现两个重载函数 print_size(T val):
//    - 一个用于指针类型 (is_pointer_v)，打印"指针，大小=8（64位）"
//    - 一个用于非指针类型，打印"对象，大小=sizeof(T)"
//    测试 int x; print_size(x); 和 print_size(&x);
// 2. 用 void_t 检测一个类型是否有 .reserve(size_t) 方法。
//    提示: void_t<decltype(std::declval<T&>().reserve(size_t{}))>
// 3. 重写练习 1 的 print_size 为 if constexpr 版本。
//    对比两种写法的可读性：你更愿意维护哪种？

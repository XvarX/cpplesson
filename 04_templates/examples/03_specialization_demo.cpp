// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  04_templates — 03 模板特化 演示                                             ║
// ║  学习目标:                                                                    ║
// ║    1. 理解全特化：为特定类型提供完全不同的实现                                ║
// ║    2. 理解偏特化：针对一类类型（如指针、const）提供特殊实现                    ║
// ║    3. 掌握 C++14 变量模板及其特化                                            ║
// ║    4. 理解 traits 类模板的经典模式                                           ║
// ║  前置: 02_class_template                                                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "templates/specialization.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>

int main() {
    lesson::print_header("04_templates — 03 模板特化");

    // ── Part 1: 全特化 ──
    lesson::print_subtitle("Part 1: 全特化");
    {
        std::println("TypeName<int>::name()         = {}", TypeName<int>::name());
        std::println("TypeName<double>::name()      = {}", TypeName<double>::name());
        std::println("TypeName<std::string>::name() = {}", TypeName<std::string>::name());
        std::println("TypeName<char>::name()        = {}", TypeName<char>::name());
    }

    // ── Part 2: 偏特化 ──
    lesson::print_subtitle("Part 2: 偏特化（指针、const）");
    {
        TypeTraits<int>::print();         // 通用模板
        TypeTraits<int*>::print();        // 匹配偏特化 T* → T=int
        TypeTraits<const double>::print();// 匹配偏特化 const T → T=double
    }

    // ── Part 3: 变量模板 ──
    lesson::print_subtitle("Part 3: 变量模板 (C++14)");
    {
        std::println("pi<double>  = {:.15f}", pi<double>);
        std::println("pi<float>   = {:.7f}", pi<float>);
        std::println("pi<int>     = {}     (特化版，取整)", pi<int>);

        std::println("is_void_v<void>  = {}", is_void_v<void>);
        std::println("is_void_v<int>   = {}", is_void_v<int>);
        // 标准库有现成的: std::is_void_v<T>
    }

    // ── Part 4: traits 模式 ──
    lesson::print_subtitle("Part 4: traits 类模板模式");
    {
        smart_print(42);
        smart_print(3.14);
        smart_print(std::string("hello"));
        smart_print("C 风格字符串（其实会被推导成 const char[5] 哦）");
    }

    lesson::print_separator("常见陷阱速查");
    std::println("1. 函数模板不支持偏特化，只能用重载或类模板间接实现");
    std::println("2. 全特化的声明必须在通用模板之后、使用之前");
    std::println("3. 偏特化时，模板参数列表必须与通用模板的"兼容"");
    std::println("4. 变量模板是 C++14 特性，C++11 不能用");

    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 为 TypeName 添加 float、char、bool 的全特化。
//    思考：如果每次新增类型都要全特化，代码会如何膨胀？
// 2. 添加一个偏特化 TypeTraits<T&>（引用类型），描述引用类型的特性。
//    提示：注意引用折叠。
// 3. 写一个自定义 traits：is_container<T>，判断 T 是否是一个容器类型。
//    提示：检测 T 是否有 begin() 和 end() 方法（可以用 SFINAE 或 concept）。
// 4. 创建变量模板 e<T> 表示自然常数 2.71828...，并对 int 特化为 3。

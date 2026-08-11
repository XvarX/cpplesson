// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  type_traits_basics.cpp — demonstrate_type_traits() 的非模板实现             ║
// ║  这是本模块中极少数可以分离到 .cpp 的函数（因为它不依赖模板参数）              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "templates/type_traits_basics.hpp"
#include "shared/lesson_utils.hpp"

void demonstrate_type_traits() {
    lesson::print_subtitle("Part 1: <type_traits> 常用工具演示");

    // 类型判断
    std::println("is_integral_v<int>     = {}", std::is_integral_v<int>);
    std::println("is_integral_v<double>  = {}", std::is_integral_v<double>);
    std::println("is_same_v<int, int32_t>= {}", std::is_same_v<int, int32_t>);
    std::println("is_same_v<int, long>   = {}", std::is_same_v<int, long>);

    // 类型转换
    using Raw = const int&;
    using Stripped = std::remove_cvref_t<Raw>;  // C++20: 同时去掉 const/volatile/引用
    std::println("原始类型: const int&  →  remove_cvref_t → {}",
                 typeid(Stripped).name());

    // 条件类型选择 (编译期三元运算符)
    using Selected = std::conditional_t<true, int, double>;
    std::println("conditional_t<true, int, double> = {}", typeid(Selected).name());
}

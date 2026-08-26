#include <print>
#include <string>
#include <string_view>
#include <cmath>
#include "shared/lesson_utils.hpp"
#include "fundamentals/my_functions_basics.hpp"

int main() {
    lesson::print_header("05 函数声明/定义、重载、默认参数、inline、[[nodiscard]]");

    lesson::print_subtitle("Part 1: 声明 vs 定义");

    fundamentals::print_info(42);
    fundamentals::print_info(3.14);

    fundamentals::print_info(std::string_view{"C++23"});

    auto greet = [](std::string_view name = "艾克斯", int times = 1) {
        for(int i=0; i < times; ++i) {
            std::println("你好, {}!", name);
        }
    };

    greet();
    greet("小明");
    greet("小红", 3);

    std::println("fast_max(10, 20) = {}", fundamentals::fast_max(10, 20));

    int important = fundamentals::compute_important_value();
    // std::println("compute_important_value() = {}", important);

    bool valid = fundamentals::validate_input(10);
    std::println("validate_input(10) = {}", valid);

    auto r1 = fundamentals::multiply(3, 4);
    auto r2 = fundamentals::multiply(3.0, 4.0);
    auto r3 = fundamentals::multiply(3, 4.5);

    std::println("multiply(3,4)    = {}", r1);
    std::println("multiply(3.0,4.0) = {}", r2);
    std::println("multiply(3,4.5)   = {}", r3);

    auto add_vals = [](int a, int b) -> int {
        return a+b;
    };

    std::println("lambda add: {} + {} = {}", 5, 7, add_vals(5, 7));

    constexpr int fact5 = fundamentals::factorial(5);
    static_assert(fundamentals::factorial(5) == 120, "编译期验证 factorial(5)");


    
}
#include <print>
#include <string>
#include <array>
#include "shared/lesson_utils.hpp"
#include "fundamentals/const_static.hpp"

int main() {
    lesson::print_header("02 const / constexpr / constinit / static / extern / inline");

    lesson::print_subtitle("Part 1: const 限定符");

    const int maxplayers = 100;

    const double pi = 3.141592653589793;
    const std::string app_name = "C++学习器";

    std::println("最大玩家数: {}", maxplayers);
    std::println("pi: {}", pi);
    std::println("应用名: {}", app_name);

    int value = 42;
    const int* p1 = &value;
    int const* p2 = &value;
    int* const p3 = &value;

    const int* const p4 = &value;

    std::println("*p3 = {} （可以修改，因为p3指向非 const int)", *p3);
    *p3 = 100;

    std::println("修改后 *p3 = {}", *p3);

    lesson::print_subtitle("Part 2: constexpr 编译期常量");

    constexpr int buffer_size = 4096;
    constexpr double e = 2.718281828459045;

    constexpr int squared = fundamentals::square(10);

    int buffer[buffer_size / 8] {};

    std::println("缓冲区大小: {}", buffer_size);
    std::println("自然常数 e: {}", e);
    std::println("square(10) 编译期计算 = {}", squared);

    static_assert(fundamentals::square(5) == 25, "constexpr 函数应该在编译期执行");

    int n = 10;
    const int cm = n * 2;

    lesson::print_subtitle("Part 3: constinit (C++20)");

    constinit static int init_ok = fundamentals::square(5);
    std::println("constinit 初始值: {}", init_ok);
    init_ok = 99;
    std::println("修改 constinit 变量: {}", init_ok);

    lesson::print_subtitle("Part 4: static 存储器");

    std::println("create_counter() 第1次调用: {}", fundamentals::create_counter());
    std::println("create_counter() 第2次调用: {}", fundamentals::create_counter());
    std::println("create_counter() 第3次调用: {}", fundamentals::create_counter());

    lesson::print_subtitle("Part 6: inline 变量 (C++17)");

    std::println("inline counter （修改前）: {}", fundamentals::inline_counter);
    ++fundamentals::inline_counter;
    std::println("inline counter （修改后）: {}", fundamentals::inline_counter);

    int runtime_val = fundamentals::square(n);

    constexpr int c5 = fundamentals::compile_square(5);

    
    std::println("constexpr square(n) 运行期: {}", runtime_val);
    std::println("consteval compile_square(5) 编译期: {}", c5);

    static_assert(fundamentals::factorial(5) == 120, "constexpr 函数应该在编译期执行");
    
    int x;
    std::cin >> x;
    int square_x = fundamentals::square(x);
    std::println("square x: {}", square_x);

    constinit static int aaa = 100;

}
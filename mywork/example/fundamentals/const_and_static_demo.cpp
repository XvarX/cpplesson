#include <print>
#include <string>
#include <array>
#include "shared/lesson_utils.hpp"

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

    constexpr int buff_size = 4096;
    constexpr double e = 2.718281828459045;



}
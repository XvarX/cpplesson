#pragma once

#include <string_view>

namespace fundamentals {
    void print_info(int value);
    void print_info(double value);
    void print_info(std::string_view msg);

    template<typename T, typename U>
    auto multiply(T a, U b) -> decltype(a *b) {
        return a * b;
    }

    [[nodiscard]] int compute_important_value();
    [[nodiscard]] bool validate_input(int x);

    inline int fast_max(int a, int b) {
        return (a > b) ? a : b;
    }

    constexpr int factorial(int n) {
        return (n <= 1) ? 1 : n * factorial(n - 1);
    }
}
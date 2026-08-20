#include <string_view>

namespace fundamentals {
    constexpr int square(int x) {
        return x*x;
    }

    consteval int compile_square(int x) {
        return x*x;
    }

    constexpr int factorial(int x) {
        if(x == 0) {
            return 1;
        }
        return factorial(x-1)*x;
    }

    int create_counter();

    inline int inline_counter = 0;

    int get_counter_value();
}
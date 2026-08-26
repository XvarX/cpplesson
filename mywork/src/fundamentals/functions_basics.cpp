#include "fundamentals/my_functions_basics.hpp"
#include <print>

namespace fundamentals {
    void print_info(int value) {
        std::println("[int]     值 = {}", value);
    }

    void print_info(double value) {
        std::println("[double] 值 = {:.6f}", value);
    }

    void print_info(std::string_view msg) {
        std::println("[string] 消息 = {}", msg);
    }

    [[nodiscard]] int compute_important_value() {
        return 42;
    }

    [[nodiscard]] bool validate_input(int x) {
        return x > 0;
    }
    
}
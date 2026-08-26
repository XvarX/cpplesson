#include <print>
#include <string>
#include <string_view>
#include <vector>
#include <cstdlib>
#include "shared/lesson_utils.hpp"
#include "fundamentals/control_flow.hpp"

int main() {
    int score = 85;

    if (score >= 90) {
        std::println("成绩: {} -> 优秀", score);
    } else if (score >= 80) {
        std::println("成绩: {} -> 良好", score);
    } else if (score >= 60) {
        std::println("成绩: {} -> 及格", score);
    } else {
        std::println("成绩: {} -> 需要进步", score);
    }
}
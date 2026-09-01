#include <print>
#include <string>
#include <string_view>
#include <vector>
#include <cstdlib>
#include "shared/lesson_utils.hpp"
#include "fundamentals/my_control_flow.hpp"

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

    if(score > 0) std::println("score 是正数");

    if (auto result = fundamentals::classify_number(score); result == "正数") {
        std::println("if-init: score 是正数({})", result);
    }

    lesson::print_subtitle("Part 2: switch");

    std::println("day_name(1): {}", fundamentals::day_name(1));
    std::println("day_name(5): {}", fundamentals::day_name(5));
    std::println("day_name(8): {}", fundamentals::day_name(8));

    switch(int day = 3; day) {
        case 1: std::println("switch-init: 周一"); break;
        case 2: std::println("switch-init: 周二"); break;
        case 3: std::println("switch-init: 周三"); break;
        default: std::println("switch-init: 其他");
    }
    
    lesson::print_subtitle("Part 3: [[fallthrough]]");

    fundamentals::describe_type('a');
    fundamentals::describe_type('b');
    fundamentals::describe_type('5');

    lesson::print_subtitle("Part 4: for 循环");

    std::print("传统 for (1~5)");

    for (int i = 1; i <= 5; ++i) {
        std::print("{} ", i);
    }
    std::println("");

    std::vector<int> nums = {10, 20, 30, 40, 50};
    std::print("range-for: ");

    for (int n : nums) {
        std::print("{} ", n);
    }

    std::println("");

    std::print("range-for with init: ");
    for (auto vec = std::vector{1,2,3}; int v: vec) {
        std::print("{} ", v);
    }
    std::println("");

    for (int& n : nums) { n *= 2;}
    std::println("翻倍后: {}", nums);

    lesson::print_subtitle("Part 5: while 和 do-while");

    std::print("while 倒计时: ");
    int countdown = 5;
    while (countdown > 0) {
        std::print("{} ", countdown);
        --countdown;
    }
    std::println("发射!");

    std::print("do-while (至少执行一次)：");
    int x = 0;
    do {
        std::print("{} ", x);
        ++x;
    } while (false);
    std::println("即使条件为false也执行了一次");

    lesson::print_subtitle("Part 6: break 和 continue");

    std::print("寻找第一个 > 50 的数: ");
    for (int n : nums) {
        if (n > 50) {
            std::println("找到 {}!", n);
        }
        std::print("检查 {}, ", n);
    }

    std::print("只打印奇数: ");
    for (int i = 1; i <= 10; ++i) {
        if (i % 2 == 0) continue;
        std::print("{} ", i);
    }

    std::println("");

    lesson::print_subtitle("Part 7: goto");

    int value = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (i * j > 6) {
                value = i * j;
                goto found;
            }
        }
    }

found:
    std::print("goto: 第一个 i*j > 6 的结果是 {}", value);

    std::println("FizzBuzz (1~15):");
    for (int i = 1; i <= 15; ++i) {
        if (i % 15 == 0) {
            std::print("FizzBuzz ");
        } else if (i % 3 == 0) {
            std::print("Fizz ");
        } else if (i % 5 == 0) {
            std::print("Buzz ");
        } else {
            std::print("{} ", i);
        }
    }
    std::print("");

    fundamentals::printmonthday(2);

    fundamentals::print99();

    std::println("calsum {}", fundamentals::calsum(12345));

    std::vector<int> myVector = {1,2,3,4,5,6, 7};
    std::println("binfind {}", fundamentals::findbinary(myVector, 0));
}
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  示例: 10_chrono_random_demo — 时间与随机数                                  ║
// ║  主题: duration / time_point / clock / 字面量 / 随机引擎 / 随机分布           ║
// ║  演示如何调用 stl_lib 中的教学函数来学习时间库与随机数库的用法                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/chrono_random.hpp"

int main() {
    lesson::print_header("10_chrono_random — 时间与随机数");

    stl_learn::part1_duration();
    stl_learn::part2_time_point();
    stl_learn::part3_literals();
    stl_learn::part4_random_engine();
    stl_learn::part5_distributions();
    stl_learn::part6_ratio();
    stl_learn::chrono_random_pitfalls();
    stl_learn::chrono_random_exercises();
}

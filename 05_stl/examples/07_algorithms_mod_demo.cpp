// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  示例: 07_algorithms_mod_demo — 修改序列操作                                 ║
// ║  主题: copy / transform / remove / replace / generate / fill / unique        ║
// ║  演示如何调用 stl_lib 中的教学函数来学习修改序列算法的用法                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/algorithms_mod.hpp"

int main() {
    lesson::print_header("07_algorithms_mod — 修改序列操作");

    stl_learn::part1_copy();
    stl_learn::part2_transform();
    stl_learn::part3_remove();
    stl_learn::part4_replace();
    stl_learn::part5_more_mod();
    stl_learn::part_pitfalls();
    stl_learn::part_exercises();
}

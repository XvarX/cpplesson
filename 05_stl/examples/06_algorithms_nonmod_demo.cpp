// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  示例: 06_algorithms_nonmod_demo — 非修改序列操作                            ║
// ║  主题: find / count / search / equal / mismatch / all_of / any_of / none_of  ║
// ║  演示如何调用 stl_lib 中的教学函数来学习非修改算法的用法                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/algorithms_nonmod.hpp"

int main() {
    lesson::print_header("06_algorithms_nonmod — 非修改序列操作");

    stl_learn::part1_find();
    stl_learn::part2_count();
    stl_learn::part3_search();
    stl_learn::part4_equal_mismatch();
    stl_learn::part5_predicates();
    stl_learn::part6_adjacent();
    stl_learn::part_pitfalls();
    stl_learn::part_exercises();
}

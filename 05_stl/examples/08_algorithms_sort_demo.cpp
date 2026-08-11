// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  示例: 08_algorithms_sort_demo — 排序与二分查找                              ║
// ║  主题: sort / stable_sort / partial_sort / merge / heap / partition           ║
// ║  演示如何调用 stl_lib 中的教学函数来学习排序与二分查找的用法                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/algorithms_sort.hpp"

int main() {
    lesson::print_header("08_algorithms_sort — 排序与二分查找");

    stl_learn::part1_sort();
    stl_learn::part2_partial();
    stl_learn::part3_binary_search();
    stl_learn::part4_merge();
    stl_learn::part5_heap();
    stl_learn::part6_partition();
    stl_learn::part_pitfalls();
    stl_learn::part_exercises();
}

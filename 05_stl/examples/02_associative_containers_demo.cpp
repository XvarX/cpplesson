// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  示例: 02_associative_containers_demo — 关联容器                            ║
// ║  主题: set / map / multiset / multimap                                      ║
// ║  演示如何调用 stl_lib 中的教学函数来学习关联容器的特性和用法                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/associative_containers.hpp"

int main() {
    lesson::print_header("02_associative_containers — 关联容器");

    stl_learn::part1_set();
    stl_learn::part2_map();
    stl_learn::part3_multi();
    stl_learn::part4_comparator();
    stl_learn::part5_bound();
    stl_learn::part_pitfalls();
    stl_learn::part_exercises();
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  示例: 03_unordered_containers_demo — 无序关联容器                          ║
// ║  主题: unordered_set / unordered_map / bucket / 自定义哈希                   ║
// ║  演示如何调用 stl_lib 中的教学函数来学习无序容器的特性和用法                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/unordered_containers.hpp"

int main() {
    lesson::print_header("03_unordered_containers — 无序关联容器");

    stl_learn::part1_unordered_set();
    stl_learn::part2_unordered_map();
    stl_learn::part3_bucket();
    stl_learn::part4_custom_hash();
    stl_learn::part5_comparison();
    stl_learn::part_pitfalls();
    stl_learn::part_exercises();
}

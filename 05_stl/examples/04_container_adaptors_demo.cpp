// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  示例: 04_container_adaptors_demo — 容器适配器                               ║
// ║  主题: stack / queue / priority_queue                                        ║
// ║  演示如何调用 stl_lib 中的教学函数来学习容器适配器的特性和用法                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/container_adaptors.hpp"

int main() {
    lesson::print_header("04_container_adaptors — 容器适配器");

    stl_learn::part1_stack();
    stl_learn::part2_queue();
    stl_learn::part3_priority_queue();
    stl_learn::part4_custom_pq();
    stl_learn::part5_summary();
    stl_learn::part_pitfalls();
    stl_learn::part_exercises();
}

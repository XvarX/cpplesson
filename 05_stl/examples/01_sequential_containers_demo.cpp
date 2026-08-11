// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  示例: 01_sequential_containers_demo — 顺序容器                             ║
// ║  主题: vector / deque / list / forward_list / array                         ║
// ║  演示如何调用 stl_lib 中的教学函数来学习各顺序容器的特性和用法                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/sequential_containers.hpp"

int main() {
    lesson::print_header("01_sequential_containers — 顺序容器");

    stl_learn::part1_vector();
    stl_learn::part2_deque();
    stl_learn::part3_list();
    stl_learn::part4_forward_list();
    stl_learn::part5_array();
    stl_learn::part6_choose_container();
    stl_learn::part_pitfalls();
    stl_learn::part_exercises();
}

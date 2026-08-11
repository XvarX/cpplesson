// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  示例: 09_tuple_pair_optional_variant_demo — 多值与可选类型                  ║
// ║  主题: pair / tuple / optional / variant / any / visit / 结构化绑定          ║
// ║  演示如何调用 stl_lib 中的教学函数来学习多值与可选类型的用法                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/tuple_types.hpp"

int main() {
    lesson::print_header("09_tuple_pair_optional_variant — 多值与可选类型");

    stl_learn::part1_pair();
    stl_learn::part2_tuple();
    stl_learn::part3_optional();
    stl_learn::part4_variant();
    stl_learn::part5_any();
    stl_learn::part_pitfalls();
    stl_learn::part_exercises();
}

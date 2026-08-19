// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  示例: 05_iterators_demo — 迭代器                                           ║
// ║  主题: 迭代器类别、iterator_traits、自定义迭代器、ranges                      ║
// ║  演示如何调用 stl_lib 中的教学函数来学习迭代器的特性和用法                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/iterators.hpp"

int main() {
    lesson::print_header("05_iterators — 迭代器");

    stl_learn::part1_categories();
    stl_learn::part2_traits();
    stl_learn::part3_adaptors();
    stl_learn::part4_custom_iterator();
    stl_learn::part5_ranges();
    stl_learn::iterators_pitfalls();
    stl_learn::iterators_exercises();
}

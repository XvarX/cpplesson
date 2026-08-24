#include <print>
#include <string>
#include <memory>
#include <cstddef>
#include <functional>
#include "shared/lesson_utils.hpp"
#include "fundamentals/pointers_refs.hpp"

int main() {
    lesson::print_header("04 指针、引用、nullptr、函数指针、void*");

    lesson::print_subtitle("Part 1: 指针基础");

    int value = 42;
    int* ptr = &value;

    int** ptr_to_ptr = &ptr;

    std::println("value         = {}", value);
}
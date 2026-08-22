#include <print>
#include <string>
#include <array>
#include <cstring>
#include <cstddef>
#include <algorithm>
#include "shared/lesson_utils.hpp"
#include "fundamentals/arrays_strings.hpp"

int main() {
    lesson::print_header("03 C风格数组、字符串、std::array 于指针算术");

    lesson::print_subtitle("Part 1: C 风格数组");

    int scores[5] = {95, 82, 78, 91, 88};
    int zeros[5]{};
    int inferred[] = {1,2,3,4,5};

    constexpr std::size_t inferred_size = sizeof(inferred)/sizeof(inferred[0]);
    std::println("scores: {}, {}, {}, {}, {}", scores[0], scores[1], scores[2], scores[3], scores[4]);
    std::println("zeros: {}, {}, {}, {}, {}", zeros[0], zeros[1], zeros[2], zeros[3], zeros[4]);
    std::println("inferred: 大小 = {} (推导出5)", inferred_size);

    std::println("安全访问 scores[0] = {}", scores[0]);
    std::println("库函数 array_size(scores) = {}", fundamentals::array_size(scores));

    lesson::print_subtitle("Part 2: C 字符串");

    char cstr1[] = "Hello";
    const char* cstr2 = "World";

    std::println("cstr1 = \"{}\", strlen = {}", cstr1, std::strlen(cstr1));
    std::println("cstr2 = \"{}\", strlen = {}", cstr2, std::strlen(cstr2));

    char buffer[32]{};
    std::strcpy(buffer, cstr1);
    std::strcat(buffer, " ");
    std::strcat(buffer, cstr2);
    std::println("strcpy + strcat 结果： \"{}\"", buffer);

    int cmp = std::strcmp(cstr1, cstr2);
    std::println("strcmp(\"Hello\", \"World\") = {}", cmp);

    lesson::print_subtitle("Part 3: std::array(c++11)");
    std::array<int, 5> arr = {10, 20, 30, 40, 50};

    std::println("std::array: {}", arr);
    std::println("arr.size() = {}", arr.size());
    std::println("arr.front() = {}， arr.back() = {}", arr.front(), arr.back());

    std::println("arr[2] = {} (不检查边界，无开销)", arr[2]);
    std::println("arr.at(2) = {} (检查边界，越界会抛异常)", arr.at(2));

    std::print("range-for 遍历: ");

    for (int v : arr) {
        std::print("{} ", v);
    }
    std::println("");

    lesson::print_subtitle("Part 4: 指针算术");

    int numbers[] = {100, 200, 300, 400, 500};
    int * p = numbers;

    std::println("numbers = {}", static_cast<const void*>(numbers));
    std::println("p = {}", static_cast<const void*>(p));
    std::println("*p = {}(首元素)", *p);
    std::println("*(p+1) = {}(第二个元素，不是+1字节！)", *(p+1));
    std::println("*(p+3) = {}(第四个元素)", *(p+3));

    std::println("p[2] = *(p+2) = {}", p[2]);

    std::print("指针遍历:");
    for(int* it = numbers; it != numbers + 5; ++it) {
        std::print("{} ", *it);
    }
    std::println("");

    int* start = numbers;
    int* end = numbers + 5;
    std::ptrdiff_t dist = end - start;
    std::println("end - start = {} 个元素", dist);

    lesson::print_subtitle("Part 5: 数据退化演示");

    int local_arr[10] = {};
    std::println("sizeof(local_arr) = {} (在作用域内，sizeof 知道完整大小)", sizeof(local_arr));

    int* decayed_ptr = local_arr;
    std::println("sizeof(decayed_ptr) = {}(退化后,sizeof 只知道指针大小)", sizeof(decayed_ptr));

    lesson::print_subtitle("Part 6: 多维数组");

    int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };

    std::println("matrix[0][1] = {}", matrix[0][1]);
    std::println("matrix[1][2] = {}", matrix[1][2]);

    std::array<std::array<int, 3>, 2> mat2 = {{{1,2,3}, {4,5,6}}};
    std::println("std::array 二维: {} {} {}", mat2[0][0], mat2[0][1], mat2[0][2]);
    std::println("                 {} {} {}", mat2[1][0], mat2[1][1], mat2[1][2]);

    int array[] = {1, 2, 3, 4, 5, 6};
    fundamentals::reverse1(array, 6);

    std::println("array reverse, {}", array[0]);

}
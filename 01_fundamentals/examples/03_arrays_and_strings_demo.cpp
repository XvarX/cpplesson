// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  01_fundamentals 示例 — C风格数组、C字符串、std::array、指针算术、数组退化   ║
// ║  目标: 理解数组的底层模型、字符串表示方式、以及指针与数组的紧密关系          ║
// ║  前置: 01_types_and_literals, 02_const_and_static                            ║
// ║  对应库头文件: fundamentals/arrays_strings.hpp                               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <array>        // std::array (C++11)
#include <cstring>      // strlen, strcpy, strcmp
#include <cstddef>      // std::size_t
#include <algorithm>    // std::sort, std::for_each
#include "shared/lesson_utils.hpp"
#include "fundamentals/arrays_strings.hpp"

int main() {
    lesson::print_header("03 C风格数组、字符串、std::array 与指针算术");

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 1: C 风格数组 — 固定大小的连续内存块
    // ═══════════════════════════════════════════════════════════════════════════════
    // C 风格数组是什么: 栈上分配的连续内存块，大小编译期确定 (C99 VLA 除外)。
    // 为什么了解它: 1) 它是 C++ 数组模型的基石  2) 遗留代码大量使用
    //               3) 理解它才能理解 std::array 为什么更好
    // 什么时候用 C 风格数组: 几乎不用! 优先用 std::array 或 std::vector。
    // 但理解它是看懂底层代码的前提。
    lesson::print_subtitle("Part 1: C 风格数组");

    // ── 声明与初始化 ──
    int scores[5] = {95, 82, 78, 91, 88};    // 传统写法
    int zeros[5]{};                            // C++ 风格: 全部初始化为 0
    int inferred[] = {1, 2, 3, 4, 5};         // 让编译器推导大小 (5)
    // 数组大小 = sizeof(整个数组) / sizeof(单个元素)
    constexpr std::size_t inferred_size = sizeof(inferred) / sizeof(inferred[0]);

    std::println("scores:   {}, {}, {}, {}, {}", scores[0], scores[1], scores[2], scores[3], scores[4]);
    std::println("zeros:    {}, {}, {}, {}, {}", zeros[0], zeros[1], zeros[2], zeros[3], zeros[4]);
    std::println("inferred: 大小 = {} (推导出 5)", inferred_size);

    // ── 越界访问: C++ 不检查 ──
    // scores[999] = 0;  // ⚠️ 未定义行为! 编译器不报错，运行时可能崩溃或静默破坏内存
    std::println("安全访问 scores[0] = {}", scores[0]);

    // 使用库中的 array_size 函数 (定义在 arrays_strings.hpp)
    std::println("库函数 array_size(scores) = {}", fundamentals::array_size(scores));

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 2: C 字符串 — 以 '\0' 结尾的字符数组
    // ═══════════════════════════════════════════════════════════════════════════════
    // C 字符串是什么: char 数组，最后一个字符必须是 '\0' (null terminator)。
    // 为什么了解它: 大量 C API 使用 C 字符串 (操作系统、网络库、嵌入式)。
    // 注意: C 字符串不是一个类型，只是一个约定 (以 0 结尾)。
    lesson::print_subtitle("Part 2: C 字符串");

    // ── 声明方式 ──
    char cstr1[] = "Hello";         // 自动加 '\0'，数组大小 = 6 (5 个字母 + '\0')
    const char* cstr2 = "World";    // 指向字符串字面量的指针 (字面量存储在只读区)
    // cstr2[0] = 'w';              // ❌ 未定义行为! 字符串字面量是只读的

    // strlen: 返回 '\0' 之前的字符数 (不包含 '\0')
    std::println("cstr1 = \"{}\", strlen = {}", cstr1, std::strlen(cstr1));
    std::println("cstr2 = \"{}\", strlen = {}", cstr2, std::strlen(cstr2));

    // ── C 字符串操作 (需要 <cstring>) ──
    char buffer[32]{};
    std::strcpy(buffer, cstr1);               // 拷贝 (不安全! 不检查缓冲区大小)
    std::strcat(buffer, " ");                 // 拼接 (同样不安全)
    std::strcat(buffer, cstr2);               // buffer 现在 = "Hello World"
    std::println("strcpy + strcat 结果: \"{}\"", buffer);

    int cmp = std::strcmp(cstr1, cstr2);      // 比较: 负数 = cstr1 < cstr2
    std::println("strcmp(\"Hello\", \"World\") = {}", cmp);  // 期望: 负数 (H < W)

    // ── C 字符串的危险 ──
    // strcpy 不检查目标缓冲区大小 → 缓冲区溢出 → 安全漏洞!
    // 现代替代: strncpy, snprintf; 更好的选择: std::string

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 3: std::array — 零开销的现代数组包装 (C++11)
    // ═══════════════════════════════════════════════════════════════════════════════
    // std::array 是什么: 对 C 风格数组的零开销包装，大小是模板参数。
    // 为什么存在: 保持 C 风格数组的性能 + 提供 STL 容器的接口 (size, at, 迭代器)。
    // 什么时候用: 当你知道编译期大小且不需要动态扩容时 (替代 C 风格数组)。
    // 关键优势: .size() 不依赖 sizeof 技巧；.at() 有边界检查；可作为函数参数/返回值。
    lesson::print_subtitle("Part 3: std::array (C++11)");

    std::array<int, 5> arr = {10, 20, 30, 40, 50};
    // C++23 可以直接打印整个容器!
    std::println("std::array: {}", arr);

    std::println("arr.size() = {}", arr.size());
    std::println("arr.front() = {}, arr.back() = {}", arr.front(), arr.back());

    // ── .at() vs [] ──
    std::println("arr[2]   = {} (不检查边界, 无开销)", arr[2]);
    std::println("arr.at(2) = {} (检查边界, 越界会抛异常)", arr.at(2));
    // arr.at(999);  // ← 抛出 std::out_of_range，比 undefined behavior 好得多

    // ── 迭代遍历 ──
    std::print("range-for 遍历: ");
    for (int v : arr) {       // range-for 隐式使用 begin()/end()
        std::print("{} ", v);
    }
    std::println("");

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 4: 指针算术 — 指针是数组操作的底层机制
    // ═══════════════════════════════════════════════════════════════════════════════
    // 数组名可以被隐式转换为指向首元素的指针，这是理解 C/C++ 数组的关键。
    // 指针算术: ptr + n 不是加 n 个字节，而是加 n 个元素!
    //   ptr + 1 → 地址增加 sizeof(*ptr) 个字节
    lesson::print_subtitle("Part 4: 指针算术");

    int numbers[] = {100, 200, 300, 400, 500};
    int* p = numbers;  // 等价于 int* p = &numbers[0]; (数组退化为指针)

    std::println("numbers   = {}", static_cast<const void*>(numbers));
    std::println("p         = {}", static_cast<const void*>(p));
    std::println("*p        = {} (首元素)", *p);
    std::println("*(p + 1)  = {} (第二个元素, 不是 +1 字节!)", *(p + 1));
    std::println("*(p + 3)  = {} (第四个元素)", *(p + 3));

    // p[i] 是 *(p + i) 的语法糖 — 完全等价!
    std::println("p[2] = *(p+2) = {}", p[2]);

    // ── 指针遍历数组 ──
    std::print("指针遍历: ");
    for (int* it = numbers; it != numbers + 5; ++it) {
        std::print("{} ", *it);
    }
    std::println("");

    // ── 指针的差: 两个同类型指针相减得到元素个数 (ptrdiff_t) ──
    int* start = numbers;
    int* end   = numbers + 5;
    std::ptrdiff_t dist = end - start;   // = 5, 不是 5*sizeof(int)
    std::println("end - start = {} 个元素", dist);

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 5: 数组退化为指针 — 最常见也最坑的特性
    // ═══════════════════════════════════════════════════════════════════════════════
    // 当数组作为函数参数传递时，它会退化为指针 (失去大小信息)。
    // 这就是为什么 C 函数总需要额外传一个 size 参数。
    // 例外: 数组作为引用的参数 (int(&arr)[N]) 不会退化。
    lesson::print_subtitle("Part 5: 数组退化演示");

    // ── 原地: sizeof 能知道数组大小 ──
    int local_arr[10] = {};
    std::println("sizeof(local_arr) = {} (在作用域内，sizeof 知道完整大小)", sizeof(local_arr));

    // ── 传给指针后: sizeof 只能得到指针大小 ──
    int* decayed_ptr = local_arr;   // 隐式退化!
    std::println("sizeof(decayed_ptr) = {} (退化后，sizeof 只知道指针大小)", sizeof(decayed_ptr));

    // ── 正确做法: 用 std::array 或 std::span (C++20) 传递数组 ──
    // void process(std::span<int> data);  // 安全! 携带大小信息

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 6: 多维数组
    // ═══════════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 6: 多维数组");

    // C 风格多维数组: 本质是"数组的数组"，内存是连续的
    int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };  // 内存布局: [1][2][3][4][5][6] — 行优先 (row-major)

    std::println("matrix[0][1] = {}", matrix[0][1]);  // 2
    std::println("matrix[1][2] = {}", matrix[1][2]);  // 6

    // std::array 的嵌套版本
    std::array<std::array<int, 3>, 2> mat2 = {{{1, 2, 3}, {4, 5, 6}}};
    std::println("std::array 二维: {} {} {}", mat2[0][0], mat2[0][1], mat2[0][2]);
    std::println("               {} {} {}", mat2[1][0], mat2[1][1], mat2[1][2]);

    // ── 常见陷阱 (Common Pitfalls) ────────────────────────────────────────────
    lesson::print_separator("常见陷阱");
    lesson::print_warn("陷阱1: C 风格数组越界 — 编译器不报错, 可能造成安全漏洞");
    lesson::print_warn("陷阱2: 数组退化为指针 — 传给函数后丢失大小信息");
    lesson::print_warn("陷阱3: sizeof(指针) != sizeof(数组) — 退化后 sizeof 得到的是指针大小");
    lesson::print_warn("陷阱4: C 字符串无越界保护 — strcpy 溢出是经典漏洞, 用 std::string");
    lesson::print_warn("陷阱5: 字符串字面量不可修改 — const char* p = \"hello\"; p[0] = 'H' 是 UB");
    lesson::print_warn("陷阱6: 多维数组参数 int a[][3] — 除第一维外都要指定大小或退化为指针");

    // ── 练习 ─────────────────────────────────────────────────────────────────
    lesson::print_separator("练习");
    std::println("1. 用 C 风格数组实现一个 reverse 函数 (原地反转 int 数组)");
    std::println("2. 手工实现 strlen (用指针算术, 不调用标准库)");
    std::println("3. 用 std::array 重写上面的 reverse, 用 .at() 保证安全");
    std::println("4. 实验: 把 C 风格数组传给函数, 在函数内用 sizeof, 观察退化效果");
    std::println("5. 写一个 3×3 矩阵乘法, 分别用 C 风格数组和 std::array 实现");

    return 0;
}

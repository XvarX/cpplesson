// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  04_templates — 02 类模板 演示                                               ║
// ║  学习目标:                                                                    ║
// ║    1. 理解类模板的定义和实例化方式                                            ║
// ║    2. 掌握类外成员函数定义的语法                                              ║
// ║    3. 学会使用非类型模板参数(NTTP)和 auto NTTP (C++17)                       ║
// ║    4. 理解默认模板参数的使用场景                                              ║
// ║  前置: 01_function_template, 02_classes_objects                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "templates/class_template.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>

int main() {
    lesson::print_header("04_templates — 02 类模板");

    // ── Part 1: 基本类模板 ──
    lesson::print_subtitle("Part 1: 基本类模板 Box<T>");
    {
        Box<int> intBox(42);
        std::println("{}", intBox.describe());
        intBox.set(100);
        std::println("修改后: {}", intBox.describe());

        Box<std::string> strBox("你好，模板！");
        std::println("{}", strBox.describe());
    }

    // ── Part 2: 类外成员函数定义 ──
    lesson::print_subtitle("Part 2: 类外成员函数定义");
    {
        Calculator<double> calc(3.5, 2.0);
        std::println("add(3.5, 2.0)      = {}", calc.add());
        std::println("multiply(3.5, 2.0) = {}", calc.multiply());
    }

    // ── Part 3: 非类型模板参数 ──
    lesson::print_subtitle("Part 3: 非类型模板参数 NTTP");
    {
        FixedStack<int, 5> stack;  // 最大 5 个元素的栈
        stack.push(10);
        stack.push(20);
        stack.push(30);
        std::println("栈容量: {}, 当前大小: {}", stack.capacity(), stack.size());
        std::println("弹出: {}", stack.pop());
        std::println("弹出: {}", stack.pop());
        std::println("剩余: {}", stack.size());
    }

    // ── Part 4: auto NTTP (C++17) ──
    lesson::print_subtitle("Part 4: auto NTTP (C++17)");
    {
        // 42 是 int，true 是 bool
        std::println("Constant<42>.value   = {}", Constant<42>::value);
        std::println("Constant<true>.value = {}", Constant<true>::value);
    }

    // ── Part 5: 默认模板参数 ──
    lesson::print_subtitle("Part 5: 默认模板参数");
    {
        Buffer<> buf1;               // 使用默认值 T=int, Size=10
        Buffer<double, 5> buf2;      // 显式指定
        std::println("buf1 容量: {}, buf2 容量: {}", buf1.capacity(), buf2.capacity());
    }

    lesson::print_separator("常见陷阱速查");
    std::println("1. 类外定义成员函数时必须重复 template<...> 声明");
    std::println("2. 模板类的声明和定义必须在同一个翻译单元(头文件)");
    std::println("3. NTTP 必须是编译期常量, 运行时变量不能作为模板参数");
    std::println("4. 每个不同的模板参数组合都会生成全新的类, 注意代码膨胀");

    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 写一个 Pair<T, U> 类模板，存储两个值 first 和 second。
//    实现 print() 方法打印两个值（用 std::println 格式化）。
//    测试 Pair<int, std::string> 和 Pair<double, char>。
// 2. 写一个 FixedArray<T, int N> 模板（NTTP），实现 size() 和 at(int i) 方法。
//    测试读取和越界访问行为。
// 3. 给 Pair 模板添加默认参数：Pair<T, U = int>。
//    观察 Pair<double> 的 second 是什么类型。

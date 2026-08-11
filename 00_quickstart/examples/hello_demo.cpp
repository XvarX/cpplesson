// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  00 — Hello World + 编译器验证 (工程版)                                     ║
// ║  目标: 确认编译器支持 C++23、CMake 能正常构建、程序能运行                    ║
// ║  前置: 安装 GCC 14+/Clang 18+/MSVC 2022+，CMake 3.20+                       ║
// ║                                                                            ║
// ║  练习: 把这段代码照着敲一遍！                                                ║
// ║  1. 在 mywork/ 目录新建 hello.cpp，照着敲代码                               ║
// ║  2. 修改 std::println 里的字符串，加入你自己的名字                          ║
// ║  3. 运行: cd build && cmake .. && cmake --build . && ./00_quickstart/examples/hello_demo
// ║  4. 如果不通过编译，检查编译器版本是否 >= GCC 14 / Clang 18                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "quickstart/compiler_info.hpp"   // 编译器信息检测
#include "shared/lesson_utils.hpp"         // 学习辅助工具 (print_header 等)

#include <iostream>
#include <print>     // C++23 std::print 头文件 (GCC 14+, Clang 18+)
#include <vector>
#include <string>

int main() {
    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: 传统方式 — std::cout
    // ═══════════════════════════════════════════════════════════════════════════
    // std::cout 是 C++ 最早的标准输出流，来自 <iostream>
    // << 是"流插入运算符"，把数据送到输出流
    // std::endl 是换行 + 刷新缓冲区 (也可以用 '\n')
    lesson::print_subtitle("Part 1: std::cout — 传统输出方式");
    std::cout << "Hello, C++!" << std::endl;
    std::cout << "编译器版本宏: C++" << __cplusplus << std::endl;

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: C++23 新方式 — std::print
    // ═══════════════════════════════════════════════════════════════════════════
    // std::println 是 C++23 新增的格式化打印函数 (类似 Python 的 print)
    // 比 cout 简洁，不用写 << 和 std::endl
    // 内部使用 std::format，性能更好，线程安全
    lesson::print_subtitle("Part 2: std::println — C++23 格式化输出");
    std::println("Hello from C++23 std::println!");          // 自动换行
    std::print("std::print 不会自动换行，");                  // 不换行
    std::print("所以这句话接在后面\n");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: 现代 C++ 容器快速体验
    // ═══════════════════════════════════════════════════════════════════════════
    // vector 是动态数组，最常用的容器
    // C++23 的 std::println 可以直接打印容器内容
    lesson::print_subtitle("Part 3: std::vector — 动态数组快速体验");
    std::vector<int> numbers = {3, 1, 4, 1, 5, 9, 2, 6};

    // 用 std::println 直接打印容器 (C++23 新特性)
    // {} 是占位符，类似 Python 的 f-string
    std::println("数组内容: {}", numbers);

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: 验证编译器信息 (使用 quickstart 库)
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 4: 编译器信息");
    // 调用 quickstart 库中的函数 — 代码复用！
    // print_compiler_info() 实现在 src/compiler_info.cpp 中
    lesson::quickstart::print_compiler_info();

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: 基础 CMake 概念 (注释教学)
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("Part 5: CMake 概念速览");
    lesson::print_separator("CMake 基础概念");

    // CMake 是 C++ 最主流的构建系统。你需要知道的几个概念：
    //
    //   CMakeLists.txt  — 构建配置文件 (每个目录一个)
    //   target          — 构建目标 (可执行文件 或 库)
    //   cmake -B build  — 在 build/ 目录生成构建文件 (只需一次)
    //   cmake --build build — 执行编译
    //
    // 本项目采用"工程化布局"：
    //   include/quickstart/  — 头文件 (声明)
    //   src/                — 实现文件 (定义)
    //   examples/           — 演示程序 (包含 main，这是你照着敲的)
    //
    // 这种布局模拟了真实项目的结构：
    //   - 库代码和演示代码分离
    //   - 头文件保护 (#pragma once)
    //   - CMake 管理编译依赖
    //
    // 打开 00_quickstart/CMakeLists.txt 看看具体是怎么写的！
    // ----------------------------------------------------------------

    lesson::print_note("恭喜！你的 C++23 环境配置正确。可以继续学习了。");
    return 0;
}

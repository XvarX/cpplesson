// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  13_build_and_tooling — examples/preprocessor_demo.cpp                         ║
// ║  学习目标:                                                                    ║
// ║    1. 掌握 #define/#undef 宏定义，理解宏的文本替换本质                          ║
// ║    2. 掌握条件编译 #ifdef/#ifndef/#else/#endif 及其常见用途                     ║
// ║    3. 理解 #pragma once 与 include guard 的区别和各自适用场景                   ║
// ║    4. 使用 __has_include (C++17) 做编译期能力检测                              ║
// ║    5. 了解 #embed (C++23) — 将外部文件内容嵌入为编译期数据                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <iostream>
#include <print>
#include <string>
#include <vector>
#include <array>
#include <cassert>
#include <string_view>

#include "shared/lesson_utils.hpp"
#include "build_tooling/macro_utils.hpp"
#include "build_tooling/platform_detect.hpp"

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: #define / #undef — 宏的文本替换本质
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是宏 (#define)?
//   预处理器在编译之前先扫描源码，把宏名"文本替换"成宏体。
//   宏不是 C++ 语言的一部分，预处理器不懂 C++ 语法，只是在做字符串替换。
//
// 为什么还需要宏 (在 constexpr/consteval 时代)?
//   - 条件编译 (运行时做不到)
//   - 字符串化 (# 运算符)
//   - 标记拼接 (## 运算符)
//   - 与外部工具配合 (编译标志 -DVERSION="1.0")
//   - __FILE__, __LINE__, __func__ 等调试信息
//
// 何时用宏 vs constexpr?
//   - 能用 constexpr/consteval 时优先用 (类型安全，有作用域)
//   - 需要条件编译或字符串化时用宏
//
// 注意: PI, APP_NAME, BUFFER_SIZE, SQUARE, MAX, STRINGIFY, CONCAT, MAKE_VAR
//       等宏定义在 include/build_tooling/macro_utils.hpp 中

// #undef : 取消宏定义 — 演示用
#define TEMP_VALUE 100

void demo_define_undef() {
    lesson::print_subtitle("Part 1: #define / #undef 文本替换");

    // 基本使用 (宏来自 macro_utils.hpp)
    std::println("  PI = {}", PI);
    std::println("  APP_NAME = {}", APP_NAME);

    // 函数式宏
    std::println("  SQUARE(5) = {}", SQUARE(5));
    // 展开为: ((5) * (5)) = 25
    std::println("  SQUARE(2+3) = {}", SQUARE(2+3));
    // 展开为: ((2+3) * (2+3)) = 25  ← 如果不加括号: 2+3*2+3 = 11!
    std::println("  MAX(10, 20) = {}", MAX(10, 20));

    // 字符串化
    std::println("  STRINGIFY(hello) = \"{}\"", STRINGIFY(hello));

    // 标记拼接 — 生成新标识符
    int MAKE_VAR(my_var_, 1) = 42;  // 展开为 int my_var_1 = 42;
    std::println("  my_var_1 (由 MAKE_VAR 生成) = {}", my_var_1);

    // #undef 取消定义
    std::println("  TEMP_VALUE 存在: {}", TEMP_VALUE);
    #undef TEMP_VALUE
    // std::println("{}", TEMP_VALUE);  // 编译错误: TEMP_VALUE 未定义
    std::println("  TEMP_VALUE 已通过 #undef 取消定义");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 条件编译 — #ifdef / #ifndef / #else / #endif
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是条件编译?
//   根据宏是否定义，选择性保留或删除代码块。
//   被排除的代码不会出现在最终编译单元中 (不是注释掉，是彻底不存在)。
//
// 为什么需要条件编译?
//   - 跨平台: 不同操作系统用不同 API
//   - Debug/Release: 调试模式下启用额外的检查
//   - 功能开关: 编译时选择特性集 (如 ENABLE_GPU=ON)
//   - 编译器适配: 不同编译器有不同的扩展语法
//
// #if vs #ifdef:
//   #ifdef MACRO    — MACRO 是否定义了 (不管值是什么)
//   #if MACRO       — MACRO 的值是否为真 (0 为假，非 0 为真)
//   #ifndef MACRO   — MACRO 是否未定义
//   defined(MACRO)  — 另一种写法，可用于 && / ||
//
// 注意: PLATFORM, IS_DEBUG, ENABLE_LOGGING 定义在 platform_detect.hpp 中

void demo_conditional_compilation() {
    lesson::print_subtitle("Part 2: 条件编译 #ifdef / #ifndef / #else / #endif");

    std::println("  当前平台: {}", PLATFORM);
    std::println("  调试模式: {}", IS_DEBUG ? "是 (Debug)" : "否 (Release)");
    std::println("  日志功能: {}", ENABLE_LOGGING ? "开启" : "关闭");

    // 条件编译控制功能
    #if ENABLE_LOGGING
        std::println("  [LOG] 这条消息只在 ENABLE_LOGGING=1 时编译");
    #else
        std::println("  [LOG] 日志功能已禁用 (编译时 -DENABLE_LOGGING=1 可开启)");
    #endif

    // 编译器特定代码
    #if defined(__GNUC__) && !defined(__clang__)
        std::println("  使用 GCC 特定优化路径");
    #elif defined(__clang__)
        std::println("  使用 Clang 特定优化路径");
    #elif defined(_MSC_VER)
        std::println("  使用 MSVC 特定优化路径");
    #endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: #pragma once vs Include Guard
// ═══════════════════════════════════════════════════════════════════════════════
// 问题: 如果 a.h 包含了 b.h，且 main.cpp 也包含了 b.h，b.h 的内容会出现两次
//        → 重复定义错误 (redefinition)
//
// 解决方案 1 — Include Guard (传统，标准):
//   #ifndef MY_HEADER_H
//   #define MY_HEADER_H
//   // ... 头文件内容 ...
//   #endif
//
// 解决方案 2 — #pragma once (现代，非标准但所有编译器都支持):
//   #pragma once
//   // ... 头文件内容 ...
//
// 对比:
//   | 特性         | Include Guard               | #pragma once         |
//   |--------------|----------------------------|----------------------|
//   | 标准化       | C++ 标准                    | 非标准 (事实标准)     |
//   | 唯一性       | 依赖宏名 (可能冲突)         | 靠文件路径判断        |
//   | 硬链接/符号链接 | 可能失效                  | 编译器各自处理        |
//   | 可读性       | 3 行样板代码                | 1 行                 |
//   | 移植性       | 100%                        | 所有主流编译器支持    |
//
// 最佳实践:
//   - 新项目直接用 #pragma once (简洁、不会命名冲突)
//   - 需要极致移植性时用 include guard (极少数场景)

void demo_include_guard() {
    lesson::print_subtitle("Part 3: #pragma once vs Include Guard");

    std::println("  Include Guard 写法:");
    std::println("    #ifndef MYLIB_MATH_UTILS_HPP");
    std::println("    #define MYLIB_MATH_UTILS_HPP");
    std::println("    // 头文件内容...");
    std::println("    #endif  // MYLIB_MATH_UTILS_HPP");
    std::println("");
    std::println("  #pragma once 写法 (推荐):");
    std::println("    #pragma once");
    std::println("    // 头文件内容...");
    std::println("");
    std::println("  头文件保护原理:");
    std::println("    第一次包含 → 宏未定义 → 定义宏, 展开内容");
    std::println("    第二次包含 → 宏已定义 → 跳过全部内容");
    std::println("  ✓ 本项目使用 #pragma once (简洁、不会宏名冲突)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: __has_include (C++17) — 编译期检测头文件是否存在
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是 __has_include?
//   预处理运算符，在编译期检测某个头文件是否存在。
//   返回 1 (存在) 或 0 (不存在)，可与 #if 配合使用。
//
// 为什么需要?
//   - 可选依赖: 有某个库就用，没有就回退到替代方案
//   - 跨平台: 不同平台有不同的系统头文件
//   - 编译器特性: 新标准库头文件可能还不存在
//
// 注意: HAS_OPTIONAL, HAS_VERSION_HEADER, HAS_UNISTD 定义在 platform_detect.hpp 中
//       platform_detect.hpp 中通过 __has_include 完成了头文件检测

void demo_has_include() {
    lesson::print_subtitle("Part 4: __has_include (C++17) 编译期头文件检测");

    std::println("  <optional> 头文件: {}", HAS_OPTIONAL ? "可用 ✓" : "不可用 ✗");
    std::println("  <version> 头文件:  {}", HAS_VERSION_HEADER ? "可用 ✓" : "不可用 ✗");
    std::println("  <unistd.h> (POSIX): {}", HAS_UNISTD ? "存在 ✓" : "不存在 ✗ (Windows)");

#if HAS_OPTIONAL
    std::optional<int> opt = 42;
    std::println("  std::optional 示例: {}", opt.value());
#endif

    std::println("");
    std::println("  实际用途:");
    std::println("    1. 可选依赖库: #if __has_include(<fmt/core.h>) → 用 fmt 库");
    std::println("    2. 平台适配:   #if __has_include(<windows.h>) → Windows 路径");
    std::println("    3. 编译器升级: #if __has_include(<expected>) → C++23 新特性");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: #embed (C++23) — 将文件内容嵌入为编译期数据
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是 #embed?
//   C++23 新增的预处理指令，将外部文件 (二进制或文本) 的内容嵌入为
//   编译期可用数据 (std::byte 数组)。
//
// 为什么需要 #embed?
//   传统做法:
//     - xxd -i 把文件转成 C 数组 → 额外构建步骤, 版本管理麻烦
//     - 运行时读取文件 → 路径问题, 性能开销
//     - std::embed (被推迟) → #embed 是更底层的替代方案
//   #embed 优势:
//     - 编译器内置, 无外部工具依赖
//     - 编译期完成, 零运行时开销
//     - 类型安全 (返回 std::byte 序列)
//
// 语法 (C++23):
//   constexpr std::array<std::byte, N> data = {
//       #embed "file.bin"
//   };
//
// 使用场景:
//   - 嵌入着色器源码 (OpenGL/Vulkan)
//   - 嵌入配置文件默认值
//   - 嵌入静态网页资源 (小型 HTTP 服务器)
//   - 嵌入机器学习模型权重

void demo_embed() {
    lesson::print_subtitle("Part 5: #embed (C++23) 编译期文件嵌入");

    std::println("  #embed 是 C++23 新增的预处理指令");
    std::println("  用法: constexpr std::array<std::byte, N> data = {{ #embed \"file.bin\" }};");
    std::println("");
    std::println("  典型场景: 嵌入着色器源码");
    std::println("    constexpr std::string_view vertex_shader = {{");
    std::println("        #embed \"shaders/triangle.vert\"  // 编译时嵌入, 零运行时开销");
    std::println("    }};");
    std::println("");
    std::println("  #embed 支持参数:");
    std::println("    limit(N)   — 只嵌入前 N 个字节");
    std::println("    prefix()   — 在嵌入内容前插入数据");
    std::println("    suffix()   — 在嵌入内容后追加数据");
    std::println("    if_empty() — 文件为空或不存在时的回退内容");
    std::println("");
    std::println("  注意: 本文件不直接演示 #embed (需要外部文件)");
    std::println("        编译器需 GCC 15+ / Clang 19+ 才支持 #embed");

    // 模拟 #embed 的效果: 手动创建一个"嵌入"的数据
    // 真实代码中会是: constexpr auto data = { #embed "config.json" };
    constexpr std::array<char, 20> simulated_embed = {
        '{', '"', 'n', 'a', 'm', 'e', '"', ':', '"', 'd',
        'e', 'm', 'o', '"', '}', '\n', 0, 0, 0, 0
    };
    std::println("  模拟 #embed 结果: {}", std::string_view(simulated_embed.data(), 16));
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 宏参数不加括号导致优先级错误:
//    #define DOUBLE(x) x * 2     ← 危险!
//    DOUBLE(1+2) → 1+2*2 = 5 而不是 6
//    正确: #define DOUBLE(x) ((x) * 2)
//
// 2. 带副作用的宏参数被多次求值:
//    #define SQUARE(x) ((x)*(x))
//    int i = 1;
//    SQUARE(++i) → ((++i)*(++i)) → 2*3 = 6 (i 被加了两次!)
//
// 3. 头文件保护宏名冲突:
//    两个不同的库都用 #ifndef UTILS_H → 后包含的被跳过!
//    解决方案: 用 #pragma once 或在宏名中加入命名空间
//    #ifndef MYPROJECT_MYLIB_UTILS_H
//
// 4. #if 中未定义的宏默认为 0:
//    #if MISSING_MACRO    ← 不会报错, MISSING_MACRO 被当作 0
//    应该用 #ifdef / defined() 明确检查是否定义
//
// 5. 忘记 #embed 的 limit 参数导致二进制膨胀:
//    大文件用 #embed "huge.bin" limit(4096) 限制嵌入大小

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 写一个 DEBUG_LOG 宏: 在 Debug 模式下输出文件和行号，Release 下什么都不做
// 2. 写一个函数式宏 ABS(x)，正确处理所有类型和运算符优先级
// 3. 用 __has_include 检测你的编译器是否有 <ranges> 头文件
// 4. 创建两个头文件 A.hpp 和 B.hpp，A 包含 B，测试 include guard 的效果
//    (故意不加 guard 看编译错误，再加 guard 看修复)
// 5. 如果编译器支持 C++23 #embed，尝试嵌入一个小文本文件并打印

// ═══════════════════════════════════════════════════════════════════════════════

int main() {
    lesson::print_header("02 — 预处理器: #define / 条件编译 / include guard / #embed");

    demo_define_undef();
    demo_conditional_compilation();
    demo_include_guard();
    demo_has_include();
    demo_embed();

    lesson::print_separator("完成: 现在你对 C/C++ 预处理器有了全⾯理解");

    return 0;
}

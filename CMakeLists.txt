cmake_minimum_required(VERSION 3.20)

# ╔══════════════════════════════════════════════════════════════════════════════╗
# ║  C++ 学习模板 — 顶层 CMake (真实工程布局)                                  ║
# ║                                                                            ║
# ║  每个模块是一个独立库 (include/ + src/) + 演示程序 (examples/)             ║
# ║  你的练习代码放在 mywork/ 下                                               ║
# ║                                                                            ║
# ║  快速开始:                                                                 ║
# ║    cmake -B build -G "Ninja"        # 或 "Visual Studio 17 2022"          ║
# ║    cmake --build build                                                     ║
# ║    cmake --build build --target <name>   # 只编译某一个                    ║
# ║                                                                            ║
# ║  推荐编译器:                                                               ║
# ║    GCC 14+, Clang 18+, MSVC 2022 17.10+ (都需要 C++23 支持)               ║
# ╚══════════════════════════════════════════════════════════════════════════════╝

project(cpplesson
    VERSION 1.0.0
    DESCRIPTION "C++ 学习代码模板 — 从基础到 C++23 (真实工程布局)"
    LANGUAGES CXX
)

# ---- 全局 C++23 标准 ----
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- 编译选项 ----
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor)
endif()

# ---- GCC 16+ (MinGW): 链接 libstdc++exp 实验库 ----
# GCC 16 起，std::print 的 Windows 终端支持 (__open_terminal 等) 位于
# libstdc++exp.a，且默认不链接 — 缺了它所有用 std::println 的程序无法链接
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16)
    link_libraries(stdc++exp)
endif()

# ---- 所有目标输出到统一目录 (方便运行) ----
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---- 共享工具库 (header-only) ----
add_subdirectory(shared)

# ---- 各学习模块 (按学习顺序) ----
# 每个模块都是一个独立的 CMake 子项目:
#   include/<模块名>/  — 头文件 (接口)
#   src/              — 实现文件 (编译进静态库)
#   examples/         — 演示程序 (每个有 main(), 你照着敲的模板)
add_subdirectory(00_quickstart)
add_subdirectory(01_fundamentals)
add_subdirectory(02_classes)
add_subdirectory(03_memory)
add_subdirectory(04_templates)
add_subdirectory(05_stl)
add_subdirectory(06_streams_and_io)
add_subdirectory(07_modern_syntax)
add_subdirectory(08_move_and_lambda)
add_subdirectory(09_error_and_safety)
add_subdirectory(10_concurrency)
add_subdirectory(11_cpp20_cpp23)
add_subdirectory(12_patterns)
add_subdirectory(13_build_and_tooling)

# ---- 你的练习目录 ----
# 把照着敲的 .cpp 放这里
add_subdirectory(mywork)

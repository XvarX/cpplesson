# ------------------------------------------------------------------------------
# 11_cpp20_cpp23 — C++20/23 新特性
#
# 涵盖主题:
#   - std::bit_cast / std::byteswap / <bit> 位运算工具
#   - Concepts 进阶 (四种 requires 表达式、concept 组合、约束偏序)
#   - 协程基础 (co_await / co_yield / co_return、简易 generator<T>)
#   - Deducing This (C++23 显式对象参数、简化 CRTP、值类别感知)
#   - std::flat_map / std::mdspan / submdspan (C++23)
#   - Ranges 库 (views、管道操作符、ranges::to)
#   - std::span / std::source_location
#   - <=> 三路比较 (strong/weak/partial_ordering、=default)
#
# Library:  cpp20_23_lib
#   Sources: src/bit_utils.cpp, src/deducing_this.cpp, src/span_utils.cpp
#   Headers: include/cpp20_23/*.hpp (8 个头文件，含纯模板/纯头文件)
#   Links:   lesson_utils (PUBLIC — 所有模块共享的基础设施)
#
# Examples:
#   01_bit_cast_and_bytes_demo       → links cpp20_23_lib
#   02_concepts_deep_demo            → links cpp20_23_lib
#   03_coroutines_demo               → links cpp20_23_lib
#   04_deducing_this_demo            → links cpp20_23_lib
#   05_flat_and_mdspan_demo          → links cpp20_23_lib
#   06_ranges_demo                   → links cpp20_23_lib
#   07_span_and_source_location_demo → links cpp20_23_lib
#   08_three_way_comparison_demo     → links cpp20_23_lib
# ------------------------------------------------------------------------------

# ==============================================================================
# Library — cpp20_23_lib
# ==============================================================================
# 静态库，包含本模块所有带独立编译单元的 .cpp 实现文件。
# 纯模板和纯头文件 (concepts_deep, generator, ranges, three_way_types, flat_md)
# 不在此列表中——它们由 #include 直接展开，无需编译为 .o。
#
# PUBLIC include 目录使消费者能 #include <cpp20_23/xxx.hpp>。
# PUBLIC lesson_utils：本模块头文件未引用任何上一模块的类型，
# 仅依赖 lesson_utils 提供的跨模块共享基础设施（日志、断言等）。

add_library(cpp20_23_lib STATIC
    src/bit_utils.cpp
    src/deducing_this.cpp
    src/span_utils.cpp
)

target_include_directories(cpp20_23_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cpp20_23_lib PUBLIC
    lesson_utils
)

# ==============================================================================
# Examples — 每个 example 一个独立的 add_executable()，无循环/函数包装
# ==============================================================================
# 所有示例 PRIVATE 链接 cpp20_23_lib：示例是最终消费者，不对外暴露传递依赖。

# 01: std::bit_cast、std::byteswap、<bit> 位运算工具函数
add_executable(01_bit_cast_and_bytes_demo examples/bit_cast_and_bytes_demo.cpp)
target_link_libraries(01_bit_cast_and_bytes_demo PRIVATE cpp20_23_lib)

# 02: Concepts 进阶 — 四种 requires 表达式、concept 逻辑组合、约束偏序
add_executable(02_concepts_deep_demo examples/concepts_deep_demo.cpp)
target_link_libraries(02_concepts_deep_demo PRIVATE cpp20_23_lib)

# 03: 协程基础 — promise_type、generator<T>、co_await/co_yield/co_return
add_executable(03_coroutines_demo examples/coroutines_demo.cpp)
target_link_libraries(03_coroutines_demo PRIVATE cpp20_23_lib)

# 04: Deducing This (C++23) — 显式对象参数、简化 CRTP、值类别感知重载
add_executable(04_deducing_this_demo examples/deducing_this_demo.cpp)
target_link_libraries(04_deducing_this_demo PRIVATE cpp20_23_lib)

# 05: flat_map 与 mdspan (C++23) — 连续内存有序容器、多维视图与切片
add_executable(05_flat_and_mdspan_demo examples/flat_and_mdspan_demo.cpp)
target_link_libraries(05_flat_and_mdspan_demo PRIVATE cpp20_23_lib)

# 06: Ranges 库 — views 管道操作符、range adaptors、ranges::to
add_executable(06_ranges_demo examples/ranges_demo.cpp)
target_link_libraries(06_ranges_demo PRIVATE cpp20_23_lib)

# 07: std::span 与 std::source_location — 安全数组视图、编译期源码位置
add_executable(07_span_and_source_location_demo examples/span_and_source_location_demo.cpp)
target_link_libraries(07_span_and_source_location_demo PRIVATE cpp20_23_lib)

# 08: <=> 三路比较 — strong/weak/partial_ordering、=default 自动生成运算符
add_executable(08_three_way_comparison_demo examples/three_way_comparison_demo.cpp)
target_link_libraries(08_three_way_comparison_demo PRIVATE cpp20_23_lib)

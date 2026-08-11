# ------------------------------------------------------------------------------
# 07_modern_syntax — 现代语法
#
# 本章覆盖 C++11/14/17/20 现代语法特性：
#   - auto / decltype / decltype(auto) / 尾置返回类型
#   - 类模板参数推导 (CTAD) / 推导指引 / 聚合 CTAD (C++17/20)
#   - enum class (限定作用域枚举) / 底层类型 / using enum (C++20)
#   - 统一初始化 {} / initializer_list / 指定初始化 (C++20)
#   - range-for / init-if / init-switch / [[likely]] [[unlikely]]
#   - 用户定义字面量 (operator"") / _km _deg / std::to_underlying / 标准库字面量
#   - [[no_unique_address]] / explicit(bool) / hardware_destructive_interference_size
#
# Library:  modern_syntax_lib
#   Sources: src/auto_decltype.cpp
#            src/ctad.cpp
#            src/enum_class.cpp
#            src/init_and_designated.cpp
#            src/range_for_init_if.cpp
#            src/structured_bindings.cpp
#            src/user_defined_literals.cpp
#            src/attributes_and_explicit_bool.cpp
#   Headers: include/modern_syntax/*.hpp
#   Links:   lesson_utils (PUBLIC — 所有模块都需要)
#
# Examples:
#   01_auto_decltype_demo          → links modern_syntax_lib
#   02_ctad_demo                   → links modern_syntax_lib
#   03_enum_class_demo             → links modern_syntax_lib
#   04_init_and_designated_demo    → links modern_syntax_lib
#   05_range_for_init_if_demo      → links modern_syntax_lib
#   06_structured_bindings_demo    → links modern_syntax_lib
#   07_user_defined_literals_demo  → links modern_syntax_lib
#   08_attributes_explicit_demo    → links modern_syntax_lib
# ------------------------------------------------------------------------------

# ---- Library ----
# 静态库，包含所有可复用的现代语法工具函数和类型
add_library(modern_syntax_lib STATIC
    src/auto_decltype.cpp
    src/ctad.cpp
    src/enum_class.cpp
    src/init_and_designated.cpp
    src/range_for_init_if.cpp
    src/structured_bindings.cpp
    src/user_defined_literals.cpp
    src/attributes_and_explicit_bool.cpp
)

# PUBLIC: 所有链接此库的目标都可以访问 include/ 下的头文件
target_include_directories(modern_syntax_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# PUBLIC: lesson_utils 提供所有模块公用的日志/打印/工具函数
target_link_libraries(modern_syntax_lib PUBLIC
    lesson_utils
)

# ---- Examples ----
# 每个示例是独立的可执行文件，PRIVATE 链接 modern_syntax_lib

add_executable(01_auto_decltype_demo examples/auto_decltype_demo.cpp)
target_link_libraries(01_auto_decltype_demo PRIVATE modern_syntax_lib)

add_executable(02_ctad_demo examples/ctad_demo.cpp)
target_link_libraries(02_ctad_demo PRIVATE modern_syntax_lib)

add_executable(03_enum_class_demo examples/enum_class_demo.cpp)
target_link_libraries(03_enum_class_demo PRIVATE modern_syntax_lib)

add_executable(04_init_and_designated_demo examples/init_and_designated_demo.cpp)
target_link_libraries(04_init_and_designated_demo PRIVATE modern_syntax_lib)

add_executable(05_range_for_init_if_demo examples/range_for_init_if_demo.cpp)
target_link_libraries(05_range_for_init_if_demo PRIVATE modern_syntax_lib)

add_executable(06_structured_bindings_demo examples/structured_bindings_demo.cpp)
target_link_libraries(06_structured_bindings_demo PRIVATE modern_syntax_lib)

add_executable(07_user_defined_literals_demo examples/07_user_defined_literals_demo.cpp)
target_link_libraries(07_user_defined_literals_demo PRIVATE modern_syntax_lib)

add_executable(08_attributes_explicit_demo examples/08_attributes_and_explicit_bool_demo.cpp)
target_link_libraries(08_attributes_explicit_demo PRIVATE modern_syntax_lib)

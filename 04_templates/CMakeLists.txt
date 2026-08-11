# ------------------------------------------------------------------------------
# 04_templates — 模板
#
# 涵盖：函数模板、类模板、全特化/偏特化、变参模板、折叠表达式、
#       type_traits/SFINAE、C++20 Concepts。
#
# Library:  templates_lib
#   Sources: src/type_traits_basics.cpp（仅有这个函数非模板，可分离到 .cpp）
#   Headers: include/templates/ 下 7 个头文件（均为纯模板头文件）
#   Links:   lesson_utils（所有模块公共依赖）
#
# Examples:
#   01_function_template_demo      — 函数模板基础、auto 模板参数
#   02_class_template_demo         — 类模板、NTTP、auto NTTP、默认模板参数
#   03_specialization_demo         — 全特化、偏特化、变量模板、traits 模式
#   04_variadic_template_demo      — 变参模板、递归展开、逗号展开
#   05_fold_expressions_demo       — 一元/二元折叠表达式
#   06_type_traits_basics_demo     — type_traits、SFINAE、void_t、if constexpr
#   07_concepts_demo               — C++20 Concepts、requires 表达式、简写语法
# ------------------------------------------------------------------------------

# ---- Library ----
# 本模块绝大多数代码是头文件模板（header-only），只有 demonstrate_type_traits()
# 被实现在 .cpp 中，因此库只有一个源文件。
add_library(templates_lib STATIC
    src/type_traits_basics.cpp
)

# PUBLIC：头文件中定义了大量函数模板和类模板，使用者需要以相同编译选项
# 实例化这些模板，所以 include 路径必须透传。
target_include_directories(templates_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# lesson_utils 提供统一的输出流程（lesson::print_title / print_subtitle 等），
# 被 src/type_traits_basics.cpp 直接使用，且任何使用本库的示例也可能需要，
# 因此设为 PUBLIC。
target_link_libraries(templates_lib PUBLIC
    lesson_utils
)

# ---- Examples ----

# 示例 01：函数模板基础
# 演示 my_max、my_min、print_type_info、auto_max 等函数模板用法
add_executable(01_function_template_demo
    examples/01_function_template_demo.cpp
)
target_link_libraries(01_function_template_demo PRIVATE
    templates_lib
)

# 示例 02：类模板基础
# 演示 Box、Calculator、FixedStack、Constant、Buffer 等类模板用法
add_executable(02_class_template_demo
    examples/02_class_template_demo.cpp
)
target_link_libraries(02_class_template_demo PRIVATE
    templates_lib
)

# 示例 03：模板特化
# 演示全特化、偏特化、变量模板、traits 模式、smart_print
add_executable(03_specialization_demo
    examples/03_specialization_demo.cpp
)
target_link_libraries(03_specialization_demo PRIVATE
    templates_lib
)

# 示例 04：变参模板
# 演示参数包、递归展开、逗号展开、print_all、concat
add_executable(04_variadic_template_demo
    examples/04_variadic_template_demo.cpp
)
target_link_libraries(04_variadic_template_demo PRIVATE
    templates_lib
)

# 示例 05：折叠表达式 (C++17)
# 演示一元/二元折叠、逻辑折叠、push_all、formatted_print
add_executable(05_fold_expressions_demo
    examples/05_fold_expressions_demo.cpp
)
target_link_libraries(05_fold_expressions_demo PRIVATE
    templates_lib
)

# 示例 06：type_traits 基础与 SFINAE
# 演示 type_traits 常用工具、enable_if、void_t、if constexpr
add_executable(06_type_traits_basics_demo
    examples/06_type_traits_basics_demo.cpp
)
target_link_libraries(06_type_traits_basics_demo PRIVATE
    templates_lib
)

# 示例 07：C++20 Concepts
# 演示标准 concept、自定义 concept、requires 表达式、简写函数模板语法
add_executable(07_concepts_demo
    examples/07_concepts_demo.cpp
)
target_link_libraries(07_concepts_demo PRIVATE
    templates_lib
)

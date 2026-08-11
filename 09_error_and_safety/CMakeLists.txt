# ------------------------------------------------------------------------------
# 09_error_and_safety — 错误处理与安全
#
# Library:  error_safety_lib
#   Sources: src/exceptions.cpp
#            src/noexcept_contracts.cpp
#            src/error_code.cpp
#            src/expected_utils.cpp
#            src/stacktrace.cpp
#   Headers: include/error_safety/exceptions.hpp
#            include/error_safety/noexcept_contracts.hpp
#            include/error_safety/error_code.hpp
#            include/error_safety/expected_utils.hpp
#            include/error_safety/stacktrace.hpp
#   Links:   lesson_utils (PUBLIC — 提供 lesson::print_header 等共享工具)
#
# Examples:
#   01_exceptions_demo            → 异常机制基础 (throw/try/catch/栈展开/重新抛出)
#   02_noexcept_and_contracts_demo → noexcept 说明符/运算符、异常安全三级保证、
#                                    noexcept 移动的重要性、[[nodiscard]]
#   03_error_code_demo            → std::error_code / error_condition、
#                                    自定义 error_category、std::system_error
#   04_expected_demo              → std::expected<T,E> (C++23)、单子操作
#                                    (and_then / or_else / transform)
#   05_stacktrace_demo            → std::stacktrace (C++23)、调用栈捕获、
#                                    basic_stacktrace/stacktrace_entry、
#                                    与异常/日志集成
#
# Dependencies rationale:
#   - 本模块的所有头文件仅依赖 C++ 标准库，不依赖任何前置模块的 library，
#     因此 error_safety_lib 除 lesson_utils 外无需链接其他模块。
#   - 所有 example 文件均直接 include "shared/lesson_utils.hpp"，通过
#     error_safety_lib → lesson_utils 的 PUBLIC 传递获得头文件可见性。
#   - example 可执行文件以 PRIVATE 链接 error_safety_lib，不向外暴露依赖。
# ------------------------------------------------------------------------------

# ---- Library ---------------------------------------------------------------

add_library(error_safety_lib STATIC
    src/exceptions.cpp
    src/noexcept_contracts.cpp
    src/error_code.cpp
    src/expected_utils.cpp
    src/stacktrace.cpp
)

target_include_directories(error_safety_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(error_safety_lib PUBLIC
    lesson_utils
)

# ---- Examples --------------------------------------------------------------

add_executable(01_exceptions_demo examples/01_exceptions_demo.cpp)
target_link_libraries(01_exceptions_demo PRIVATE error_safety_lib)

add_executable(02_noexcept_and_contracts_demo examples/02_noexcept_and_contracts_demo.cpp)
target_link_libraries(02_noexcept_and_contracts_demo PRIVATE error_safety_lib)

add_executable(03_error_code_demo examples/03_error_code_demo.cpp)
target_link_libraries(03_error_code_demo PRIVATE error_safety_lib)

add_executable(04_expected_demo examples/04_expected_demo.cpp)
target_link_libraries(04_expected_demo PRIVATE error_safety_lib)

add_executable(05_stacktrace_demo examples/05_stacktrace_demo.cpp)
target_link_libraries(05_stacktrace_demo PRIVATE error_safety_lib)

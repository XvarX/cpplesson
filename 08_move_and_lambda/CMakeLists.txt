# ------------------------------------------------------------------------------
# 08_move_and_lambda — 移动与Lambda
#
# Library:  move_lambda_lib (STATIC)
#   Sources: src/rvalue_move.cpp
#            src/forward_perfect.cpp
#            src/lambda_basics.cpp
#            src/lambda_advanced.cpp
#            src/functional.cpp
#   Headers: include/move_lambda/*.hpp
#   Links:   lesson_utils (PUBLIC — all modules share shared/ utilities)
#
# Examples (each is standalone; all use lesson_utils via move_lambda_lib transitively):
#   01_rvalue_move_demo        — 左值/右值、移动语义、noexcept 移动、RVO/NRVO
#   02_forward_perfect_demo    — std::forward、万能引用、完美转发、emplace
#   03_lambda_basics_demo      — Lambda 语法、捕获方式、mutable、IILE、[this]/[*this]
#   04_lambda_advanced_demo    — 泛型lambda、constexpr lambda、C++20 显式this捕获
#   05_functional_demo         — std::function、bind、mem_fn、invoke、reference_wrapper
# ------------------------------------------------------------------------------

# ---- Library (STATIC — headers are template-heavy, no DLL export needed) ----
add_library(move_lambda_lib STATIC
    src/rvalue_move.cpp
    src/forward_perfect.cpp
    src/lambda_basics.cpp
    src/lambda_advanced.cpp
    src/functional.cpp
)

# PUBLIC: any consumer of move_lambda_lib must also see these headers
target_include_directories(move_lambda_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# PUBLIC: lesson_utils provides shared utilities (lesson_main, section/print helpers)
# that the library and its consumers both depend on
target_link_libraries(move_lambda_lib PUBLIC
    lesson_utils
)

# ---- Examples ----
# Each example is a standalone executable that links move_lambda_lib privately.
# lesson_utils is available transitively through move_lambda_lib's PUBLIC linkage.

add_executable(01_rvalue_move_demo examples/01_rvalue_move_demo.cpp)
target_link_libraries(01_rvalue_move_demo PRIVATE move_lambda_lib)

add_executable(02_forward_perfect_demo examples/02_forward_perfect_demo.cpp)
target_link_libraries(02_forward_perfect_demo PRIVATE move_lambda_lib)

add_executable(03_lambda_basics_demo examples/03_lambda_basics_demo.cpp)
target_link_libraries(03_lambda_basics_demo PRIVATE move_lambda_lib)

add_executable(04_lambda_advanced_demo examples/04_lambda_advanced_demo.cpp)
target_link_libraries(04_lambda_advanced_demo PRIVATE move_lambda_lib)

add_executable(05_functional_demo examples/05_functional_demo.cpp)
target_link_libraries(05_functional_demo PRIVATE move_lambda_lib)

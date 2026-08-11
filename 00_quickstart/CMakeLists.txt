# ------------------------------------------------------------------------------
# 00_quickstart — 环境验证
#
# Library:  quickstart_lib
#   Sources: src/compiler_info.cpp
#   Headers: include/quickstart/compiler_info.hpp
#   Links:   lesson_utils (PUBLIC)
#
# Examples:
#   hello_demo → links quickstart_lib
# ------------------------------------------------------------------------------

# ---- Library ----------------------------------------------------------------

add_library(quickstart_lib STATIC
    src/compiler_info.cpp
)

target_include_directories(quickstart_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(quickstart_lib PUBLIC
    lesson_utils
)

# ---- Examples ---------------------------------------------------------------

add_executable(hello_demo examples/hello_demo.cpp)
target_link_libraries(hello_demo PRIVATE quickstart_lib)

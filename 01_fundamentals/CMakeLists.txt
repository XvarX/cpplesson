# ------------------------------------------------------------------------------
# 01_fundamentals — 语言基础
#
# Library:  fundamentals_lib
#   Sources: src/types_literals.cpp, src/const_static.cpp, src/arrays_strings.cpp,
#            src/pointers_refs.cpp, src/functions_basics.cpp, src/control_flow.cpp
#   Headers: include/fundamentals/*.hpp
#   Links:   PUBLIC → lesson_utils (logging/utility macros shared across all modules)
#
# Examples:
#   01_types_and_literals_demo → links fundamentals_lib
#   02_const_and_static_demo   → links fundamentals_lib
#   03_arrays_and_strings_demo → links fundamentals_lib
#   04_pointers_and_refs_demo  → links fundamentals_lib
#   05_function_basics_demo    → links fundamentals_lib
#   06_control_flow_demo       → links fundamentals_lib
# ------------------------------------------------------------------------------

# ==============================================================================
# Library
# ==============================================================================
# STATIC library: each module compiles into a self-contained .a/.lib archive.
# Consumers pick up only the symbols they reference (dead-code elimination at
# link time), and there is no runtime shared-library lookup cost.

add_library(fundamentals_lib STATIC
    src/types_literals.cpp
    src/const_static.cpp
    src/arrays_strings.cpp
    src/pointers_refs.cpp
    src/functions_basics.cpp
    src/control_flow.cpp
)

# PUBLIC include directory — any target that links fundamentals_lib inherits
# this include path.  Code inside the library uses angle-bracket includes like
#   #include <fundamentals/types_literals.hpp>
# and consumers get the same include path automatically.

target_include_directories(fundamentals_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# PUBLIC dependency on lesson_utils — all example files use LOG(), SEPARATOR(),
# and other utility macros declared in lesson_utils headers.  Making it PUBLIC
# means consumers of fundamentals_lib don't need to re-declare this dependency.
# The headers in include/fundamentals/*.hpp use only standard-library includes
# and have no dependency on any other lesson module, so no additional PUBLIC
# libraries are needed.

target_link_libraries(fundamentals_lib PUBLIC
    lesson_utils
)

# ==============================================================================
# Examples
# ==============================================================================
# Each example is a standalone executable that exercises one topic area.
# They link fundamentals_lib PRIVATE because the examples themselves don't
# re-export any headers — only the compiled demo .cpp needs the library.

add_executable(01_types_and_literals_demo
    examples/01_types_and_literals_demo.cpp
)
target_link_libraries(01_types_and_literals_demo PRIVATE fundamentals_lib)

add_executable(02_const_and_static_demo
    examples/02_const_and_static_demo.cpp
)
target_link_libraries(02_const_and_static_demo PRIVATE fundamentals_lib)

add_executable(03_arrays_and_strings_demo
    examples/03_arrays_and_strings_demo.cpp
)
target_link_libraries(03_arrays_and_strings_demo PRIVATE fundamentals_lib)

add_executable(04_pointers_and_refs_demo
    examples/04_pointers_and_refs_demo.cpp
)
target_link_libraries(04_pointers_and_refs_demo PRIVATE fundamentals_lib)

add_executable(05_function_basics_demo
    examples/05_function_basics_demo.cpp
)
target_link_libraries(05_function_basics_demo PRIVATE fundamentals_lib)

add_executable(06_control_flow_demo
    examples/06_control_flow_demo.cpp
)
target_link_libraries(06_control_flow_demo PRIVATE fundamentals_lib)

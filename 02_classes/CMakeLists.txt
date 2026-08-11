# ------------------------------------------------------------------------------
# 02_classes — 类与OOP
#
# Library:  classes_lib
#   Sources: src/class_basics.cpp, src/ctor_dtor.cpp, src/copy_move.cpp,
#            src/operator_overload.cpp, src/inheritance.cpp,
#            src/polymorphism.cpp, src/raii.cpp, src/friend_nested.cpp,
#            src/union_and_volatile.cpp
#   Headers: include/classes/*.hpp
#   Links:   lesson_utils (PUBLIC — headers／examples 依赖 shared/lesson_utils.hpp)
#
#   Note: 不依赖 01_fundamentals 模块——本模块所有头文件只使用 C++ 标准库设施，
#         未包含任何 fundamentals 类型。
#
# Examples:
#   01_class_declaration_demo        → links classes_lib
#   02_ctor_dtor_demo                → links classes_lib
#   03_copy_move_control_demo        → links classes_lib
#   04_operator_overload_demo        → links classes_lib
#   05_inheritance_demo              → links classes_lib
#   06_virtual_and_polymorphism_demo → links classes_lib
#   07_raii_and_resource_demo        → links classes_lib
#   08_friend_and_nested_demo        → links classes_lib
#   09_union_and_volatile_demo       → links classes_lib
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)

# ---- Library ----
# 静态库：将 src/ 下 9 个 .cpp 编译为 classes_lib，供所有示例程序链接
add_library(classes_lib STATIC
    src/class_basics.cpp
    src/ctor_dtor.cpp
    src/copy_move.cpp
    src/operator_overload.cpp
    src/inheritance.cpp
    src/polymorphism.cpp
    src/raii.cpp
    src/friend_nested.cpp
    src/union_and_volatile.cpp
)

# PUBLIC：include 目录对直接／间接使用者均可见（示例通过 PRIVATE 链接 classes_lib 即可看到头文件）
target_include_directories(classes_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# PUBLIC：lesson_utils 是共享工具库，所有模块都需要；设为 PUBLIC 后示例无需重复声明
target_link_libraries(classes_lib PUBLIC
    lesson_utils
)

# 要求 C++20：本模块使用了三路比较(<=>)、std::print、std::format 等 C++20 特性
target_compile_features(classes_lib PUBLIC cxx_std_20)

# ---- Examples ----
# 每个 examples/*.cpp 对应一个独立可执行文件，逐个显式声明
# （禁止使用 foreach/macro/function 包装——保持 CMake 可读性与可搜索性）

add_executable(01_class_declaration_demo
    examples/01_class_declaration_demo.cpp
)
target_link_libraries(01_class_declaration_demo PRIVATE classes_lib)

add_executable(02_ctor_dtor_demo
    examples/02_ctor_dtor_demo.cpp
)
target_link_libraries(02_ctor_dtor_demo PRIVATE classes_lib)

add_executable(03_copy_move_control_demo
    examples/03_copy_move_control_demo.cpp
)
target_link_libraries(03_copy_move_control_demo PRIVATE classes_lib)

add_executable(04_operator_overload_demo
    examples/04_operator_overload_demo.cpp
)
target_link_libraries(04_operator_overload_demo PRIVATE classes_lib)

add_executable(05_inheritance_demo
    examples/05_inheritance_demo.cpp
)
target_link_libraries(05_inheritance_demo PRIVATE classes_lib)

add_executable(06_virtual_and_polymorphism_demo
    examples/06_virtual_and_polymorphism_demo.cpp
)
target_link_libraries(06_virtual_and_polymorphism_demo PRIVATE classes_lib)

add_executable(07_raii_and_resource_demo
    examples/07_raii_and_resource_demo.cpp
)
target_link_libraries(07_raii_and_resource_demo PRIVATE classes_lib)

add_executable(08_friend_and_nested_demo
    examples/08_friend_and_nested_demo.cpp
)
target_link_libraries(08_friend_and_nested_demo PRIVATE classes_lib)

add_executable(09_union_and_volatile_demo
    examples/09_union_and_volatile_demo.cpp
)
target_link_libraries(09_union_and_volatile_demo PRIVATE classes_lib)

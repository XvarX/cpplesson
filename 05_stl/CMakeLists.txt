# ------------------------------------------------------------------------------
# 05_stl — STL标准库
#
# 模块内容: C++ 标准模板库核心组件教学，涵盖顺序容器、关联容器、无序容器、
#           容器适配器、迭代器、算法（非修改/修改/排序）、pair/tuple/optional/
#           variant、chrono 时间库与 random 随机数库
#
# Library:  stl_lib
#   命名空间: stl_learn
#   源码:     src/ 中的 10 个 .cpp 文件
#   头文件:   include/stl/*.hpp
#   公共依赖: lesson_utils（所有头文件均包含 "shared/lesson_utils.hpp"）
#
# Examples:  10 个演示程序，每个对应一个主题
# ------------------------------------------------------------------------------

# ---- Library ---------------------------------------------------------------

add_library(stl_lib STATIC
    src/sequential_containers.cpp
    src/associative_containers.cpp
    src/unordered_containers.cpp
    src/container_adaptors.cpp
    src/iterators.cpp
    src/algorithms_nonmod.cpp
    src/algorithms_mod.cpp
    src/algorithms_sort.cpp
    src/tuple_types.cpp
    src/chrono_random.cpp
)

# PUBLIC: 使用者需要 include/stl/ 下的头文件来调用库函数
target_include_directories(stl_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# PUBLIC: 模块头文件全部包含 "shared/lesson_utils.hpp"，任何链接 stl_lib 的
#         目标也需要该头文件路径
target_link_libraries(stl_lib PUBLIC
    lesson_utils
)

# ---- Examples --------------------------------------------------------------
# 每个 add_executable 单独写出一行，不使用函数/循环/macro 以减少抽象层

add_executable(01_sequential_containers_demo examples/01_sequential_containers_demo.cpp)
target_link_libraries(01_sequential_containers_demo PRIVATE stl_lib)

add_executable(02_associative_containers_demo examples/02_associative_containers_demo.cpp)
target_link_libraries(02_associative_containers_demo PRIVATE stl_lib)

add_executable(03_unordered_containers_demo examples/03_unordered_containers_demo.cpp)
target_link_libraries(03_unordered_containers_demo PRIVATE stl_lib)

add_executable(04_container_adaptors_demo examples/04_container_adaptors_demo.cpp)
target_link_libraries(04_container_adaptors_demo PRIVATE stl_lib)

add_executable(05_iterators_demo examples/05_iterators_demo.cpp)
target_link_libraries(05_iterators_demo PRIVATE stl_lib)

add_executable(06_algorithms_nonmod_demo examples/06_algorithms_nonmod_demo.cpp)
target_link_libraries(06_algorithms_nonmod_demo PRIVATE stl_lib)

add_executable(07_algorithms_mod_demo examples/07_algorithms_mod_demo.cpp)
target_link_libraries(07_algorithms_mod_demo PRIVATE stl_lib)

add_executable(08_algorithms_sort_demo examples/08_algorithms_sort_demo.cpp)
target_link_libraries(08_algorithms_sort_demo PRIVATE stl_lib)

add_executable(09_tuple_pair_optional_variant_demo examples/09_tuple_pair_optional_variant_demo.cpp)
target_link_libraries(09_tuple_pair_optional_variant_demo PRIVATE stl_lib)

add_executable(10_chrono_random_demo examples/10_chrono_random_demo.cpp)
target_link_libraries(10_chrono_random_demo PRIVATE stl_lib)

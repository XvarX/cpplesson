# ------------------------------------------------------------------------------
# 03_memory — 内存管理
#
# Library:  memory_lib
#   Sources: src/stack_vs_heap.cpp, src/unique_ptr.cpp
#   Headers: include/memory/*.hpp
#   Links:   lesson_utils (PUBLIC — 所有模块通用工具库)
#
#   本模块所有头文件仅依赖 C++ 标准库，不依赖其他 lesson 模块。
#
# Examples:
#   01_stack_vs_heap_demo      — 栈 vs 堆分配，全局 operator new/delete 追踪
#   02_placement_new_demo      — placement new、对齐、手动内存管理
#   03_unique_ptr_demo         — unique_ptr 独占所有权与自定义删除器
#   04_shared_weak_ptr_demo    — shared_ptr/weak_ptr、循环引用、enable_shared_from_this
#   05_allocator_basics_demo   — 自定义分配器入门 (CountingAllocator)
#   06_memory_pool_demo        — 固定大小 MemoryPool (freelist + placement new + allocator)
# ------------------------------------------------------------------------------

# ---- Library ----------------------------------------------------------------
# 静态库: 包含全局 operator new/delete 重载 和 unique_ptr 辅助函数实现
add_library(memory_lib STATIC
    src/stack_vs_heap.cpp
    src/unique_ptr.cpp
    src/memory_pool.cpp
)

# PUBLIC: 下游只需链接 memory_lib 即可自动获得 include/memory 路径
target_include_directories(memory_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# PUBLIC: lesson_utils 是项目级通用工具库，所有模块都需要
target_link_libraries(memory_lib PUBLIC
    lesson_utils
)

# ---- Examples ---------------------------------------------------------------
# 每个示例独立可执行文件，PRIVATE 链接 memory_lib (无需传播依赖)

# 01 — 栈 vs 堆分配演示
add_executable(01_stack_vs_heap_demo examples/01_stack_vs_heap_demo.cpp)
target_link_libraries(01_stack_vs_heap_demo PRIVATE memory_lib)

# 02 — placement new、对齐、手动内存管理演示
add_executable(02_placement_new_demo examples/02_placement_new_demo.cpp)
target_link_libraries(02_placement_new_demo PRIVATE memory_lib)

# 03 — unique_ptr 独占所有权与自定义删除器演示
add_executable(03_unique_ptr_demo examples/03_unique_ptr_demo.cpp)
target_link_libraries(03_unique_ptr_demo PRIVATE memory_lib)

# 04 — shared_ptr/weak_ptr、循环引用、enable_shared_from_this 演示
add_executable(04_shared_weak_ptr_demo examples/04_shared_weak_ptr_demo.cpp)
target_link_libraries(04_shared_weak_ptr_demo PRIVATE memory_lib)

# 05 — 自定义分配器 (CountingAllocator) 入门演示
add_executable(05_allocator_basics_demo examples/05_allocator_basics_demo.cpp)
target_link_libraries(05_allocator_basics_demo PRIVATE memory_lib)

# 06 — 固定大小 MemoryPool: freelist、placement new、std::allocator 集成
add_executable(06_memory_pool_demo examples/06_memory_pool_demo.cpp)
target_link_libraries(06_memory_pool_demo PRIVATE memory_lib)

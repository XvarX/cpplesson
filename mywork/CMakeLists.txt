# ╔══════════════════════════════════════════════════════════════════════════════╗
# ║  mywork — 你的练习工程                                                      ║
# ║                                                                            ║
# ║  每学完一章，在下面按模板格式添加你的库和示例。                              ║
# ║  照着实工程的方式：显式列出源文件、手动管理依赖。                            ║
# ║                                                                            ║
# ║  目录结构:                                                                 ║
# ║    mywork/                                                                 ║
# ║    ├── include/            你的头文件 (按模块分子目录)                      ║
# ║    │   ├── fundamentals/                                                    ║
# ║    │   ├── classes/                                                         ║
# ║    │   └── ...                                                              ║
# ║    ├── src/                你的实现 (按模块分子目录)                        ║
# ║    │   ├── fundamentals/                                                    ║
# ║    │   ├── classes/                                                         ║
# ║    │   └── ...                                                              ║
# ║    └── examples/           你的演示程序 (按模块分子目录, 每个有 main)       ║
# ║        ├── fundamentals/                                                    ║
# ║        ├── classes/                                                         ║
# ║        └── ...                                                              ║
# ╚══════════════════════════════════════════════════════════════════════════════╝

cmake_minimum_required(VERSION 3.20)

# ═══════════════════════════════════════════════════════════════════════════════
# 第 1 章 — fundamentals (语言基础)
# ═══════════════════════════════════════════════════════════════════════════════
# 模板位置: 01_fundamentals/
# 敲完后取消下面代码的注释即可编译

# add_library(mywork_fundamentals STATIC
#     src/fundamentals/types_literals.cpp
#     src/fundamentals/const_static.cpp
#     src/fundamentals/arrays_strings.cpp
#     src/fundamentals/pointers_refs.cpp
#     src/fundamentals/functions_basics.cpp
#     src/fundamentals/control_flow.cpp
# )
# target_include_directories(mywork_fundamentals PUBLIC include)
# target_link_libraries(mywork_fundamentals PUBLIC lesson_utils)

# add_executable(01_types_demo examples/fundamentals/01_types_and_literals_demo.cpp)
# target_link_libraries(01_types_demo PRIVATE mywork_fundamentals)
# add_executable(02_const_demo examples/fundamentals/02_const_and_static_demo.cpp)
# target_link_libraries(02_const_demo PRIVATE mywork_fundamentals)
# add_executable(03_arrays_demo examples/fundamentals/03_arrays_and_strings_demo.cpp)
# target_link_libraries(03_arrays_demo PRIVATE mywork_fundamentals)
# add_executable(04_pointers_demo examples/fundamentals/04_pointers_and_refs_demo.cpp)
# target_link_libraries(04_pointers_demo PRIVATE mywork_fundamentals)
# add_executable(05_functions_demo examples/fundamentals/05_function_basics_demo.cpp)
# target_link_libraries(05_functions_demo PRIVATE mywork_fundamentals)
# add_executable(06_control_demo examples/fundamentals/06_control_flow_demo.cpp)
# target_link_libraries(06_control_demo PRIVATE mywork_fundamentals)


# ═══════════════════════════════════════════════════════════════════════════════
# 第 2 章 — classes (类与面向对象)
# ═══════════════════════════════════════════════════════════════════════════════
# 模板位置: 02_classes/
# 学完第 2 章后取消注释

# add_library(mywork_classes STATIC
#     src/classes/class_basics.cpp
#     src/classes/ctor_dtor.cpp
#     src/classes/copy_move.cpp
#     src/classes/operator_overload.cpp
#     src/classes/inheritance.cpp
#     src/classes/polymorphism.cpp
#     src/classes/raii.cpp
#     src/classes/friend_nested.cpp
# )
# target_include_directories(mywork_classes PUBLIC include)
# target_link_libraries(mywork_classes PUBLIC lesson_utils)

# add_executable(01_class_demo examples/classes/01_class_declaration_demo.cpp)
# target_link_libraries(01_class_demo PRIVATE mywork_classes)
# add_executable(02_ctor_demo examples/classes/02_ctor_dtor_demo.cpp)
# target_link_libraries(02_ctor_demo PRIVATE mywork_classes)
# add_executable(03_copy_move_demo examples/classes/03_copy_move_control_demo.cpp)
# target_link_libraries(03_copy_move_demo PRIVATE mywork_classes)
# add_executable(04_operator_demo examples/classes/04_operator_overload_demo.cpp)
# target_link_libraries(04_operator_demo PRIVATE mywork_classes)
# add_executable(05_inheritance_demo examples/classes/05_inheritance_demo.cpp)
# target_link_libraries(05_inheritance_demo PRIVATE mywork_classes)
# add_executable(06_virtual_demo examples/classes/06_virtual_and_polymorphism_demo.cpp)
# target_link_libraries(06_virtual_demo PRIVATE mywork_classes)
# add_executable(07_raii_demo examples/classes/07_raii_and_resource_demo.cpp)
# target_link_libraries(07_raii_demo PRIVATE mywork_classes)
# add_executable(08_friend_demo examples/classes/08_friend_and_nested_demo.cpp)
# target_link_libraries(08_friend_demo PRIVATE mywork_classes)


# ═══════════════════════════════════════════════════════════════════════════════
# 第 3 章 — memory (内存管理)
# ═══════════════════════════════════════════════════════════════════════════════
# 模板位置: 03_memory/
# 学完第 3 章后取消注释

# add_library(mywork_memory STATIC
#     src/memory/stack_vs_heap.cpp
#     src/memory/placement_new.cpp
#     src/memory/unique_ptr.cpp
#     src/memory/shared_weak_ptr.cpp
#     src/memory/allocator_basics.cpp
# )
# target_include_directories(mywork_memory PUBLIC include)
# target_link_libraries(mywork_memory PUBLIC lesson_utils)

# add_executable(01_stack_heap_demo examples/memory/01_stack_vs_heap_demo.cpp)
# target_link_libraries(01_stack_heap_demo PRIVATE mywork_memory)
# add_executable(02_placement_demo examples/memory/02_placement_new_demo.cpp)
# target_link_libraries(02_placement_demo PRIVATE mywork_memory)
# add_executable(03_unique_ptr_demo examples/memory/03_unique_ptr_demo.cpp)
# target_link_libraries(03_unique_ptr_demo PRIVATE mywork_memory)
# add_executable(04_shared_weak_demo examples/memory/04_shared_weak_ptr_demo.cpp)
# target_link_libraries(04_shared_weak_demo PRIVATE mywork_memory)
# add_executable(05_allocator_demo examples/memory/05_allocator_basics_demo.cpp)
# target_link_libraries(05_allocator_demo PRIVATE mywork_memory)


# ═══════════════════════════════════════════════════════════════════════════════
# 第 4 章 — templates (模板)
# ═══════════════════════════════════════════════════════════════════════════════
# 模板大部分是 header-only，src/ 只有少量非模板实现

# add_library(mywork_templates STATIC
#     src/templates/type_traits_basics.cpp
# )
# target_include_directories(mywork_templates PUBLIC include)
# target_link_libraries(mywork_templates PUBLIC lesson_utils)

# add_executable(01_func_template_demo examples/templates/01_function_template_demo.cpp)
# target_link_libraries(01_func_template_demo PRIVATE mywork_templates)
# add_executable(02_class_template_demo examples/templates/02_class_template_demo.cpp)
# target_link_libraries(02_class_template_demo PRIVATE mywork_templates)
# add_executable(03_specialization_demo examples/templates/03_specialization_demo.cpp)
# target_link_libraries(03_specialization_demo PRIVATE mywork_templates)
# add_executable(04_variadic_demo examples/templates/04_variadic_template_demo.cpp)
# target_link_libraries(04_variadic_demo PRIVATE mywork_templates)
# add_executable(05_fold_demo examples/templates/05_fold_expressions_demo.cpp)
# target_link_libraries(05_fold_demo PRIVATE mywork_templates)
# add_executable(06_traits_demo examples/templates/06_type_traits_basics_demo.cpp)
# target_link_libraries(06_traits_demo PRIVATE mywork_templates)
# add_executable(07_concepts_demo examples/templates/07_concepts_demo.cpp)
# target_link_libraries(07_concepts_demo PRIVATE mywork_templates)


# ═══════════════════════════════════════════════════════════════════════════════
# 第 5-13 章模板同上，学完一章取消注释一章。
# 你敲完所有 14 章后，这个 CMakeLists.txt 就是一个完整的工业级构建文件。
# ═══════════════════════════════════════════════════════════════════════════════

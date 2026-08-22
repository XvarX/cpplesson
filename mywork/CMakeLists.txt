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

add_library(mywork_fundamentals STATIC
    src/fundamentals/types_literals.cpp
    src/fundamentals/const_static.cpp
    src/fundamentals/arrays_strings.cpp
)

target_include_directories(mywork_fundamentals PUBLIC include)
target_link_libraries(mywork_fundamentals PUBLIC lesson_utils)

add_executable(types_demo examples/fundamentals/01_types_and_literals_demo.cpp)
target_link_libraries(types_demo PRIVATE mywork_fundamentals)

add_executable(const_demo examples/fundamentals/02_const_and_static_demo.cpp)
target_link_libraries(const_demo PRIVATE mywork_fundamentals)

add_executable(arrays_demo examples/fundamentals/03_arrays_and_strings_demo.cpp)
target_link_libraries(arrays_demo PRIVATE mywork_fundamentals)

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
#     src/classes/union_and_volatile.cpp
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
# add_executable(09_union_volatile_demo examples/classes/09_union_and_volatile_demo.cpp)
# target_link_libraries(09_union_volatile_demo PRIVATE mywork_classes)


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
#     src/memory/memory_pool.cpp
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
# add_executable(06_memory_pool_demo examples/memory/06_memory_pool_demo.cpp)
# target_link_libraries(06_memory_pool_demo PRIVATE mywork_memory)


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
# 第 7 章 — modern_syntax (现代语法糖)
# ═══════════════════════════════════════════════════════════════════════════════
# 模板位置: 07_modern_syntax/
# 学完第 7 章后取消注释

# add_library(mywork_modern_syntax STATIC
#     src/modern_syntax/auto_decltype.cpp
#     src/modern_syntax/ctad.cpp
#     src/modern_syntax/enum_class.cpp
#     src/modern_syntax/init_and_designated.cpp
#     src/modern_syntax/range_for_init_if.cpp
#     src/modern_syntax/structured_bindings.cpp
#     src/modern_syntax/user_defined_literals.cpp
#     src/modern_syntax/attributes_and_explicit_bool.cpp
# )
# target_include_directories(mywork_modern_syntax PUBLIC include)
# target_link_libraries(mywork_modern_syntax PUBLIC lesson_utils)

# add_executable(07_udl_demo examples/modern_syntax/07_user_defined_literals_demo.cpp)
# target_link_libraries(07_udl_demo PRIVATE mywork_modern_syntax)
# add_executable(08_attributes_demo examples/modern_syntax/08_attributes_and_explicit_bool_demo.cpp)
# target_link_libraries(08_attributes_demo PRIVATE mywork_modern_syntax)


# ═══════════════════════════════════════════════════════════════════════════════
# 第 9 章 — error_safety (错误处理与安全)
# ═══════════════════════════════════════════════════════════════════════════════
# 模板位置: 09_error_and_safety/
# 敲完后取消下面代码的注释即可编译

# add_library(mywork_error_safety STATIC
#     src/error_safety/exceptions.cpp
#     src/error_safety/noexcept_contracts.cpp
#     src/error_safety/error_code.cpp
#     src/error_safety/expected_utils.cpp
#     src/error_safety/stacktrace.cpp
# )
# target_include_directories(mywork_error_safety PUBLIC include)
# target_link_libraries(mywork_error_safety PUBLIC lesson_utils)

# add_executable(01_exceptions_demo examples/error_safety/01_exceptions_demo.cpp)
# target_link_libraries(01_exceptions_demo PRIVATE mywork_error_safety)
# add_executable(02_noexcept_demo examples/error_safety/02_noexcept_and_contracts_demo.cpp)
# target_link_libraries(02_noexcept_demo PRIVATE mywork_error_safety)
# add_executable(03_error_code_demo examples/error_safety/03_error_code_demo.cpp)
# target_link_libraries(03_error_code_demo PRIVATE mywork_error_safety)
# add_executable(04_expected_demo examples/error_safety/04_expected_demo.cpp)
# target_link_libraries(04_expected_demo PRIVATE mywork_error_safety)
# add_executable(05_stacktrace_demo examples/error_safety/05_stacktrace_demo.cpp)
# target_link_libraries(05_stacktrace_demo PRIVATE mywork_error_safety)


# ═══════════════════════════════════════════════════════════════════════════════
# 第 5 章 — stl (STL 标准库)
# ═══════════════════════════════════════════════════════════════════════════════
# 模板位置: 05_stl/
# 学完第 5 章后取消注释

# add_library(mywork_stl STATIC
#     src/stl/sequential_containers.cpp
#     src/stl/associative_containers.cpp
#     src/stl/unordered_containers.cpp
#     src/stl/container_adaptors.cpp
#     src/stl/iterators.cpp
#     src/stl/algorithms_nonmod.cpp
#     src/stl/algorithms_mod.cpp
#     src/stl/algorithms_sort.cpp
#     src/stl/tuple_types.cpp
#     src/stl/chrono_random.cpp
#     src/stl/charconv_and_optional_monadic.cpp
# )
# target_include_directories(mywork_stl PUBLIC include)
# target_link_libraries(mywork_stl PUBLIC lesson_utils)

# add_executable(01_seq_cont_demo examples/stl/01_sequential_containers_demo.cpp)
# target_link_libraries(01_seq_cont_demo PRIVATE mywork_stl)
# add_executable(02_assoc_cont_demo examples/stl/02_associative_containers_demo.cpp)
# target_link_libraries(02_assoc_cont_demo PRIVATE mywork_stl)
# add_executable(03_unord_cont_demo examples/stl/03_unordered_containers_demo.cpp)
# target_link_libraries(03_unord_cont_demo PRIVATE mywork_stl)
# add_executable(04_adaptors_demo examples/stl/04_container_adaptors_demo.cpp)
# target_link_libraries(04_adaptors_demo PRIVATE mywork_stl)
# add_executable(05_iterators_demo examples/stl/05_iterators_demo.cpp)
# target_link_libraries(05_iterators_demo PRIVATE mywork_stl)
# add_executable(06_algo_nonmod_demo examples/stl/06_algorithms_nonmod_demo.cpp)
# target_link_libraries(06_algo_nonmod_demo PRIVATE mywork_stl)
# add_executable(07_algo_mod_demo examples/stl/07_algorithms_mod_demo.cpp)
# target_link_libraries(07_algo_mod_demo PRIVATE mywork_stl)
# add_executable(08_algo_sort_demo examples/stl/08_algorithms_sort_demo.cpp)
# target_link_libraries(08_algo_sort_demo PRIVATE mywork_stl)
# add_executable(09_tuple_opt_demo examples/stl/09_tuple_pair_optional_variant_demo.cpp)
# target_link_libraries(09_tuple_opt_demo PRIVATE mywork_stl)
# add_executable(10_chrono_demo examples/stl/10_chrono_random_demo.cpp)
# target_link_libraries(10_chrono_demo PRIVATE mywork_stl)
# add_executable(11_charconv_demo examples/stl/11_charconv_and_optional_monadic_demo.cpp)
# target_link_libraries(11_charconv_demo PRIVATE mywork_stl)


# ═══════════════════════════════════════════════════════════════════════════════
# 第 10 章 — concurrency (并发编程)
# ═══════════════════════════════════════════════════════════════════════════════
# 模板位置: 10_concurrency/
# 学完第 10 章后取消注释

# add_library(mywork_concurrency STATIC
#     src/concurrency/thread_basics.cpp
#     src/concurrency/mutex_exercises.cpp
#     src/concurrency/condition_variable_exercises.cpp
#     src/concurrency/future_promise_exercises.cpp
#     src/concurrency/atomic_exercises.cpp
#     src/concurrency/jthread_exercises.cpp
#     src/concurrency/thread_pool_exercises.cpp
# )
# target_include_directories(mywork_concurrency PUBLIC include)
# target_link_libraries(mywork_concurrency PUBLIC lesson_utils Threads::Threads)

# add_executable(concurrency_01_threads_demo examples/concurrency/01_thread_basics_demo.cpp)
# target_link_libraries(concurrency_01_threads_demo PRIVATE mywork_concurrency)
# add_executable(concurrency_02_mutex_demo examples/concurrency/02_mutex_and_lock_demo.cpp)
# target_link_libraries(concurrency_02_mutex_demo PRIVATE mywork_concurrency)
# add_executable(concurrency_03_cv_demo examples/concurrency/03_condition_variable_demo.cpp)
# target_link_libraries(concurrency_03_cv_demo PRIVATE mywork_concurrency)
# add_executable(concurrency_04_future_demo examples/concurrency/04_future_and_promise_demo.cpp)
# target_link_libraries(concurrency_04_future_demo PRIVATE mywork_concurrency)
# add_executable(concurrency_05_atomic_demo examples/concurrency/05_atomic_and_memory_demo.cpp)
# target_link_libraries(concurrency_05_atomic_demo PRIVATE mywork_concurrency)
# add_executable(concurrency_06_jthread_demo examples/concurrency/06_jthread_and_parallel_demo.cpp)
# target_link_libraries(concurrency_06_jthread_demo PRIVATE mywork_concurrency)
# add_executable(concurrency_07_thread_pool_demo examples/concurrency/07_thread_pool_demo.cpp)
# target_link_libraries(concurrency_07_thread_pool_demo PRIVATE mywork_concurrency)


# ═══════════════════════════════════════════════════════════════════════════════
# 第 6-13 章模板同上，学完一章取消注释一章。
# 你敲完所有 14 章后，这个 CMakeLists.txt 就是一个完整的工业级构建文件。
# ═══════════════════════════════════════════════════════════════════════════════

# ═══════════════════════════════════════════════════════════════════════════════
# 第 8 章 — move_lambda (可调用工具)
# ═══════════════════════════════════════════════════════════════════════════════
# 模板位置: 08_move_and_lambda/
# 学完第 8 章第 6 课后取消注释

# add_library(mywork_move_lambda STATIC
#     src/move_lambda/callable_utils.cpp
# )
# target_include_directories(mywork_move_lambda PUBLIC include)
# target_link_libraries(mywork_move_lambda PUBLIC lesson_utils)

# add_executable(mw_callable_utils_demo examples/move_lambda/06_callable_utils_demo.cpp)
# target_link_libraries(mw_callable_utils_demo PRIVATE mywork_move_lambda)


# ═══════════════════════════════════════════════════════════════════════════════
# 第 11 章练习 — cpp20_23 (C++20/23 新特性练习)
# ═══════════════════════════════════════════════════════════════════════════════
# 模板位置: 11_cpp20_cpp23/
# 学完第 11 章第 9 课后取消注释

# add_library(mywork_cpp20_23 STATIC
#     src/cpp20_23/constexpr_enhanced_practice.cpp
# )
# target_include_directories(mywork_cpp20_23 PUBLIC include)
# target_link_libraries(mywork_cpp20_23 PUBLIC lesson_utils)

# 09: constexpr 增强 — constexpr new/delete、is_constant_evaluated、constexpr 析构、std::unreachable
# add_executable(09_constexpr_practice examples/cpp20_23/09_constexpr_enhanced_practice.cpp)
# target_link_libraries(09_constexpr_practice PRIVATE mywork_cpp20_23)

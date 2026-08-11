# ------------------------------------------------------------------------------
# 10_concurrency — 并发编程 (std::thread, mutex, atomic, future, C++20 并行原语)
#
# Library:  concurrency_lib (STATIC)
#   Sources: src/concurrency_module.cpp
#   Headers: include/concurrency/thread_basics.hpp
#            include/concurrency/mutex_and_lock.hpp
#            include/concurrency/blocking_queue.hpp
#            include/concurrency/future_and_promise.hpp
#            include/concurrency/atomic_utils.hpp
#            include/concurrency/jthread_and_parallel.hpp
#
#   PUBLIC links:
#     - lesson_utils     (共享工具库, 所有模块都需要)
#     - Threads::Threads  (并发模块必需的线程库, 如 pthread / Windows threads)
#
# Examples (每个都是独立的可执行文件, 链接 concurrency_lib):
#   01_thread_basics_demo         — std::thread 基础: 创建/join/成员函数线程
#   02_mutex_and_lock_demo        — 互斥量与锁: mutex, lock_guard, 死锁避免
#   03_condition_variable_demo    — 条件变量: wait/notify, 生产者-消费者
#   04_future_and_promise_demo    — future/promise: async, 异常传播, packaged_task
#   05_atomic_and_memory_demo     — 原子操作与内存序: CAS, spinlock, memory_order
#   06_jthread_and_parallel_demo  — C++20 并行原语: jthread, latch, barrier, semaphore
# ------------------------------------------------------------------------------

# ---- 线程库 (跨平台: pthread / Windows threads) ----
find_package(Threads REQUIRED)

# ---- Library ----
add_library(concurrency_lib STATIC
    src/concurrency_module.cpp
)

target_include_directories(concurrency_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(concurrency_lib PUBLIC
    lesson_utils
    Threads::Threads
)

# ---- Examples ----

add_executable(01_thread_basics_demo examples/01_thread_basics_demo.cpp)
target_link_libraries(01_thread_basics_demo PRIVATE concurrency_lib)

add_executable(02_mutex_and_lock_demo examples/02_mutex_and_lock_demo.cpp)
target_link_libraries(02_mutex_and_lock_demo PRIVATE concurrency_lib)

add_executable(03_condition_variable_demo examples/03_condition_variable_demo.cpp)
target_link_libraries(03_condition_variable_demo PRIVATE concurrency_lib)

add_executable(04_future_and_promise_demo examples/04_future_and_promise_demo.cpp)
target_link_libraries(04_future_and_promise_demo PRIVATE concurrency_lib)

add_executable(05_atomic_and_memory_demo examples/05_atomic_and_memory_demo.cpp)
target_link_libraries(05_atomic_and_memory_demo PRIVATE concurrency_lib)

add_executable(06_jthread_and_parallel_demo examples/06_jthread_and_parallel_demo.cpp)
target_link_libraries(06_jthread_and_parallel_demo PRIVATE concurrency_lib)

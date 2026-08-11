// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  concurrency library — 模块入口 (编译验证)                                    ║
// ║                                                                              ║
// ║  大部分实现在头文件中 (模板类 + inline 函数), 此文件用于:                     ║
// ║    1. 确保所有头文件可编译 (避免遗漏依赖)                                     ║
// ║    2. 为 CMake 静态库目标提供至少一个 .cpp 源文件                             ║
// ║    3. 未来可在此放置需要显式实例化的模板, 或非 inline 的工具函数              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "concurrency/thread_basics.hpp"
#include "concurrency/mutex_and_lock.hpp"
#include "concurrency/blocking_queue.hpp"
#include "concurrency/future_and_promise.hpp"
#include "concurrency/atomic_utils.hpp"
#include "concurrency/jthread_and_parallel.hpp"

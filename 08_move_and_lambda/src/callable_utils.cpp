// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  callable_utils.cpp — 自由函数实现: square, call_with_ref, call_with_function║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "move_lambda/callable_utils.hpp"

namespace move_lambda {

// ── square — 自由函数, 用于 function_ref 包装函数指针演示 ──────────────────────
int square(int x) {
    return x * x;
}

// ── call_with_ref — 通过 function_ref 调用回调(非拥有, 零开销) ─────────────────
// function_ref 本质是 { void* obj, R(*erased_fn)(void*, Args...) }
// 相比 function 少了一次堆分配 + 类型擦除的虚函数开销
void call_with_ref(std::function_ref<int(int)> callback, int value) {
    std::println("  call_with_ref: callback({}) = {}", value, callback(value));
}

// ── call_with_function — 通过 function 调用回调(拥有所有权, 可能堆分配) ────────
// function 通过类型擦除按值存储可调用对象, 这意味着:
// 1. 可调用对象必须可拷贝(或可移动)
// 2. 如果对象超过 SBO 阈值(通常是 sizeof(void*)*2~4), 会触发堆分配
// 3. 每次调用有间接跳转开销(类似虚函数)
void call_with_function(std::function<int(int)> callback, int value) {
    std::println("  call_with_function: callback({}) = {}", value, callback(value));
}

} // namespace move_lambda

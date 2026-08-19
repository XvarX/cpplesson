#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  callable_utils.hpp — C++23 可调用工具: function_ref、move_only_function、  ║
// ║                        invocable/predicate concept 约束                     ║
// ║  提供: CopyTracker 函子、MoveOnlyGate 移动函子、通用约束模板函数            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <functional>
#include <concepts>
#include <memory>
#include <type_traits>
#include <vector>

namespace move_lambda {

// ── function_ref 可用性检测 ──────────────────────────────────────────────────
// std::function_ref (P0792) 于 C++26 进入标准。本课程按 -std=c++23 编译，
// 故用特性宏检测: 标准库有则直接 using 引入，没有则用下面的教学版回退实现
// (语义一致: 非拥有、零拷贝、可包装任意可调用对象)。
#if defined(__cpp_lib_function_ref)
using std::function_ref;
#else
// ── function_ref 教学版 (C++23 回退) ─────────────────────────────────────────
// 本质: { void* 对象指针, void(*调用桩)(void*, Args...) } 两个指针的"视图"
// 不拥有对象、不堆分配 — 与 std::function (类型擦除+可能堆分配) 的关键区别
template <typename Sig>
class function_ref;  // 主模板不定义 — 只支持函数签名特化

template <typename R, typename... Args>
class function_ref<R(Args...)> {
    void* obj_ = nullptr;
    R (*invoke_)(void*, Args...) = nullptr;

    template <typename F>
    static R invoke_stub(void* o, Args... args) {
        // 把 void* 还原为可调用对象的引用再调用 (不拷贝!)
        // 注意: F 可能是函数引用 — void* 与函数指针之间只能 reinterpret_cast
        return (*reinterpret_cast<std::add_pointer_t<F>>(o))(std::forward<Args>(args)...);
    }

public:
    template <typename F>
        requires std::invocable<F&, Args...>
              && std::is_convertible_v<std::invoke_result_t<F&, Args...>, R>
              // 拒绝右值临时量 — function_ref 不延长生命周期, 绑定临时量必悬垂
              && (!std::is_rvalue_reference_v<F&&>)
              && (!std::same_as<std::decay_t<F>, function_ref>)
    function_ref(F&& f) noexcept
        : obj_(reinterpret_cast<void*>(std::addressof(f))),
          invoke_(&invoke_stub<F>) {}

    R operator()(Args... args) const {
        return invoke_(obj_, std::forward<Args>(args)...);
    }
};
#endif // __cpp_lib_function_ref

// ═══════════════════════════════════════════════════════════════════════════════
// CopyTracker — 追踪拷贝/移动的函子, 用于对比 function_ref 与 function 开销
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  一个重载 operator() 的可调用对象, 每次拷贝/移动都会打印日志
// WHY:   直观展示 std::function 按值存储时会产生拷贝, 而 function_ref 不拷贝
// WHEN:   教学演示, 理解 function_ref 的"非拥有"含义
struct CopyTracker {
    int id;
    inline static int copy_count = 0;
    inline static int move_count = 0;

    explicit CopyTracker(int id_ = 0) : id(id_) {}

    CopyTracker(const CopyTracker& other) : id(other.id) {
        ++copy_count;
        std::println("  [CopyTracker] 拷贝构造 id={}  (总拷贝次数={})", id, copy_count);
    }

    CopyTracker(CopyTracker&& other) noexcept : id(other.id) {
        other.id = -1;
        ++move_count;
        std::println("  [CopyTracker] 移动构造 id={}  (总移动次数={})", id, move_count);
    }

    CopyTracker& operator=(const CopyTracker&) = delete;
    CopyTracker& operator=(CopyTracker&&) = delete;

    int operator()(int x) const {
        std::println("  CopyTracker<id={}>::operator()({}) = {}", id, x, x * id);
        return x * id;  // 返回 int — 使其满足 function<int(int)> / function_ref<int(int)>
    }

    static void reset_counts() { copy_count = 0; move_count = 0; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Counter — 简单的有状态计数器函子(可拷贝), 用于泛型约束演示
// ═══════════════════════════════════════════════════════════════════════════════
struct Counter {
    int count = 0;
    int operator()(int x) {
        count += x;
        return count;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// 自由函数, 用于演示 function_ref 包装函数指针
// ═══════════════════════════════════════════════════════════════════════════════
int square(int x);

// ═══════════════════════════════════════════════════════════════════════════════
// call_with_ref — 使用 function_ref 作为参数(非拥有, 零开销)
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  接受 function_ref<int(int)>, 轻量级回调参数
//        (C++26 用标准库 std::function_ref; C++23 用本文件的教学版回退)
//        不接管所有权, 不产生堆分配, 不要求可调用对象可拷贝
// WHY:   热路径回调的首选 — function_ref ≈ 裸函数指针, 但能包装任何可调用对象
// WHEN:   函数参数是回调且你不需要存储它(调用后即丢弃)
void call_with_ref(function_ref<int(int)> callback, int value);

// ═══════════════════════════════════════════════════════════════════════════════
// call_with_function — 对比: 使用 std::function 作为参数(拥有所有权)
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  接受 std::function<int(int)>, 拥有可调用对象的所有权
// WHY:   对比 function_ref 的开销差异
void call_with_function(std::function<int(int)> callback, int value);

// ═══════════════════════════════════════════════════════════════════════════════
// 约束模板: 使用 std::invocable 约束可调用参数
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::invocable<F, Args...> concept: 检查 F 是否可以用 Args... 调用
// WHY:   编译期约束, 提供清晰的错误信息, 替代 SFINAE 的晦涩写法
// WHEN:   模板函数接受可调用对象参数时, 用 concept 替代 enable_if
template <std::invocable<int> F>
int apply_twice(F&& f, int x) {
    return f(f(x));
}

// ═══════════════════════════════════════════════════════════════════════════════
// 约束模板: 使用 std::predicate 约束谓词参数
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::predicate<F, Args...> = invocable + 返回值可转为 bool
// WHY:   确保算法传入的谓词类型安全, 编译期捕获错误
// WHEN:   编写需要布尔判断的泛型算法(find_if, remove_if, sort 等)
template <std::predicate<int> P>
int count_if_custom(const std::vector<int>& data, P&& pred) {
    int result = 0;
    for (int val : data) {
        if (std::invoke(std::forward<P>(pred), val)) {
            ++result;
        }
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 约束模板: 使用 std::relation 约束二元关系
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::relation<F, T, U> 约束二元谓词(用于排序、比较等)
// WHY:   确保比较函数签名正确, 编译期而不是运行时报错
template <typename T, std::relation<T, T> R>
bool is_ordered(const T& a, const T& b, R&& rel) {
    return std::invoke(std::forward<R>(rel), a, b);
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// ┌─────────────────────────────────────────────────────────────────────────┐
// │ 陷阱1: function_ref 悬垂 — 被引用的可调用对象销毁后, function_ref 悬垂  │
// │   ✅ auto ref = std::function_ref<int(int)>{local_lambda};              │
// │   ❌ return std::function_ref<int(int)>{local_lambda}; // 返回后悬垂!   │
// │                                                                         │
// │ 陷阱2: move_only_function 只能移动不能拷贝                               │
// │   ✅ auto f2 = std::move(f1);                                          │
// │   ❌ auto f2 = f1;  // 编译错误!                                        │
// │                                                                         │
// │ 陷阱3: concept 检查的是可调用性, 不检查语义正确性                         │
// │   std::predicate 只检查返回 bool, 但不管实际逻辑是否正确                  │
// │                                                                         │
// │ 陷阱4: function_ref 不能存储(非拥有, 不管理生命周期)                     │
// │   如果要存储回调, 请用 std::function 或 std::move_only_function         │
// └─────────────────────────────────────────────────────────────────────────┘

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 写一个函数, 参数用 function_ref<void(int)>, 观察它对 Lambda/函数指针/函子的调用
// 2. 创建一个捕获 unique_ptr 的 Lambda, 用 move_only_function 存储它
// 3. 用 std::invocable 约束一个模板函数, 故意传入不可调用类型, 观察编译错误信息
// 4. 用 std::predicate 实现一个 filter 模板函数, 对比无约束版本的错误信息质量
// 5. 对比 function、function_ref、move_only_function 在以下场景的适用性:
//    a) 异步回调(需存储)  b) 同步访问(不存储)  c) 移动-only 回调

} // namespace move_lambda

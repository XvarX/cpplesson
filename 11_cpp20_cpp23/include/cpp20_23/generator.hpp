#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: 协程基础 — co_await/co_yield/co_return、promise_type、generator      ║
// ║  目标: 理解协程的核心组件，实现简易 generator<T>，掌握 awaiter 概念          ║
// ║                                                                            ║
// ║  协程三要素:                                                                 ║
// ║  1. co_await  — 挂起当前协程，等待异步操作完成                               ║
// ║  2. co_yield  — 挂起并产出一个值，协程可恢复继续执行 (generator 模式)        ║
// ║  3. co_return — 返回最终值并结束协程                                         ║
// ║                                                                            ║
// ║  每个协程关联一个 promise_type，编译器用它控制协程生命周期。                  ║
// ║  协程是不可拷贝的 (浅层) — 它们是状态机，移动可以，拷贝无意义。              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <coroutine>     // C++20 协程基础设施
#include <exception>
#include <utility>

// ============================================================================
// 实现一个简易 generator<T> — 理解 promise_type
// ============================================================================
// generator<T> 是最常见的协程模式，类似 Python 的 yield。
// 核心组件: promise_type (控制协程)、handle (操控协程)、iterator (遍历结果)

template<typename T>
struct Generator {
    // ── promise_type: 编译器要求协程的返回类型必须嵌套此名称 ──
    struct promise_type {
        T current_value;  // 当前 yield 的值

        // 协程开始时调用
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // 初始挂起点: 协程创建后立即挂起 (惰性启动)
        std::suspend_always initial_suspend() noexcept { return {}; }

        // 最终挂起点: 协程结束后挂起 (保持 handle 有效以便读取结果)
        std::suspend_always final_suspend() noexcept { return {}; }

        // 处理 co_yield: 保存值并挂起
        std::suspend_always yield_value(T value) noexcept {
            current_value = std::move(value);
            return {};
        }

        // 处理 co_return (void)
        void return_void() noexcept {}

        // 处理未捕获异常
        void unhandled_exception() { std::terminate(); }
    };

    using Handle = std::coroutine_handle<promise_type>;

    // ── Generator 自身结构 ──
    Handle handle;

    explicit Generator(Handle h) : handle(h) {}

    // 禁止拷贝 (协程状态唯一)
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    // 允许移动
    Generator(Generator&& other) noexcept : handle(std::exchange(other.handle, {})) {}
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle) handle.destroy();
            handle = std::exchange(other.handle, {});
        }
        return *this;
    }

    ~Generator() { if (handle) handle.destroy(); }

    // ── 迭代支持: 让 range-for 可用 ──
    struct iterator {
        Handle handle;
        bool operator!=(std::default_sentinel_t) const {
            return handle && !handle.done();
        }
        iterator& operator++() {
            handle.resume();          // 恢复协程执行到下一个 co_yield
            return *this;
        }
        T operator*() const {
            return handle.promise().current_value;
        }
    };

    iterator begin() {
        handle.resume();              // 启动协程，执行到第一个 co_yield
        return {handle};
    }

    std::default_sentinel_t end() { return {}; }

    // 查询是否已完成
    bool done() const { return handle.done(); }
};

// ============================================================================
// Awaiter 概念 — co_await 背后的机制
// ============================================================================
// co_await 操作的对象称为 awaiter，需要实现三个方法:
//   await_ready()   → 如果已完成，不挂起直接继续
//   await_suspend() → 挂起时调用，可用来注册回调
//   await_resume()  → 恢复时调用，返回 co_await 的结果值
//
// std::suspend_always / std::suspend_never 是标准库提供的简单 awaiter。

#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  主题: constexpr 增强 — constexpr new/delete、is_constant_evaluated、        ║
// ║        constexpr 析构函数、std::unreachable                                  ║
// ║  目标: 掌握编译期动态内存分配、常量求值检测、编译期资源管理、不可达标记       ║
// ║                                                                            ║
// ║  constexpr new/delete (C++20): 编译期动态内存分配                             ║
// ║  ────────────────────────────────────────────────────────                    ║
// ║  C++20 允许在 constexpr 上下文中使用 new/delete，但有限制:                    ║
// ║  1. 编译期分配的内存必须在同一常量求值期间被 delete 释放                       ║
// ║  2. 编译期 new 返回的指针不能"逃逸"到运行时                                    ║
// ║  3. 本质上编译期 new 使用了一种非堆的临时存储                                  ║
// ║                                                                            ║
// ║  std::is_constant_evaluated() (C++20): 编译期/运行时分支                      ║
// ║  ────────────────────────────────────────────────────                        ║
// ║  在 constexpr 函数中检测当前是否处于常量求值上下文。                           ║
// ║  允许同一函数在编译期和运行时采用不同实现策略。                                ║
// ║                                                                            ║
// ║  constexpr 析构函数 (C++20): 编译期资源释放                                  ║
// ║  ────────────────────────────────────────────                                ║
// ║  C++20 允许在 constexpr 上下文中使用非平凡析构函数。                           ║
// ║  使得 constexpr 对象可以使用 RAII，在编译期自动管理资源。                     ║
// ║                                                                            ║
// ║  std::unreachable() (C++23): 标记不可达代码路径                              ║
// ║  ────────────────────────────────────────────                                ║
// ║  用于告知编译器某个代码路径永远不可达。                                       ║
// ║  - 编译期: 如果执行到则产生编译错误                                           ║
// ║  - 运行时: 如果执行到则是未定义行为 (UB)，编译器可据此优化                     ║
// ║                                                                            ║
// ║  WHY:                                                                       ║
// ║  1. constexpr new/delete → 编译期就能构建复杂数据结构 (如 vector, string)    ║
// ║  2. is_constant_evaluated → 编译期用快速算法，运行时用安全算法                ║
// ║  3. constexpr 析构函数 → 让编译期 RAII 成为可能，推动 constexpr 生态          ║
// ║  4. std::unreachable → 帮助编译器生成更优代码，消除虚假的 "可能未初始化" 警告 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <new>           // std::construct_at, std::destroy_at, 全局 operator new
#include <memory>        // std::allocator, std::construct_at (C++20), std::destroy_at
#include <type_traits>   // std::is_scalar, std::is_constant_evaluated
#include <utility>       // std::unreachable (C++23)
#include <string>
#include <vector>
#include <array>
#include <print>

namespace cpp20_23 {

// ============================================================================
// Part 1: constexpr new/delete (C++20) — 编译期动态内存分配
// ============================================================================
// C++20 的核心突破: 允许在常量求值中使用 new/delete 表达式。
// 过去 constexpr 只能处理栈上的值，现在可以在编译期动态分配内存。
//
// 关键限制:
//   - 编译期 new 的内存必须在同一常量求值中 delete
//   - 不能把编译期分配的指针传递到运行时
//   - 编译器内部使用特殊存储机制 (非堆内存) 来实现

/// 编译期动态分配数组并计算其和
/// 演示: constexpr 函数中使用 new[] 和 delete[]
constexpr int compile_time_array_sum(int n) {
    // C++20: 在 constexpr 中使用 new[]
    int* data = new int[n];
    for (int i = 0; i < n; ++i) {
        data[i] = i * i;  // 填充平方数
    }
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += data[i];
    }
    // C++20: 必须在同一 constexpr 求值中 delete[] — 不能"逃逸"到运行时
    delete[] data;
    return sum;
}

/// 编译期使用 operator new + placement 构造对象
/// 演示: 编译期 std::construct_at / std::destroy_at (C++20)
constexpr int compile_time_object_lifetime() {
    // C++20: 在编译期使用 operator new
    // allocate raw memory
    void* raw = ::operator new(sizeof(int));
    // placement new / construct_at
    auto* p = std::construct_at(static_cast<int*>(raw), 42);
    int result = *p * 2;
    std::destroy_at(p);       // C++20: constexpr destroy_at
    ::operator delete(raw);   // C++20: constexpr operator delete
    return result;
}

// ============================================================================
// Part 2: std::is_constant_evaluated() (C++20) — 编译期/运行时分支
// ============================================================================
// 在 constexpr 函数中检测当前是否处于常量求值上下文。
// 这是 C++20 中唯一能在 constexpr 函数体内判断"现在是不是在编译期"的标准工具。
//
// ⚠️ 常见误用: 不要用 if constexpr (std::is_constant_evaluated())
//    is_constant_evaluated() 是运行时条件，不是编译期常量!
//    应该用普通的 if (std::is_constant_evaluated()) {...}
//
// WHEN: 当编译期可以做更快但可能不安全的优化，运行时期望安全行为时。
// 典型场景:
//   - 编译期: 用位运算快速计算 (不需要担心非可移植行为)
//   - 运行时: 用标准库函数或更安全的缓慢实现

/// 演示 is_constant_evaluated 的正确用法
/// 编译期用快速算法，运行时用安全算法
constexpr double fast_or_safe_sqrt(double x) {
    if (std::is_constant_evaluated()) {
        // 编译期: 牛顿迭代法求平方根 — 慢但能在编译期执行
        // 注意: 编译期不能调用 std::sqrt (它不是 constexpr in C++20)
        if (x <= 0.0) return 0.0;
        double guess = x;
        for (int i = 0; i < 20; ++i) {
            guess = (guess + x / guess) * 0.5;
        }
        return guess;
    } else {
        // 运行时: 直接调用硬件加速的 std::sqrt
        return x <= 0.0 ? 0.0 : x / 2.0;  // 简化版 (实际会用 std::sqrt)
    }
}

/// is_constant_evaluated 典型模式: 提供编译期和运行时两个版本
constexpr size_t bit_ceil_compile_or_runtime(size_t x) {
    if (std::is_constant_evaluated()) {
        // 编译期实现: 手动计算大于等于 x 的最小 2 的幂
        if (x <= 1) return 1;
        size_t result = 1;
        while (result < x) result <<= 1;
        return result;
    } else {
        // 运行时: 使用高效的 std::bit_ceil (C++20 标准库)
        // return std::bit_ceil(x);
        // 这里提供手动实现作为演示
        if (x <= 1) return 1;
        size_t result = 1;
        while (result < x) result <<= 1;
        return result;
    }
}

// ============================================================================
// Part 3: constexpr 析构函数 (C++20) — 编译期 RAII
// ============================================================================
// C++20 允许在 constexpr 上下文中调用非平凡析构函数。
// 这使得 constexpr 对象可以在编译期使用 RAII 模式管理资源。
//
// WHY: 在 C++17 中，constexpr 构造的对象必须具有平凡析构函数。
//      这意味着不能在编译期使用 std::vector、std::string 等有析构函数的容器。
//      C++20 改变了这一点——只要析构函数满足 constexpr 要求即可。

/// 编译期 RAII 资源句柄: 演示 constexpr 构造 + constexpr 析构
/// 模拟一个在编译期管理动态内存的资源类
class ConstexprBuffer {
public:
    constexpr explicit ConstexprBuffer(size_t size)
        : size_(size)
        , data_(new int[size]{})  // C++20: constexpr new[]
    {}

    // C++20: constexpr 析构函数 — 编译期自动清理资源!
    constexpr ~ConstexprBuffer() {
        delete[] data_;  // C++20: constexpr delete[]
    }

    // 禁止拷贝 (管理所有权)
    ConstexprBuffer(const ConstexprBuffer&) = delete;
    ConstexprBuffer& operator=(const ConstexprBuffer&) = delete;

    // 支持移动 (允许编译期 RAII 转移)
    constexpr ConstexprBuffer(ConstexprBuffer&& other) noexcept
        : size_(other.size_)
        , data_(other.data_)
    {
        other.size_ = 0;
        other.data_ = nullptr;
    }
    constexpr ConstexprBuffer& operator=(ConstexprBuffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            data_ = other.data_;
            other.size_ = 0;
            other.data_ = nullptr;
        }
        return *this;
    }

    constexpr int& operator[](size_t i) { return data_[i]; }
    constexpr const int& operator[](size_t i) const { return data_[i]; }
    constexpr size_t size() const { return size_; }
    constexpr int* begin() { return data_; }
    constexpr int* end() { return data_ + size_; }

private:
    size_t size_;
    int* data_;
};

/// 编译期使用 ConstexprBuffer (RAII 自动清理)
constexpr int compile_time_raii_sum() {
    ConstexprBuffer buf(10);
    for (size_t i = 0; i < buf.size(); ++i) {
        buf[i] = static_cast<int>(i + 1);
    }
    // 离开作用域时自动调用 ~ConstexprBuffer() 释放内存
    // C++20: 这个析构在编译期也可以工作!
    int total = 0;
    for (auto v : buf) total += v;
    return total;
}

/// 编译期 vector 的极简实现 — 演示 constexpr 构造/析构/赋值
/// 展示 C++20 让容器在编译期变得可能的核心机制
template<typename T>
class ConstexprVector {
public:
    constexpr ConstexprVector() = default;

    constexpr explicit ConstexprVector(size_t count) {
        data_ = new T[count]{};  // C++20: constexpr new[]
        size_ = count;
        capacity_ = count;
    }

    // C++20: constexpr 析构
    constexpr ~ConstexprVector() {
        delete[] data_;  // C++20: constexpr delete[]
    }

    // 移动语义支持
    constexpr ConstexprVector(ConstexprVector&& other) noexcept
        : data_(other.data_)
        , size_(other.size_)
        , capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    constexpr ConstexprVector& operator=(ConstexprVector&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    // 禁止拷贝
    ConstexprVector(const ConstexprVector&) = delete;
    ConstexprVector& operator=(const ConstexprVector&) = delete;

    constexpr void push_back(const T& value) {
        if (size_ >= capacity_) {
            size_t new_cap = capacity_ == 0 ? 4 : capacity_ * 2;
            T* new_data = new T[new_cap]{};
            for (size_t i = 0; i < size_; ++i) new_data[i] = data_[i];
            delete[] data_;
            data_ = new_data;
            capacity_ = new_cap;
        }
        data_[size_++] = value;
    }

    constexpr T& operator[](size_t i) { return data_[i]; }
    constexpr const T& operator[](size_t i) const { return data_[i]; }
    constexpr size_t size() const { return size_; }
    constexpr size_t capacity() const { return capacity_; }
    constexpr T* begin() { return data_; }
    constexpr T* end() { return data_ + size_; }
    constexpr const T* begin() const { return data_; }
    constexpr const T* end() const { return data_ + size_; }

private:
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

/// 编译期使用 ConstexprVector (演示 constexpr 容器)
constexpr auto compile_time_vector_use() {
    ConstexprVector<int> vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    vec.push_back(40);
    vec.push_back(50);

    int sum = 0;
    for (size_t i = 0; i < vec.size(); ++i) sum += vec[i];
    // ~ConstexprVector 自动释放 — 编译期 RAII
    return std::pair{sum, vec.size()};
}

// ============================================================================
// Part 4: std::unreachable() (C++23) — 标记不可达代码路径
// ============================================================================
// C++23 引入 std::unreachable() 作为标记不可达代码的标准方式。
//
// 之前的方式 (各有问题):
//   - __builtin_unreachable() — GCC/Clang 专有，不可移植
//   - __assume(false)          — MSVC 专有
//   - assert(false)            — 在 release 模式下被移除
//
// WHY std::unreachable():
//   1. 告诉编译器 "这段代码永远不会执行"，帮助优化器消除死代码
//   2. 消除编译器的 "控制流到达非 void 函数末尾" 警告
//   3. 消除 "变量可能未初始化" 的虚假警告
//   4. 如果实际执行到 — UB，宁可 crash 也不要静默执行错误逻辑
//
// WHEN:
//   - switch 的 exhaustive match 后
//   - 枚举值的 default 分支 (所有已知值都已处理)
//   - 逻辑上不可能的分支 (如前置条件已排除)

/// 演示: 使用 std::unreachable() 消除编译器警告
/// 场景: switch 穷举了所有值，default 不可达
constexpr int exhaustive_switch(int n) {
    // 假设 n 只能是 1, 2, 3 (通过前置条件保证)
    switch (n) {
    case 1: return 10;
    case 2: return 20;
    case 3: return 30;
    default: std::unreachable();  // 告诉编译器: 走不到这里!
    }
}

/// 演示: 标记逻辑上不可能的 else 分支
/// 场景: 前置条件已经排除某些值
constexpr int positive_only(int x) {
    if (x > 0) {
        return x * 2;
    } else if (x == 0) {
        return 1;
    } else {
        // 函数契约要求 x >= 0，走到这里说明调用者破坏了契约
        std::unreachable();  // C++23: 标记为不可达
    }
}

/// 演示: std::unreachable() 在 switch 上的优化效果
/// 当编译器知道某些分支不可达时，可以生成更紧凑的跳转表
constexpr int lookup_or_default(int code) {
    switch (code) {
    case 0:  return 100;
    case 1:  return 200;
    case 2:  return 300;
    case 3:  return 400;
    case 4:  return 500;
    // 假设 code 合法范围是 [0, 4]
    default: std::unreachable();
    }
}

// ============================================================================
// Part 5: 综合演练 — 编译期 JSON 解析器草图
// ============================================================================
// 利用 C++20 constexpr 增强特性，可以在编译期构建复杂的数据结构。
// 这是 constexpr 最激动人心的应用方向之一。

/// 编译期字符串拼接: 使用 constexpr new/delete
constexpr auto compile_time_concat(std::string_view a, std::string_view b) {
    size_t len = a.size() + b.size();
    char* buf = new char[len + 1]{};
    for (size_t i = 0; i < a.size(); ++i) buf[i] = a[i];
    for (size_t i = 0; i < b.size(); ++i) buf[a.size() + i] = b[i];
    // 返回一个 array (固定大小，可以在编译期传递到运行时)
    // ⚠️ 不能直接返回 buf (那会在编译期"逃逸")
    // 这里演示的是编译期构造 + 传递结果的思想
    delete[] buf;
    return len;  // 编译器可以计算这个结果
}

}  // namespace cpp20_23

// ============================================================================
// 常见陷阱
// ============================================================================
// 1. constexpr new 的内存必须"完全在编译期"处理
//    编译期 new 的指针不能通过返回值、参数引用、全局变量传递到运行时
//    否则编译错误: "allocation performed here was not deallocated"
//
// 2. 不要把 if constexpr 和 is_constant_evaluated 混用
//    if constexpr (std::is_constant_evaluated())  // ✗ 错误! 永远为 true
//    if (std::is_constant_evaluated())            // ✓ 正确
//    原因: is_constant_evaluated() 是函数调用，不是编译期常量表达式
//          if constexpr 的条件必须是编译期常量，所以 is_constant_evaluated()
//          在 if constexpr 中始终被视为 true
//
// 3. constexpr 析构函数不会自动让类成为 literal type
//    类还需要满足其他 literal type 要求:
//    - 所有非静态成员必须是 literal type
//    - 必须至少有一个 constexpr 构造函数
//
// 4. std::unreachable() 不是防御性编程的替代品
//    它告诉编译器 "这里不可达"，但运行时若真执行到就是 UB。
//    不应该用 std::unreachable() 来替代异常/错误处理。
//    正确用法: 只在逻辑上真的不可能达到的地方使用。
//
// 5. 编译器对 constexpr new 的内存限制
//    编译期 new 使用的是编译器的虚拟内存，不同编译器有不同的上限。
//    大量的编译期分配可能导致编译时间显著增加或编译失败。
//
// 6. 跨翻译单元的 constexpr new/delete
//    编译期 new 和 delete 必须在同一个常量表达式中完成。
//    不能在 constexpr 函数 A 中 new，在 constexpr 函数 B 中 delete
//    (如果它们不是同一个完整表达式的组成部分的话)。

// ============================================================================
// 练习
// ============================================================================
// 1. 实现一个编译期链表 (Node* 用 constexpr new 分配)
//    链表的构造、遍历、销毁必须在同一个 constexpr 函数中完成
//    验证链表在编译期的计算结果正确
//
// 2. 用 is_constant_evaluated() 实现一个通用的 pow() 函数
//    编译期用朴素乘法循环，运行时调用 std::pow
//    对比两种实现在不同场景下的行为
//
// 3. 用 ConstexprVector 实现一个编译期排序算法
//    不依赖标准库 (std::sort 在 C++20 中已是 constexpr)
//    用编译期 assert 验证排序结果
//
// 4. 设计一个编译期 RAII 锁 (模拟)
//    在构造时打印 "Acquired"，在析构时打印 "Released"
//    验证在 constexpr 上下文中析构函数确实被调用了
//
// 5. 用 std::unreachable() 改进现有的 switch-case 代码
//    找到一个枚举类型的所有值都被处理的 switch 语句
//    把 default: 替换为 std::unreachable()
//    观察编译器警告是否消失

#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  specialization.hpp — 模板特化（全特化、偏特化、变量模板、traits 模式）      ║
// ║  可复用组件: TypeName<T> + 特化, TypeTraits<T> + 偏特化, pi<T>,            ║
// ║              is_void_v<T>, is_string_like<T>, smart_print<T>               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <type_traits>

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: 全特化 (Full/Explicit Specialization)
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】为某个具体的模板实参组合提供一个"完全定制版"的实现。
// 【为什么】通用模板对大多数类型都适用，但有些类型需要特殊处理。
//          比如 std::vector<bool> 就做了全特化以节省内存。
// 【什么时候用】当某个特定类型的实现逻辑与通用版本完全不同时。

// 通用模板
template <typename T>
struct TypeName {
    static std::string name() { return "未知类型"; }
};

// 全特化：为 int 定制
template <>
struct TypeName<int> {
    static std::string name() { return "int (整数)"; }
};

// 全特化：为 double 定制
template <>
struct TypeName<double> {
    static std::string name() { return "double (双精度浮点)"; }
};

// 全特化：为 std::string 定制
template <>
struct TypeName<std::string> {
    static std::string name() { return "std::string (字符串)"; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 偏特化 (Partial Specialization)
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】不是特化到具体类型，而是特化"某类类型"：指针、引用、const 等。
// 【为什么】你不可能为 int*、double*、string*... 每个指针都全特化一遍。
//          偏特化让你一次性处理"所有指针类型"。
// 【什么时候用】需要区别对待指针/非指针、const/非const、容器等模式时。

// 通用模板：打印类型描述
template <typename T>
struct TypeTraits {
    static void print() {
        std::println("类型: {} (大小: {} 字节)", typeid(T).name(), sizeof(T));
    }
};

// 偏特化：针对所有指针类型 T*
template <typename T>
struct TypeTraits<T*> {
    static void print() {
        std::println("指针类型: {}* (大小: {} 字节, 指向 {} 字节)",
                     typeid(T).name(), sizeof(T*), sizeof(T));
    }
};

// 偏特化：针对所有 const 类型
template <typename T>
struct TypeTraits<const T> {
    static void print() {
        std::println("const 类型: const {} (大小: {} 字节)", typeid(T).name(), sizeof(T));
    }
};

// 【重要规则】偏特化只适用于类模板，函数模板不支持偏特化！
// 函数模板只能全特化，或者用重载来模拟偏特化效果。

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: C++14 变量模板 (Variable Template)
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】模板化常量/变量——模板参数决定值。
// 【为什么】不用每次都写 traits<T>::value，直接写 pi<double> 更简洁。
// 【什么时候用】数学常量、配置值、编译期标志位等。

template <typename T>
constexpr T pi = T(3.14159265358979323846L);

// 变量模板也可以特化
template <>
constexpr int pi<int> = 3;  // int 版本的 pi 取整

// 实用例子：is_void 的变量模板简化版 (C++17 起，标准库也有 std::is_void_v)
template <typename T>
constexpr bool is_void_v = std::is_void<T>::value;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: Traits 类模板 — 模板元编程的经典模式
// ═══════════════════════════════════════════════════════════════════════════════
// 【是什么】通过特化来查询或修改类型的"属性"。
// 【为什么】编译期获取类型信息，避免运行时开销。标准库 <type_traits> 全用这个模式。
// 【经典模式】(1) 定义一个 primary template，(2) 偏特化或全特化来定制。

// 自定义 traits：判断类型是否为"字符串类型"（string 或 const char*）
template <typename T>
struct is_string_like : std::false_type {};  // 默认: 不是

template <>
struct is_string_like<std::string> : std::true_type {};

template <>
struct is_string_like<const char*> : std::true_type {};

template <typename T>
constexpr bool is_string_like_v = is_string_like<T>::value;

// 利用 traits 做编译期分支
template <typename T>
void smart_print(const T& val) {
    if constexpr (is_string_like_v<T>) {
        std::println("📝 字符串: \"{}\"", val);
    } else if constexpr (std::is_arithmetic_v<T>) {
        std::println("🔢 数值: {}", val);
    } else {
        std::println("📦 其他类型");
    }
}

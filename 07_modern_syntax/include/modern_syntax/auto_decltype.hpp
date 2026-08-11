#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  modern_syntax/auto_decltype.hpp                                            ║
// ║  auto / decltype / decltype(auto) / 尾置返回类型 —— 可复用模板函数          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <vector>
#include <type_traits>

namespace modern_syntax {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  auto 返回会"丢掉"引用 —— 危险的例子                                         ║
// ║  auto 推导为 T，不是 T& —— 返回的是拷贝！                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
template<typename T>
auto dangerous_get_element(std::vector<T>& v, size_t idx) {
    return v[idx];      // auto 推导为 T，不是 T&！—— 返回的是拷贝！
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  decltype(auto) 正确保留引用                                                  ║
// ║  decltype(auto) 推导为 T& —— 返回引用，零拷贝！                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
template<typename T>
decltype(auto) safe_get_element(std::vector<T>& v, size_t idx) {
    return v[idx];      // decltype(auto) 推导为 T& —— 返回引用，零拷贝！
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  尾置返回类型 (trailing return type)                                          ║
// ║  WHAT: auto func(args) -> ReturnType { ... }                                 ║
// ║  典型场景: 两个不同类型的加法，返回类型应该是什么？                          ║
// ║  使用 decltype + 参数名来声明依赖参数的返回类型                               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
template<typename T, typename U>
auto add(T&& t, U&& u) -> decltype(t + u) {   // 尾置返回类型
    return t + u;
}

// C++14 之后可以简化，但尾置返回类型在读代码时更清晰
template<typename T, typename U>
auto multiply(T a, U b) {   // 编译器自己推导
    return a * b;
}

} // namespace modern_syntax

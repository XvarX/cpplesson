#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  modern_syntax/structured_bindings.hpp                                      ║
// ║  结构化绑定 (C++17) —— 可复用类型: Person / PrivatePerson                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <string>
#include <tuple>
#include <utility>

namespace modern_syntax {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Person —— 聚合类型，公有成员，天然支持结构化绑定                           ║
// ║  auto [name, age, salary] = person;                                         ║
// ║  注意: 只有公有、非静态成员的聚合类型才能用结构化绑定                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
struct Person {
    std::string name;
    int         age;
    double      salary;
};

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  PrivatePerson —— 私有成员类，需要实现 tuple 协议才能支持结构化绑定         ║
// ║  实现方式: 添加 get<I>() 成员函数模板                                       ║
// ║  注意: 还需要特化 std::tuple_size 和 std::tuple_element (见下方)            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
class PrivatePerson {
public:
    PrivatePerson(std::string n, int a) : name_(std::move(n)), age_(a) {}

    // 实现 tuple 协议让它可以支持结构化绑定 (高级用法)
    template<size_t I>
    decltype(auto) get() const {
        if constexpr (I == 0) return (name_);
        else if constexpr (I == 1) return (age_);
    }

    template<size_t I>
    decltype(auto) get() {
        if constexpr (I == 0) return (name_);
        else if constexpr (I == 1) return (age_);
    }

private:
    std::string name_;
    int age_;
};

} // namespace modern_syntax

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  特化 std::tuple_size 和 std::tuple_element 以支持 PrivatePerson 的结构化   ║
// ║  绑定。这是让非聚合类型支持 auto [a, b] = obj; 的标准方式。                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

template<>
struct std::tuple_size<modern_syntax::PrivatePerson>
    : std::integral_constant<size_t, 2> {};

template<>
struct std::tuple_element<0, modern_syntax::PrivatePerson> {
    using type = std::string;
};

template<>
struct std::tuple_element<1, modern_syntax::PrivatePerson> {
    using type = int;
};

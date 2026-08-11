// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  deducing_this 实现: ValueInspector 的值类别感知方法                         ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "cpp20_23/deducing_this.hpp"
#include <print>

void ValueInspector::inspect(this const ValueInspector& self) {
    std::println("    [const&] 绑定到左值: {}", self.name);
}

void ValueInspector::inspect(this ValueInspector&& self) {
    std::println("    [&&] 绑定到右值: 可以移动 {}", self.name);
}

void ValueInspector::modify(this ValueInspector& self, std::string_view new_name) {
    std::println("    [&] 修改 '{}' → '{}'", self.name, new_name);
    self.name = new_name;
}

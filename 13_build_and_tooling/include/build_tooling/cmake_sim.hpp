#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  build_tooling/cmake_sim.hpp — 用 C++ 类模拟 CMake target 属性传递          ║
// ║  主题: PUBLIC / PRIVATE / INTERFACE 的语义区别                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <string>
#include <vector>

namespace build_tooling {

// ── Target — 模拟 CMake 的 target 及其 include 属性传递 ────────────────────────
// 是什么: 一个 CMake target (库/可执行文件) 的最小模拟，带三种 include 属性
// 为什么: CMake 的依赖是"图"结构，属性是否传递给下游取决于 PUBLIC/PRIVATE/INTERFACE
// 什么时候用: 理解 target_include_directories / target_link_libraries 的传递规则
class Target {
public:
    explicit Target(std::string n);

    std::string name;

    // 三种属性 (对应 target_include_directories 的三个关键字):
    std::vector<std::string> iface_includes;  // INTERFACE: 只传给下游, 自己不用
    std::vector<std::string> pub_includes;    // PUBLIC:    自己用, 也传给下游
    std::vector<std::string> priv_includes;   // PRIVATE:   只自己用, 不传递

    // 下游链接本 target 后能继承到的 include = PUBLIC + INTERFACE
    std::vector<std::string> inherited_includes() const;

    // 本 target 自己编译时可见的 include = PUBLIC + PRIVATE
    std::vector<std::string> my_includes() const;
};

} // namespace build_tooling

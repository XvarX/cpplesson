// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/cmake_sim.cpp — Target 类的方法实现                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "build_tooling/cmake_sim.hpp"

namespace build_tooling {

Target::Target(std::string n) : name(std::move(n)) {}

std::vector<std::string> Target::inherited_includes() const {
    // PUBLIC + INTERFACE 会传递给下游; PRIVATE 不会
    std::vector<std::string> result = iface_includes;
    result.insert(result.end(), pub_includes.begin(), pub_includes.end());
    return result;
}

std::vector<std::string> Target::my_includes() const {
    // 我自己能看到: PUBLIC + PRIVATE
    std::vector<std::string> result = priv_includes;
    result.insert(result.end(), pub_includes.begin(), pub_includes.end());
    return result;
}

} // namespace build_tooling

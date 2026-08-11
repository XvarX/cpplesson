// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/ctest_sim.cpp — CTestRunner 类的方法实现                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "build_tooling/ctest_sim.hpp"
#include <print>
#include <iostream>

namespace build_tooling {

void CTestRunner::add_test(std::string name, std::function<bool()> fn) {
    tests.push_back({std::move(name), std::move(fn)});
}

void CTestRunner::run_all() {
    int passed = 0, failed = 0;
    for (auto& t : tests) {
        std::print("  Test: {} ... ", t.name);
        try {
            if (t.run()) {
                std::println("PASSED");
                ++passed;
            } else {
                std::println("FAILED");
                ++failed;
            }
        } catch (const std::exception& e) {
            std::println("ERROR ({})", e.what());
            ++failed;
        }
    }
    std::println("\n  ════ 结果: {} 通过, {} 失败, {} 总计 ════",
                 passed, failed, passed + failed);
}

} // namespace build_tooling

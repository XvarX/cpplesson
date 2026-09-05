#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  build_tooling/ctest_sim.hpp — 用 C++ 类模拟 CTest 测试运行器               ║
// ║  主题: add_test 注册 + run_all 运行 (对应 enable_testing / ctest)           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <functional>
#include <string>
#include <vector>

namespace build_tooling {

// ── CTestRunner — 模拟 CTest 的测试注册与运行 ─────────────────────────────────
// 是什么: CMake 自带测试运行器 ctest 的最小模拟
// 为什么: add_test() 注册的测试由 ctest 统一调度、汇总、筛选 (-R)、并行 (-j)
// 什么时候用: 任何需要回归测试的项目 — 比 shell 脚本可靠, 与 CDash/CI 集成
class CTestRunner {
public:
    // 模拟 add_test(NAME xxx COMMAND yyy): 注册一个返回 bool 的测试
    void add_test(std::string name, std::function<bool()> fn);

    // 模拟运行 ctest: 逐个执行, 统计 通过/失败/总计
    void run_all();

private:
    struct TestCase {
        std::string name;
        std::function<bool()> run;
    };

    std::vector<TestCase> tests;
};

} // namespace build_tooling

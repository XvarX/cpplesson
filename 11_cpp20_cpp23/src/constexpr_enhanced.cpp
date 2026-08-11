// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  实现: constexpr 增强 — 运行时辅助演示函数                                   ║
// ║                                                                            ║
// ║  说明: constexpr 函数必须在头文件中定义 (编译期求值要求定义可见)。            ║
// ║        本文件中的函数仅做运行时演示；编译期验证请见 examples/ 中的           ║
// ║        static_assert。MSVC 对 constexpr new 的编译期求值支持有限，           ║
// ║        因此本地不强制使用 constexpr 变量。                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "cpp20_23/constexpr_enhanced.hpp"

#include <iostream>
#include <print>
#include <string>
#include <vector>
#include <array>
#include <cstring>
#include <cmath>

namespace cpp20_23 {

// ============================================================================
// Part 1: constexpr new/delete — 运行时验证
// ============================================================================

void verify_compile_time_array_sum() {
    // 编译期: 在 examples/ 中用 static_assert 验证
    // 运行时: 相同函数在运行时同样可用
    int rt_sum = compile_time_array_sum(5);  // 0+1+4+9+16 = 30
    std::cout << "  constexpr new[]/delete[] 验证 (运行时调用):\n";
    std::cout << "    结果 (n=5):  " << rt_sum << "\n";
    std::cout << "    同一函数可在编译期和运行时共用!\n";
}

void verify_compile_time_object_lifetime() {
    int rt_val = compile_time_object_lifetime();
    std::cout << "  constexpr operator new/construct_at/destroy_at 验证:\n";
    std::cout << "    运行时结果: " << rt_val << "  (编译期结果见 examples/)\n";
}

// ============================================================================
// Part 2: std::is_constant_evaluated — 运行时演示
// ============================================================================

void demonstrate_is_constant_evaluated() {
    // 运行时调用 — is_constant_evaluated 返回 false
    double rt_sqrt = fast_or_safe_sqrt(2.0);
    std::cout << "  std::is_constant_evaluated() 分支演示:\n";
    std::cout << "    运行时 sqrt(2.0) ≈ " << rt_sqrt << "\n";
    std::cout << "    (编译期用牛顿迭代法, 运行时走另一条路径)\n";

    size_t rt_ceil17  = bit_ceil_compile_or_runtime(17);
    size_t rt_ceil64  = bit_ceil_compile_or_runtime(64);
    std::cout << "\n";
    std::cout << "    运行时 bit_ceil:\n";
    std::cout << "      bit_ceil(17)  = " << rt_ceil17 << "  (下一个 2 的幂)\n";
    std::cout << "      bit_ceil(64)  = " << rt_ceil64 << "  (已是 2 的幂)\n";
}

// ============================================================================
// Part 3: constexpr 析构函数 / RAII — 运行时验证
// ============================================================================

void demonstrate_constexpr_raii() {
    int rt_raii_sum = compile_time_raii_sum();
    std::cout << "  constexpr 析构函数 + RAII 验证:\n";
    std::cout << "    运行时 sum(1..10) = " << rt_raii_sum << "\n";
    std::cout << "    同一 RAII 类在编译期也能正常工作!\n";

    ConstexprBuffer rt_buf(8);
    for (size_t i = 0; i < rt_buf.size(); ++i) {
        rt_buf[i] = static_cast<int>((i + 1) * 10);
    }
    std::cout << "    运行时 ConstexprBuffer 内容:\n";
    std::cout << "      ";
    for (auto v : rt_buf) std::cout << v << " ";
    std::cout << "\n";
}

void demonstrate_constexpr_vector() {
    auto [rt_sum, rt_size] = compile_time_vector_use();
    std::cout << "  ConstexprVector<T> (编译期 vector) 验证:\n";
    std::cout << "    运行时 vec = [10,20,30,40,50]\n";
    std::cout << "    运行时 sum = " << rt_sum << ", size = " << rt_size << "\n";

    ConstexprVector<std::string> rt_vec;
    rt_vec.push_back("C++20");
    rt_vec.push_back("constexpr");
    rt_vec.push_back("vector");
    std::cout << "    运行时 ConstexprVector<string>:\n";
    for (size_t i = 0; i < rt_vec.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << rt_vec[i];
    }
    std::cout << "\n";
}

// ============================================================================
// Part 4: std::unreachable — 编译期和运行时行为
// ============================================================================

void demonstrate_unreachable() {
    int rt1 = exhaustive_switch(2);
    int rt2 = positive_only(5);
    int rt3 = lookup_or_default(3);

    std::cout << "  std::unreachable() (C++23) 演示:\n";
    std::cout << "    exhaustive_switch(2)  = " << rt1 << "\n";
    std::cout << "    positive_only(5)      = " << rt2 << "\n";
    std::cout << "    lookup_or_default(3)  = " << rt3 << "\n";
    std::cout << "    (编译期验证见 examples/ 中的 static_assert)\n";

    std::cout << "\n";
    std::cout << "  ⚠️  注意: 如果运行时的参数超出预期范围, std::unreachable() 被触发 → UB\n";
    std::cout << "      这是故意的不安全 — std::unreachable 是给编译器优化的,不是给错误处理的\n";
}

// ============================================================================
// Part 5: 综合演示
// ============================================================================

void demonstrate_compile_vs_runtime() {
    // 演示: 同一段逻辑可以在编译期和运行时执行
    // 编译期版本 → 见 examples/09_constexpr_enhanced_demo.cpp 中的 static_assert

    ConstexprVector<int> rt_data;
    for (int i = 1; i <= 20; ++i) rt_data.push_back(i * i);

    long long rt_sum = 0;
    int rt_max = 0;
    for (size_t i = 0; i < rt_data.size(); ++i) {
        rt_sum += rt_data[i];
        if (rt_data[i] > rt_max) rt_max = rt_data[i];
    }
    std::cout << "  运行时构建 20 个元素的动态数组:\n";
    std::cout << "    总数:       " << rt_sum << "\n";
    std::cout << "    元素个数:   " << rt_data.size() << "\n";
    std::cout << "    最大值:     " << rt_max << "\n";
    // sum(1²..20²) = 20*21*41/6 = 2870
    std::cout << "    预期 sum:   2870\n";
    std::cout << "    " << (rt_sum == 2870 ? "✓" : "✗") << " 一致性!\n";
}

}  // namespace cpp20_23

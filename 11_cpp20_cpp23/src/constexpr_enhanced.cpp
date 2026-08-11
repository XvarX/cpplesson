// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  实现: constexpr 增强 — 运行时辅助演示函数                                   ║
// ║                                                                            ║
// ║  说明: constexpr 函数必须在头文件中定义 (编译期求值要求定义可见)。            ║
// ║        本文件提供运行时辅助和验证函数，供示例程序使用。                        ║
// ║                                                                            ║
// ║  兼容性: 部分函数使用 std::cout 代替 std::println，                         ║
// ║          因为 MSVC 对 consteval format_string 的检查较为严格。                ║
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
    constexpr int ct_sum = compile_time_array_sum(5);  // 0+1+4+9+16 = 30
    int rt_sum = compile_time_array_sum(5);
    std::cout << "  constexpr new[]/delete[] 验证:\n";
    std::cout << "    编译期结果 (n=5):  " << ct_sum << "\n";
    std::cout << "    运行时结果 (n=5):  " << rt_sum << "\n";
    std::cout << "    一致性: " << (ct_sum == rt_sum ? "✓ 通过" : "✗ 不一致!") << "\n";
}

void verify_compile_time_object_lifetime() {
    constexpr int ct_val = compile_time_object_lifetime();
    int rt_val = compile_time_object_lifetime();
    std::cout << "  constexpr operator new/construct_at/destroy_at 验证:\n";
    std::cout << "    编译期结果: " << ct_val << "\n";
    std::cout << "    运行时结果: " << rt_val << "\n";
}

// ============================================================================
// Part 2: std::is_constant_evaluated — 运行时演示
// ============================================================================

void demonstrate_is_constant_evaluated() {
    constexpr double ct_sqrt = fast_or_safe_sqrt(2.0);
    double rt_sqrt = fast_or_safe_sqrt(2.0);

    std::cout << "  std::is_constant_evaluated() 分支演示:\n";
    std::cout << "    编译期 sqrt(2.0) ≈ " << ct_sqrt << "  (使用牛顿迭代法)\n";
    std::cout << "    运行时 sqrt(2.0) ≈ " << rt_sqrt << "  (使用运行时路径)\n";
    std::cout << "    编译期使用了不同的算法路径!\n";

    constexpr size_t ct_ceil17  = bit_ceil_compile_or_runtime(17);
    constexpr size_t ct_ceil64  = bit_ceil_compile_or_runtime(64);
    constexpr size_t ct_ceil1   = bit_ceil_compile_or_runtime(1);
    std::cout << "\n";
    std::cout << "    编译期 bit_ceil:\n";
    std::cout << "      bit_ceil(17)  = " << ct_ceil17 << "  (下一个 2 的幂)\n";
    std::cout << "      bit_ceil(64)  = " << ct_ceil64 << "  (已是 2 的幂)\n";
    std::cout << "      bit_ceil(1)   = " << ct_ceil1 << "\n";
}

// ============================================================================
// Part 3: constexpr 析构函数 / RAII — 运行时验证
// ============================================================================

void demonstrate_constexpr_raii() {
    constexpr int ct_raii_sum = compile_time_raii_sum();
    std::cout << "  constexpr 析构函数 + RAII 验证:\n";
    std::cout << "    编译期 sum(1..10) = " << ct_raii_sum << "\n";
    std::cout << "    ConstexprBuffer 在编译期自动调用析构释放内存!\n";

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
    constexpr auto [ct_sum, ct_size] = compile_time_vector_use();
    std::cout << "  ConstexprVector<T> (编译期 vector) 验证:\n";
    std::cout << "    编译期 vec = [10,20,30,40,50]\n";
    std::cout << "    编译期 sum = " << ct_sum << ", size = " << ct_size << "\n";

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
    constexpr int ct1 = exhaustive_switch(2);
    constexpr int ct2 = positive_only(5);
    constexpr int ct3 = lookup_or_default(3);

    std::cout << "  std::unreachable() (C++23) 演示:\n";
    std::cout << "    exhaustive_switch(2)  = " << ct1 << "  [编译期]\n";
    std::cout << "    positive_only(5)      = " << ct2 << "  [编译期]\n";
    std::cout << "    lookup_or_default(3)  = " << ct3 << "  [编译期]\n";

    std::cout << "    exhaustive_switch(1)  = " << exhaustive_switch(1) << "  [运行时]\n";
    std::cout << "    positive_only(10)     = " << positive_only(10) << "  [运行时]\n";

    std::cout << "\n";
    std::cout << "  ⚠️  注意: 如果运行时的参数超出预期范围, std::unreachable() 被触发 → UB\n";
    std::cout << "      这是故意的不安全 — std::unreachable 是给编译器优化的,不是给错误处理的\n";
}

// ============================================================================
// Part 5: 综合演示
// ============================================================================

void demonstrate_compile_vs_runtime() {
    constexpr auto build_and_query() {
        ConstexprVector<int> data;
        for (int i = 1; i <= 20; ++i) {
            data.push_back(i * i);
        }
        long long sum = 0;
        int max_val = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            sum += data[i];
            if (data[i] > max_val) max_val = data[i];
        }
        struct Stats { long long total; size_t count; int max; };
        return Stats{sum, data.size(), max_val};
    }

    constexpr auto ct_stats = build_and_query();
    std::cout << "  编译期构建 20 个元素的动态数组:\n";
    std::cout << "    总数:       " << ct_stats.total << "\n";
    std::cout << "    元素个数:   " << ct_stats.count << "\n";
    std::cout << "    最大值:     " << ct_stats.max << "\n";

    // sum(1²..20²) = 20*21*41/6 = 2870
    std::cout << "    预期 sum:   2870\n";
    std::cout << "    " << (ct_stats.total == 2870 ? "✓" : "✗") << " 一致性!\n";

    ConstexprVector<int> rt_data;
    for (int i = 1; i <= 20; ++i) rt_data.push_back(i * i);
    long long rt_sum = 0;
    for (size_t i = 0; i < rt_data.size(); ++i) rt_sum += rt_data[i];
    std::cout << "    运行时 sum:  " << rt_sum << "\n";
}

}  // namespace cpp20_23

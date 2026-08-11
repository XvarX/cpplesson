// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: C++20/23 新特性                                                       ║
// ║  实现: constexpr 增强 — 运行时辅助演示函数                                   ║
// ║                                                                            ║
// ║  说明: constexpr 函数必须在头文件中定义 (编译期求值要求定义可见)。            ║
// ║        本文件提供运行时辅助和验证函数，供示例程序使用。                        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "cpp20_23/constexpr_enhanced.hpp"

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

/// 运行时验证: 确保编译期计算结果与运行时一致
void verify_compile_time_array_sum() {
    // 编译期: constexpr 函数在编译期被求值
    constexpr int ct_sum = compile_time_array_sum(5);  // 1*1+2*2+3*3+4*4 = 0+1+4+9+16 = 30
    // 运行时: 同样的函数在运行时调用
    int rt_sum = compile_time_array_sum(5);
    std::println("  constexpr new[]/delete[] 验证:");
    std::println("    编译期结果 (n=5):  {}", ct_sum);
    std::println("    运行时结果 (n=5):  {}", rt_sum);
    std::println("    一致性: {}", ct_sum == rt_sum ? "✓ 通过" : "✗ 不一致!");
}

/// 运行时验证: 编译期对象构造/析构
void verify_compile_time_object_lifetime() {
    constexpr int ct_val = compile_time_object_lifetime();
    int rt_val = compile_time_object_lifetime();
    std::println("  constexpr operator new/construct_at/destroy_at 验证:");
    std::println("    编译期结果: {}", ct_val);
    std::println("    运行时结果: {}", rt_val);
}

// ============================================================================
// Part 2: std::is_constant_evaluated — 运行时演示
// ============================================================================

/// 演示 is_constant_evaluated 在编译期 vs 运行时的不同行为
void demonstrate_is_constant_evaluated() {
    // 编译期调用 — is_constant_evaluated 返回 true
    constexpr double ct_sqrt = fast_or_safe_sqrt(2.0);
    // 运行时调用 — is_constant_evaluated 返回 false
    double rt_sqrt = fast_or_safe_sqrt(2.0);

    std::println("  std::is_constant_evaluated() 分支演示:");
    std::println("    编译期 sqrt(2.0) ≈ {:.6f}  (使用牛顿迭代法)", ct_sqrt);
    std::println("    运行时 sqrt(2.0) ≈ {:.6f}  (使用运行时路径)", rt_sqrt);
    std::println("    编译期使用了不同的算法路径!");

    // 编译期 bit_ceil 演示
    constexpr size_t ct_ceil17  = bit_ceil_compile_or_runtime(17);
    constexpr size_t ct_ceil64  = bit_ceil_compile_or_runtime(64);
    constexpr size_t ct_ceil1   = bit_ceil_compile_or_runtime(1);
    std::println("");
    std::println("    编译期 bit_ceil:");
    std::println("      bit_ceil(17)  = {}  (下一个 2 的幂)", ct_ceil17);
    std::println("      bit_ceil(64)  = {}  (已是 2 的幂)", ct_ceil64);
    std::println("      bit_ceil(1)   = {}", ct_ceil1);
}

// ============================================================================
// Part 3: constexpr 析构函数 / RAII — 运行时验证
// ============================================================================

/// 运行时验证: ConstexprBuffer RAII 在编译期正常工作
void demonstrate_constexpr_raii() {
    // 编译期: ConstexprBuffer 的构造和析构都在编译期完成
    constexpr int ct_raii_sum = compile_time_raii_sum();  // 1+2+...+10 = 55
    std::println("  constexpr 析构函数 + RAII 验证:");
    std::println("    编译期 sum(1..10) = {}", ct_raii_sum);
    std::println("    ConstexprBuffer 在编译期自动调用析构释放内存!");

    // ConstexprBuffer 也可以在运行时使用
    ConstexprBuffer rt_buf(8);
    for (size_t i = 0; i < rt_buf.size(); ++i) {
        rt_buf[i] = static_cast<int>((i + 1) * 10);
    }
    std::println("    运行时 ConstexprBuffer 内容:");
    std::print("      ");
    for (auto v : rt_buf) std::print("{} ", v);
    std::println("");
    // rt_buf 离开作用域时自动析构 — 运行时 RAII 同样生效
}

/// 运行时验证: ConstexprVector 编译期使用
void demonstrate_constexpr_vector() {
    constexpr auto [ct_sum, ct_size] = compile_time_vector_use();
    std::println("  ConstexprVector<T> (编译期 vector) 验证:");
    std::println("    编译期 vec = [10,20,30,40,50]");
    std::println("    编译期 sum = {}, size = {}", ct_sum, ct_size);

    // 运行时使用 ConstexprVector
    ConstexprVector<std::string> rt_vec;
    rt_vec.push_back("C++20");
    rt_vec.push_back("constexpr");
    rt_vec.push_back("vector");
    std::println("    运行时 ConstexprVector<string>:");
    for (size_t i = 0; i < rt_vec.size(); ++i) {
        if (i > 0) std::print(", ");
        std::print("{}", rt_vec[i]);
    }
    std::println("");
}

// ============================================================================
// Part 4: std::unreachable — 编译期和运行时行为
// ============================================================================

/// 演示: std::unreachable 在编译期和运行时的不同语义
void demonstrate_unreachable() {
    // 编译期使用 — 正常返回
    constexpr int ct1 = exhaustive_switch(2);
    constexpr int ct2 = positive_only(5);
    constexpr int ct3 = lookup_or_default(3);

    std::println("  std::unreachable() (C++23) 演示:");
    std::println("    exhaustive_switch(2)  = {}  [编译期]", ct1);
    std::println("    positive_only(5)      = {}  [编译期]", ct2);
    std::println("    lookup_or_default(3)  = {}  [编译期]", ct3);

    // 运行时正常路径 — 正常工作
    std::println("    exhaustive_switch(1)  = {}  [运行时]", exhaustive_switch(1));
    std::println("    positive_only(10)     = {}  [运行时]", positive_only(10));

    std::println("");
    std::println("  ⚠️  注意: 如果运行时的参数超出预期范围, std::unreachable() 被触发 → UB");
    std::println("      这是故意的不安全 — std::unreachable 是给编译器优化的,不是给错误处理的");
}

// ============================================================================
// Part 5: 综合演示
// ============================================================================

/// 编译期计算 vs 运行时计算: constexpr 增强让编译期能做更多事
void demonstrate_compile_vs_runtime() {
    // 在编译期构建一个"数据库"并查询
    // 利用 constexpr new/delete + constexpr 析构函数

    constexpr auto build_and_query() {
        ConstexprVector<int> data;
        for (int i = 1; i <= 20; ++i) {
            data.push_back(i * i);  // 存储平方数
        }
        // 计算统计信息
        long long sum = 0;
        int max_val = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            sum += data[i];
            if (data[i] > max_val) max_val = data[i];
        }
        // 返回统计结果
        struct Stats { long long total; size_t count; int max; };
        return Stats{sum, data.size(), max_val};
    }

    constexpr auto ct_stats = build_and_query();
    std::println("  编译期构建 20 个元素的动态数组:");
    std::println("    总数:       {}", ct_stats.total);
    std::println("    元素个数:   {}", ct_stats.count);
    std::println("    最大值:     {}", ct_stats.max);

    // 验证结果
    // sum(1²..20²) = 20*21*41/6 = 2870
    std::println("    预期 sum:   2870");
    std::println("    {} 一致性!", ct_stats.total == 2870 ? "✓" : "✗");

    // 运行时同样操作
    ConstexprVector<int> rt_data;
    for (int i = 1; i <= 20; ++i) rt_data.push_back(i * i);
    long long rt_sum = 0;
    for (size_t i = 0; i < rt_data.size(); ++i) rt_sum += rt_data[i];
    std::println("    运行时 sum:  {}", rt_sum);
}

}  // namespace cpp20_23

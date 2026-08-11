// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 08_move_and_lambda                                                     ║
// ║  主题: std::function_ref、std::move_only_function、invocable/predicate        ║
// ║  学习目标:                                                                   ║
// ║    1. 掌握 std::function_ref(C++23) — 非拥有、零开销的可调用包装              ║
// ║    2. 理解 std::move_only_function(C++23) — 可移动不可拷贝的包装器           ║
// ║    3. 使用 std::invocable concept(C++20) 约束可调用模板参数                   ║
// ║    4. 使用 std::predicate concept(C++20) 约束谓词模板参数                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "move_lambda/callable_utils.hpp"
#include <print>
#include <string>
#include <vector>
#include <functional>   // function_ref, function, move_only_function
#include <concepts>     // invocable, predicate, relation
#include <memory>
#include <algorithm>

using namespace move_lambda;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: std::function_ref(C++23) — 非拥有、零开销可调用包装
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::function_ref<R(Args...)> 是一个"可调用对象的视图(view)"
//        只存储两个指针(对象指针 + 调用函数指针), 不拥有对象, 不堆分配
// WHY:   替代裸函数指针 + void* 的惯用法, 类型安全, 性能与函数指针相同
// WHEN:   函数参数是回调且不需要存储时(同步调用、算法参数、遍历回调)

void part1_function_ref() {
    lesson::print_subtitle("Part 1: std::function_ref — 非拥有零开销包装");

    // 对比: function vs function_ref — CopyTracker 拷贝次数
    {
        std::println("对比: 传给 function(按值) vs function_ref(引用):");
        CopyTracker tracker(5);
        CopyTracker::reset_counts();

        // function: 按值存储, 产生拷贝
        std::println("传入 std::function:");
        call_with_function(std::function<int(int)>{tracker}, 3);  // 拷贝!
        std::println("  function 拷贝次数: {}", CopyTracker::copy_count);

        CopyTracker::reset_counts();
        // function_ref: 引用存储, 不拷贝
        std::println("传入 std::function_ref:");
        call_with_ref(std::function_ref<int(int)>{tracker}, 3);   // 不拷贝!
        std::println("  function_ref 拷贝次数: {} (零拷贝!)", CopyTracker::copy_count);
    }

    // function_ref 包装不同类型的可调用对象
    std::println("\nfunction_ref 包装不同可调用对象:");
    {
        // 1. 包装自由函数
        call_with_ref(square, 7);       // square(7) = 49

        // 2. 包装 Lambda
        auto triple = [](int x) { return x * 3; };
        call_with_ref(triple, 7);       // 21

        // 3. 包装函数对象
        CopyTracker t(3);
        call_with_ref(std::function_ref<int(int)>{t}, 7);  // 21
    }

    // 关键限制: function_ref 不拥有对象, 被引用者必须存活
    std::println("\n关键: function_ref 不拥有对象, 被引用者必须存活于调用期间");
    std::println("  正确用法: 函数参数中临时使用  callback(std::function_ref<int(int)>{obj})");
    std::println("  错误用法: 存储 function_ref 到成员变量(对象销毁后悬垂!)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: std::move_only_function(C++23) — 可移动不可拷贝的包装器
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::move_only_function<Signature> 是一个 move-only 的可调用对象包装器
//        与 std::function 类似, 但不支持拷贝(因此可以包装 move-only 的类型)
//        支持 cv/ref/noexcept 限定: move_only_function<void() const noexcept>
// WHY:   包装 unique_ptr 捕获的 Lambda、不可拷贝对象 — std::function 做不到
// WHEN:   异步回调、唯一所有权的回调场景、线程间转移回调

void part2_move_only_function() {
    lesson::print_subtitle("Part 2: std::move_only_function — 可移动不可拷贝");

    // 场景: Lambda 捕获 unique_ptr — std::function 无法存储
    {
        // 创建一个捕获 unique_ptr 的 Lambda (move-only)
        auto unique_lambda = [data = std::make_unique<int>(42)](int x) -> int {
            return *data + x;
        };

        // ❌ std::function 无法存储 move-only Lambda:
        // std::function<int(int)> f = unique_lambda;    // 编译错误!

        // ✅ move_only_function 可以:
        std::move_only_function<int(int)> mof = std::move(unique_lambda);
        std::println("move_only_function(10) = {}", mof(10));  // 42+10=52

        // 只能移动, 不能拷贝:
        auto mof2 = std::move(mof);   // ✅ 移动 OK
        // auto mof3 = mof2;          // ❌ 编译错误! move_only_function 不可拷贝
        std::println("移动后: mof2(20) = {}", mof2(20));     // 62
    }

    // cv-ref 限定: const 可调用对象
    {
        std::println("\ncv-ref 限定:");
        // const 限定的 move_only_function
        std::move_only_function<int() const> cmof = [x = 100]() { return x; };
        std::println("move_only_function<int() const>: {}", cmof());
    }

    // noexcept 限定
    {
        std::move_only_function<int(int) noexcept> nmof = [](int x) noexcept { return x * 2; };
        std::println("move_only_function<int(int) noexcept>: {}", nmof(10));
    }

    std::println("\n核心: move_only_function = function 的移动版本, 专为唯一所有权场景设计");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: std::invocable concept(C++20) — 编译期约束可调用类型
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::invocable<F, Args...> concept: 检查 F 是否可以用 Args... 调用
//        std::regular_invocable<F, Args...>: invocable + 可相等比较(equality-preserving)
// WHY:   替代 SFINAE / enable_if 的晦涩写法, 提供清晰的编译错误信息
// WHEN:   模板函数接受可调用参数时, 用 concept 取代无约束的 typename

void part3_invocable() {
    lesson::print_subtitle("Part 3: std::invocable — 编译期可调用性约束");

    // 使用 invocable 约束的 apply_twice 函数(定义在头文件中)
    {
        // ✅ 可调用的 Lambda
        auto inc = [](int x) { return x + 1; };
        std::println("apply_twice(inc, 3) = {}", apply_twice(inc, 3));  // inc(inc(3)) = 5

        auto square = [](int x) { return x * x; };
        std::println("apply_twice(square, 2) = {}", apply_twice(square, 2));  // 16
    }

    // ❌ 故意传入不可调用的类型会有清晰的编译错误:
    // apply_twice(42, 5);  // 编译错误: 'int' 不满足 constraint 'std::invocable<int>'

    // 使用 requires 子句编写带约束的泛型代码
    {
        std::println("\n用 requires 约束泛型 lambda:");
        // C++20 泛型 Lambda 也可以用 requires 约束
        auto safe_call = [](auto&& fn, int x) requires std::invocable<decltype(fn), int> {
            return std::invoke(std::forward<decltype(fn)>(fn), x);
        };

        std::println("safe_call([](int x){ return x*10; }, 5) = {}",
                     safe_call([](int x) { return x * 10; }, 5));
    }

    // regular_invocable: 要求调用结果可重复且一致
    {
        std::println("\nregular_invocable: 进一步要求调用的 equality-preserving");
        auto id = [](int x) { return x; };
        static_assert(std::regular_invocable<decltype(id), int>,
                      "id 是 regular_invocable(纯函数)");
        std::println("  id 满足 regular_invocable<int> ✓");
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: std::predicate concept(C++20) — 编译期约束谓词类型
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::predicate<F, Args...> = regular_invocable<F, Args...> + 返回 bool
//        std::relation<F, T, U> — 二元谓词(用于排序、比较)
//        std::equivalence_relation / std::strict_weak_order — 特定语义关系
// WHY:   确保算法传入的谓词签名正确, 编译期检查, 而不是运行时发现
// WHEN:   编写接受判断逻辑的泛型算法(find_if, remove_if, sort, unique 等)

void part4_predicate() {
    lesson::print_subtitle("Part 4: std::predicate — 编译期谓词约束");

    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // 使用 predicate 约束的 count_if_custom(定义在头文件中)
    {
        auto is_even = [](int x) { return x % 2 == 0; };
        int evens = count_if_custom(data, is_even);
        std::println("count_if_custom(偶数): {}", evens);  // 5

        auto is_gt5 = [](int x) { return x > 5; };
        int gt5 = count_if_custom(data, is_gt5);
        std::println("count_if_custom(>5): {}", gt5);      // 5
    }

    // std::relation — 二元关系(用于排序/比较)
    {
        std::println("\nstd::relation 约束二元谓词:");

        auto less = [](int a, int b) { return a < b; };
        static_assert(std::relation<decltype(less), int, int>,
                      "less 满足 relation<int,int>");
        std::println("  is_ordered(3, 7, less) = {}",
                     is_ordered(3, 7, less));   // true: 3<7
        std::println("  is_ordered(7, 3, less) = {}",
                     is_ordered(7, 3, less));   // false: 7<3
    }

    // 编译期类型安全: 传入非谓词类型会被拒绝
    {
        std::println("\n编译期类型安全:");
        // ❌ 返回非 bool 的值会被 concept 拦截:
        // auto bad_pred = [](int x) { return x * 2; };  // 返回 int
        // count_if_custom(data, bad_pred);  // 编译错误: 不满足 predicate<int>

        static_assert(!std::predicate<decltype([](int x) { return x * 2; }), int>,
                      "返回 int 的不是 predicate");
        std::println("  static_assert: 返回 int 的 Lambda 不满足 predicate<int> ✓");

        static_assert(!std::predicate<decltype([](const std::string&) { return true; }), int>,
                      "参数类型不匹配的不是 predicate");
        std::println("  static_assert: 接受 string 的 Lambda 不满足 predicate<int> ✓");
    }

    std::println("\n核心: concept 将错误从运行时/链接期前移到编译期, 错误信息精确清晰");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
void pitfalls() {
    lesson::print_header("常见陷阱");

    std::println("陷阱1: function_ref 悬垂");
    std::println("  auto make_ref() {");
    std::println("    auto lambda = [](int x) { return x * 2; };");
    std::println("    return std::function_ref<int(int)>{lambda};  // ❌ lambda 已销毁!");
    std::println("  }");
    std::println("  → function_ref 只是'视图', 不延长被引用对象的生命周期");

    std::println("陷阱2: move_only_function 移动后状态");
    std::println("  auto f2 = std::move(f1);  // f1 处于'已移动'状态");
    std::println("  f1(42);  // ❌ 未定义行为! f1 已被移动, 内容为空");
    std::println("  规则: 移动后不要再使用原对象, 除非先检查(但 move_only_function 没有 bool 转换)");

    std::println("陷阱3: concept 检查的是签名, 不是语义");
    std::println("  std::predicate 只检查: 可调用 + 返回可转bool");
    std::println("  它不检查: 谓词是否真的是纯函数、是否有副作用");
    std::println("  → C++23 没有'纯函数'概念, 歧义行为仍需自行保证");

    std::println("陷阱4: move_only_function 不能放入 std::function 或需要拷贝的容器");
    std::println("  std::vector<std::move_only_function<void()>> v;  // ❌ 默认要求元素可拷贝");
    std::println("  解决: 用 std::move 插入, 或用 std::list(不要求元素可拷贝)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
void exercises() {
    lesson::print_header("练习");
    std::println("1. 写一个 for_each 函数, 参数用 function_ref<void(int)>, 遍历 vector 调用它");
    std::println("2. 创建一个捕获 unique_ptr<string> 的 Lambda, 用 move_only_function 存储并调用");
    std::println("3. 用 std::invocable 约束一个通用 transform 模板函数, 测试正确/错误调用");
    std::println("4. 用 std::predicate 实现一个 filter 模板函数, 对比无约束版本的错误信息");
    std::println("5. 总结三种包装器的选择指南:");
    std::println("   - function_ref:  同步调用、不存储、零开销");
    std::println("   - function:       需要拷贝存储回调、类型擦除");
    std::println("   - move_only_function: 需唯一所有权、移动-only 回调");
}

int main() {
    lesson::print_header("06: function_ref、move_only_function、invocable/predicate");

    part1_function_ref();
    part2_move_only_function();
    part3_invocable();
    part4_predicate();
    pitfalls();
    exercises();

    return 0;
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 08_move_and_lambda                                                     ║
// ║  主题: 泛型 Lambda(auto)、constexpr Lambda、捕获初始化、C++20 Lambda 特性    ║
// ║  学习目标:                                                                   ║
// ║    1. 掌握泛型 Lambda(auto 参数) — 隐式模板的威力                            ║
// ║    2. 理解 constexpr Lambda — 编译期计算的利器                               ║
// ║    3. 学会捕获初始化(C++14 init capture) — 移动捕获、表达式捕获              ║
// ║    4. 使用 C++20 模板 Lambda — 比 auto 参数更精确的类型控制                 ║
// ║    5. 了解 C++20 [=, this] 的显式 this 捕获语义                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "move_lambda/lambda_advanced.hpp"
#include <print>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <ranges>

using namespace move_lambda;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: 泛型 Lambda — auto 参数(隐式模板, C++14)
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  Lambda 参数用 auto → 编译器生成隐式的模板 operator()
//        [](auto x, auto y) { return x + y; }
//        等价于一个模板函数对象
// WHY:   不需要手写模板函数对象, 类型推导自动完成
// WHEN:   需要通用操作时(如比较、打印、算术), 且不关心具体类型

void part1_generic_lambda() {
    lesson::print_subtitle("Part 1: 泛型 Lambda(auto 参数)");

    // 一个泛型 Lambda: 适用于任何支持 + 的类型
    auto generic_add = [](auto a, auto b) { return a + b; };

    std::println("generic_add(3, 5)       = {}", generic_add(3, 5));
    std::println("generic_add(3.14, 2.86) = {}", generic_add(3.14, 2.86));

    std::string s1 = "Hello, ";
    std::string s2 = "World!";
    std::println("generic_add(s1, s2)     = {}", generic_add(s1, s2));

    // 泛型 Lambda + 完美转发(结合 decltype(auto))
    // 注意: C++14 起, Lambda 可以用 auto&& 参数实现万能引用
    auto forwarder = [](auto&& arg) -> decltype(auto) {
        return static_cast<decltype(arg)>(arg);  // 完美转发
    };

    std::string msg = "test";
    auto& ref = forwarder(msg);       // 左值进入, 左值返回
    auto&& rref = forwarder(std::string("temp")); // 右值进入, 右值返回
    std::println("forwarder 保持值类别: 左值→左值, 右值→右值");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: constexpr Lambda — 编译期计算(C++17)
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  C++17 起, Lambda 隐式地是 constexpr 的(只要能编译期求值)
//        C++17 明确允许 Lambda 在常量表达式中使用
// WHY:   编译期计算替代运行时开销, 支持模板参数、static_assert、数组大小等场景
// WHEN:   需要在编译期执行逻辑的场景(元编程、固定计算等)

void part2_constexpr_lambda() {
    lesson::print_subtitle("Part 2: constexpr Lambda");

    // ⚠️ 注意: 递归 constexpr lambda 在 auto 推导完成前引用自身,
    //        在 GCC 下会因 auto 推导不完整而编译失败。
    //        以下用 constexpr 普通函数演示编译期递归计算(行为等价):
    constexpr auto factorial = [](int n) -> int {
        // 非递归: 用迭代方式实现阶乘, 避免 auto 自引用问题
        int result = 1;
        for (int i = 2; i <= n; ++i) result *= i;
        return result;
    };

    // 编译期使用:
    constexpr int fact10 = factorial(10);
    static_assert(fact10 == 3'628'800, "10! should be 3628800");
    std::println("constexpr factorial(10) = {}", fact10);

    // 用作模板参数(编译期值)
    std::array<int, factorial(4)> arr{};  // 24 个元素
    std::println("std::array<int, factorial(4)> 大小 = {}", arr.size());

    // constexpr Lambda 也可以运行时调用
    int runtime_n = 5;
    std::println("运行时 factorial(5) = {}", factorial(runtime_n));

    // 如果你使用的编译器(MSVC/Clang)支持递归 constexpr lambda,
    // 原始写法是这样的(更直观但不跨编译器):
    // constexpr auto rfac = [](int n) constexpr -> int {
    //     if (n <= 1) return 1;
    //     return n * rfac(n - 1);
    // };

    // constexpr Lambda 用于 constexpr 条件
    constexpr auto is_prime = [](int n) constexpr -> bool {
        if (n < 2) return false;
        for (int d = 2; d * d <= n; ++d)
            if (n % d == 0) return false;
        return true;
    };
    static_assert(is_prime(17), "17 is prime");
    static_assert(!is_prime(100), "100 is not prime");
    std::println("constexpr is_prime 编译期验证通过!");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: 捕获初始化(Init Capture, C++14)
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  [var = expr] 在捕获列表中进行初始化, 创建新的 Lambda 成员变量
//        包括: [p = std::move(ptr)]  — 移动捕获(将外部对象移入 Lambda)
//              [cached = compute()]  — 缓存计算结果
//              [p = std::make_shared<T>()] — 创建共享所有权
// WHY:   避免按引用捕获的悬垂问题, 避免按值捕获的拷贝开销(可改用移动)
// WHEN:   需要将 unique_ptr 移入 Lambda, 或 Lambda 需要独占某一资源

void part3_init_capture() {
    lesson::print_subtitle("Part 3: 捕获初始化(Init Capture)");

    // 场景1: 移动捕获 — 将 unique_ptr 移入 Lambda
    auto uptr = std::make_unique<std::string>("Move me into lambda!");
    std::println("unique_ptr 原始值: \"{}\"", *uptr);

    auto lambda_owns = [data = std::move(uptr)] {
        std::println("Lambda 内部: \"{}\"", *data);
    };
    // uptr 现在是 nullptr — 所有权已转移到 Lambda
    std::println("uptr 现在是: {}", uptr ? "非空" : "nullptr");
    lambda_owns();

    // 场景2: 预计算捕获 — 避免每次调用都重新计算
    auto with_cache = [sum = [] {
        int s = 0;
        for (int i = 1; i <= 100; ++i) s += i;
        return s;
    }()] {
        return sum;  // 直接使用预先计算好的值, O(1)
    };
    std::println("1+2+...+100 = {}", with_cache());

    // 场景3: 移动捕获 + mutable — 状态机
    auto stateful = [buf = std::vector<int>{1, 2, 3}]() mutable {
        if (buf.empty()) return 0;
        int val = buf.back();
        buf.pop_back();
        return val;
    };
    std::println("stateful(): {} {} {}", stateful(), stateful(), stateful());
    // 输出: 3 2 1
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: C++20 模板 Lambda — 精确的类型控制
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  C++20 允许 Lambda 使用显式模板参数语法:
//        []<typename T>(std::vector<T> v) { ... }
//        []<typename T, int N>(std::array<T, N> arr) { ... }
// WHY:   - 需要知道参数的确切类型(不只是用 auto)
//        - 需要约束多个参数具有相同类型: []<typename T>(T a, T b) { ... }
//        - 需要访问依赖类型: typename T::value_type
// WHEN:   auto 参数不够精确时(需要对类型做约束、需要类型萃取)

void part4_template_lambda() {
    lesson::print_subtitle("Part 4: C++20 模板 Lambda");

    // 对比: auto 参数 vs 模板参数
    // 用 auto: 两个参数可以类型不同
    auto auto_add = [](auto a, auto b) { return a + b; };
    std::println("auto_add(3, 4.5) = {}  // int + double 可以", auto_add(3, 4.5));

    // 用模板: 强制相同类型
    auto typed_add = []<typename T>(T a, T b) { return a + b; };
    std::println("typed_add(3, 4) = {}", typed_add(3, 4));
    // typed_add(3, 4.5)  // ❌ 歧义: T 是 int 还是 double?

    // 模板 Lambda 访问容器值类型
    auto print_value_type = []<typename Container>(const Container& c) {
        using ValueType = typename Container::value_type;
        std::println("  容器元素类型大小: {} 字节", sizeof(ValueType));
        std::println("  元素数量: {}", std::ranges::size(c));
    };

    std::vector<int> vi = {1, 2, 3};
    std::vector<double> vd = {1.0, 2.0};
    std::println("vector<int>:");
    print_value_type(vi);
    std::println("vector<double>:");
    print_value_type(vd);

    // 用模板 Lambda 做类型萃取
    auto is_integral_container = []<typename T>(const std::vector<T>&) {
        return std::is_integral_v<T>;
    };
    std::println("vector<int> 是整数容器?  {}", is_integral_container(vi));
    std::println("vector<double> 是整数容器? {}", is_integral_container(vd));
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: C++20 显式 this 捕获 — [=, this] 语义澄清
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  C++20 之前, [=] 隐式捕获 this, 容易误解为按值捕获了 *this
//        C++20: [=, this] 显式捕获 this 指针(和以前行为一样)
//               [=, *this] 按值捕获整个对象(C++17起支持)
// WHY:   让 this 捕获意图明确, 减少意外
// WHEN:   在成员函数中定义 Lambda 时, 明确表达你的意图

void part5_explicit_this() {
    lesson::print_subtitle("Part 5: C++20 显式 this 捕获");

    ThisCaptureWidget w;
    auto reader_ref = w.getReader_explicit();
    auto reader_cpy = w.getReader_byCopy();

    std::println("修改前: reader_ref={}, reader_cpy={}", reader_ref(), reader_cpy());

    w.value = 999;  // 修改原始对象

    std::println("修改后: reader_ref={}, reader_cpy={}", reader_ref(), reader_cpy());
    // reader_ref 看到新值(引用了原对象)
    // reader_cpy 还是旧值(持有的是副本)
    std::println("结论: [=, this] 跟随原对象变化, [=, *this] 持有独立快照");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
void pitfalls() {
    lesson::print_header("常见陷阱");

    std::println("陷阱1: auto 参数 Lambda 的类型推导致歧义");
    std::println("  [](auto a, auto b) {{ return a < b; }}  ");
    std::println("  → a 和 b 类型可以不同, 可能导致意外的比较结果");

    std::println("陷阱2: 捕获初始化的求值时机");
    std::println("  [x = expensive_call()]  — 在 Lambda 定义时求值, 不是调用时");
    std::println("  → 如果 expensive_call 有副作用, 确保定义时机正确");

    std::println("陷阱3: 泛型 Lambda 中错误使用 std::forward");
    std::println("  [](auto&& arg) {{ foo(std::forward<decltype(arg)>(arg)); }}");
    std::println("  → 对同一个 arg 多次 forward 可能导致 use-after-move");

    std::println("陷阱4: constexpr Lambda 的副作用");
    std::println("  → constexpr 求值不允许分配动态内存(C++20放宽但仍受限)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
void exercises() {
    lesson::print_header("练习");
    std::println("1. 用泛型 Lambda 实现一个通用的 min 函数, 接受任意可比较类型");
    std::println("2. 用 constexpr Lambda 计算编译期斐波那契数列第 N 项");
    std::println("3. 用捕获初始化将 std::thread 移入 Lambda(提示: std::thread 只可移动)");
    std::println("4. 用 C++20 模板 Lambda 写一个函数, 只接受 std::vector<T> 类型的参数");
    std::println("5. 对比: auto 参数 vs 模板参数 Lambda 在错误信息上的差异(故意写错看看)");
}

int main() {
    lesson::print_header("04: Lambda 进阶 — 泛型、constexpr、C++20 特性");

    part1_generic_lambda();
    part2_constexpr_lambda();
    part3_init_capture();
    part4_template_lambda();
    part5_explicit_this();
    pitfalls();
    exercises();

    return 0;
}

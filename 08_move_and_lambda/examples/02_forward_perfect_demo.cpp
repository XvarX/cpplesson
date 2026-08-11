// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 08_move_and_lambda                                                     ║
// ║  主题: std::forward、万能引用(T&&/auto&&)、引用折叠、完美转发、emplace     ║
// ║  学习目标:                                                                   ║
// ║    1. 理解万能引用(universal/forwarding reference)的含义                     ║
// ║    2. 掌握引用折叠规则(只有 &+& = &, 其余都是 &&)                           ║
// ║    3. 用 std::forward 实现完美转发——保持参数的值类别                        ║
// ║    4. 理解 emplace 系列函数为何比 push/insert 更高效                         ║
// ║    5. 编写泛型的工厂函数和包装器                                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "move_lambda/forward_perfect.hpp"
#include <print>
#include <string>
#include <vector>
#include <memory>
#include <utility>   // std::forward, std::move

using namespace move_lambda;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: 万能引用 — T&& 不一定是右值引用!
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  在模板推导上下文中, T&& 是"万能引用":
//        - 传入左值 → T 推导为 T&,  最终变成 T&  && → T&  (左值引用)
//        - 传入右值 → T 推导为 T,   最终变成 T&&      (右值引用)
// WHY:   一个函数模板就能同时接受左值和右值, 且保持参数的值类别
// WHEN:  编写泛型包装器、工厂函数、转发层时

void part1_forwarding_reference() {
    lesson::print_subtitle("Part 1: 万能引用 T&&");

    int a = 42;
    const int b = 100;

    std::println("relay(a) — 传入左值:");
    relay(a);                    // T 推导为 int&,       转发为 process(int&)

    std::println("relay(b) — 传入 const 左值:");
    relay(b);                    // T 推导为 const int&, 转发为 process(const int&)

    std::println("relay(10) — 传入右值:");
    relay(10);                   // T 推导为 int,        转发为 process(int&&)

    std::println("relay(std::move(a)) — 传入将亡值:");
    relay(std::move(a));         // T 推导为 int,        转发为 process(int&&)
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 引用折叠规则
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  在模板推导或 typedef 中, 引用的引用会被"折叠":
//         &  + &  = &     (只有这种情况得到左值引用)
//         &  + && = &     (左值引用 + 右值引用 → 左值引用)
//         && + &  = &     (同上)
//         && + && = &&    (两个右值引用 → 右值引用)
// WHY:   这是万能引用能够工作的底层机制
// WHEN:  理解万能引用时, 引用折叠是必知必会的理论

void part2_reference_collapsing() {
    lesson::print_subtitle("Part 2: 引用折叠");

    // 用 static_assert 验证引用折叠规则
    // 假设 T = int&:  T&& → int& && → int&
    using T1 = int&;
    static_assert(std::is_same_v<T1&&, int&>, "int& && → int&");

    // 假设 T = int&&: T&& → int&& && → int&&
    using T2 = int&&;
    static_assert(std::is_same_v<T2&&, int&&>, "int&& && → int&&");

    // 假设 T = int:   T&& → int&&
    using T3 = int;
    static_assert(std::is_same_v<T3&&, int&&>, "int && → int&&");

    // auto&& 也是万能引用!
    int x = 10;
    auto&& r1 = x;               // auto→int&,  int&  && → int&
    auto&& r2 = 42;              // auto→int,   int   && → int&&

    static_assert(std::is_same_v<decltype(r1), int&>,  "auto&& + 左值 → 左值引用");
    static_assert(std::is_same_v<decltype(r2), int&&>, "auto&& + 右值 → 右值引用");

    std::println("所有 static_assert 通过, 引用折叠规则验证正确!");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: 完美转发 — 实战: 智能指针工厂
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::forward<T>(arg) 保持 arg 的值类别原样传递
// WHY:   避免多余的拷贝, 且正确调用左值/右值的重载版本
// WHEN:  任何包装了参数再转发的场景

void part3_perfect_forwarding_factory() {
    lesson::print_subtitle("Part 3: 完美转发工厂函数");

    std::string widgetName = "MyWidget";
    std::vector<int> widgetData = {1, 2, 3, 4, 5};

    // 左值传入 → forward 为左值引用 → Widget 拷贝构造参数
    std::println("makeWidget(左值, 左值):");
    auto w1 = makeWidget(widgetName, widgetData);
    std::println("  widgetName 仍在: \"{}\"", widgetName);
    std::println("  widgetData 仍在: {} 条", widgetData.size());

    // 右值传入 → forward 为右值引用 → Widget 移动构造参数
    std::println("makeWidget(右值, 右值):");
    auto w2 = makeWidget(std::string("Temp"), std::vector<int>{6, 7, 8});
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: emplace 系列 — 原地构造, 消除临时对象
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  emplace_back / emplace / try_emplace 直接在容器内存中构造元素
//        push_back  / insert   需要先构造临时对象, 再拷贝/移动到容器
// WHY:   减少一次构造/析构, 对于不可拷贝的对象(如 unique_ptr)是必需的
// WHEN:   向容器添加元素时, 优先使用 emplace 系列

void part4_emplace() {
    lesson::print_subtitle("Part 4: emplace 原地构造");

    std::vector<Point> points;

    // push_back: 先构造临时对象, 再移动到容器
    std::println("push_back(Point(1,2,\"A\")):");
    points.push_back(Point(1, 2, "A"));
    // 输出: 构造 + 移动

    // emplace_back: 直接在容器内存中构造, 零额外开销
    std::println("emplace_back(3, 4, \"B\"):");
    points.emplace_back(3, 4, "B");
    // 输出: 只有构造, 没有移动!

    std::println("emplace 优势: 减少临时对象, 对不可拷贝类型(unique_ptr)是必需的");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: std::forward vs std::move — 何时用哪个?
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::move(arg)  → 无条件转成右值引用
//        std::forward<T>(arg) → 条件性转发: 左值→左值, 右值→右值
// 口诀:  std::move  ≈ 无条件的 cast
//        std::forward ≈ 有条件的 cast(只对右值参数转成右值引用)

void part5_move_vs_forward() {
    lesson::print_subtitle("Part 5: std::move vs std::forward");

    int x = 100;

    std::println("relay(x) — 用 std::forward(正确):");
    relay(x);                     // 调用 process(int&) ✅

    std::println("bad_relay(x) — 用 std::move(错误!):");
    bad_relay(x);                 // 调用了 process(int&&), 但 x 是左值! ❌
    // x 可能被意外修改(虽然 int 无所谓, 但对 string/vector 就是灾难)

    std::println("规则: 在模板中永远用 std::forward, 不要用 std::move!");
    std::println("std::move 只用于: 你确定不再需要这个对象时");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
void pitfalls() {
    lesson::print_header("常见陷阱");

    std::println("陷阱1: 在模板中用 std::move 代替 std::forward");
    std::println("  → 会把左值意外转成右值, 导致资源被窃取");

    std::println("陷阱2: 对同一个参数多次使用 std::forward");
    std::println("  → 第二次 forward 时参数可能已经被移动, 变为空壳");

    std::println("陷阱3: emplace_back 传参不当");
    std::println("  → emplace_back({{1,2}}) 推导不出 initializer_list, 编译错误");
    std::println("  → 应写成 emplace_back(std::initializer_list<int>{{1,2}})");

    std::println("陷阱4: 万能引用的构造函数可能比拷贝构造更匹配!");
    std::println("  → 非 const 对象会匹配到万能引用版本, 而非拷贝构造");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
void exercises() {
    lesson::print_header("练习");
    std::println("1. 写一个 setter 函数模板: setter(T&& val), 正确使用 std::forward");
    std::println("2. 实现 vector<string> 的性能对比: push_back vs emplace_back");
    std::println("3. 为什么以下代码有问题?");
    std::println("   template<typename T> void foo(T&& x) {{ bar(std::forward<T>(x)); baz(std::forward<T>(x)); }}");
    std::println("4. 写一个 make_pair 函数, 用完美转发实现, 对比 std::make_pair 行为");
}

int main() {
    lesson::print_header("02: std::forward、万能引用与完美转发");

    part1_forwarding_reference();
    part2_reference_collapsing();
    part3_perfect_forwarding_factory();
    part4_emplace();
    part5_move_vs_forward();
    pitfalls();
    exercises();

    return 0;
}

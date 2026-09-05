// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 07_modern_syntax — 现代语法糖                                        ║
// ║  课题: 类模板参数推导(CTAD) / 推导指引 / 聚合CTAD (C++17/20)              ║
// ║  学习目标:                                                                  ║
// ║    1. 掌握 CTAD (C++17) 让编译器从构造参数推导模板参数                     ║
// ║    2. 理解隐式推导指引和何时需要自定义推导指引                              ║
// ║    3. 学会聚合 CTAD (C++20) — 聚合类型也支持 CTAD                         ║
// ║    4. 识别并避免 CTAD 的常见陷阱                                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "modern_syntax/ctad.hpp"
#include "shared/lesson_utils.hpp"

#include <print>
#include <string>
#include <vector>
#include <utility>     // std::pair
#include <memory>      // std::unique_ptr, std::shared_ptr
#include <type_traits> // std::is_same_v
#include <array>
#include <mutex>

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: CTAD 基础 —— 告别冗长的模板参数                                  ║
// ║  WHAT: C++17 起, 编译器从构造函数实参自动推导类模板参数                   ║
// ║  WHY:  消除冗长的 std::pair<int,std::string>(1,"hello") 写法              ║
// ║  WHEN: 使用标准库模板(pair/tuple/vector/optional/smart_ptr);              ║
// ║        自定义模板类有合适的构造函数时                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_ctad_basics() {
    lesson::print_header("Part 1: CTAD 基础 (C++17)");

    using namespace modern_syntax;

    // ── 1.1 标准库的大幅简化 ──
    {
        lesson::print_subtitle("1.1  标准库的 CTAD 红利");
        // 旧式写法 (C++14)
        std::pair<int, std::string>    old_p(1, "hello");
        std::vector<int>               old_v = {1, 2, 3};
        std::unique_ptr<int>           old_u(new int(42));

        // CTAD 写法 (C++17) —— 编译器自动推导模板参数
        std::pair  new_p(1, "hello");         // → pair<int, const char*>
        std::vector new_v = {1, 2, 3};        // → vector<int>
        auto       new_u = std::make_unique<int>(42);    // → unique_ptr<int>

        std::println("pair:  ({}, '{}')  vector[0]={}  unique_ptr={}",
                     new_p.first, new_p.second, new_v[0], *new_u);

        lesson::print_note("省去 <int, string> <int> 等冗长后缀, 编译器全自动推导");
    }

    // ── 1.2 多种标准库类型的 CTAD 速览 ──
    {
        lesson::print_subtitle("1.2  多种标准库类型的 CTAD 示例");

        auto tp = std::tuple(42, 3.14, std::string("三")); // tuple<int,double,string>
        // 等价于 std::tuple<int, double, std::string> tp(...);

        auto sp = std::make_shared<int>(100);      // shared_ptr<int>
        // auto lk = std::lock_guard(mtx);        // lock_guard: CTAD + 命名互斥量 (C++17)

        std::println("tuple: ({}, {:.2f}, '{}')  shared_ptr={}",
                     std::get<0>(tp), std::get<1>(tp), std::get<2>(tp), *sp);
        lesson::print_note("注意: unique_ptr/shared_ptr 推荐用 make_unique/make_shared");
    }

    // ── 1.3 自定义模板类的 CTAD ──
    {
        lesson::print_subtitle("1.3  自定义模板类自动享受 CTAD");

        Wrapper w1(42);              // CTAD → Wrapper<int>
        Wrapper w2(std::string("CTAD 真棒!")); // CTAD → Wrapper<string>
        // 旧写法: Wrapper<int> w1(42);

        std::println("w1.value={}  w2.value='{}'", w1.value, w2.value);

        // 验证推导结果
        static_assert(std::is_same_v<decltype(w1), Wrapper<int>>);
        static_assert(std::is_same_v<decltype(w2), Wrapper<std::string>>);
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: 推导指引 (Deduction Guides)                                       ║
// ║  WHAT: 显式告诉编译器"从这些构造参数推导出那个模板参数"                     ║
// ║  WHY:  ① 编译器自动推导的结果不是你想要的                                   ║
// ║        ② 构造函数参数无法直接反映模板参数类型 (如迭代器范围构造)          ║
// ║        ③ 对 C 风格字符串的特殊处理 (char[] → std::string)                 ║
// ║  WHEN: 标准库已提供 vector(const char*→string); 自定义模板需特殊推导时     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_deduction_guides() {
    lesson::print_header("Part 2: 推导指引 (Deduction Guides)");

    using namespace modern_syntax;

    // ── 2.1 隐式推导: initializer_list 构造函数 ──
    {
        lesson::print_subtitle("2.1  隐式推导 (initializer_list 构造)");
        DynArray nums = {1, 2, 3, 4};    // 隐式从 int → DynArray<int>
        std::print("DynArray<int>: ");
        for (auto v : nums.data) std::print("{} ", v);
        std::println("");
    }

    // ── 2.2 自定义推导指引: 迭代器范围 → 推导 value_type ──
    {
        lesson::print_subtitle("2.2  自定义推导指引: 迭代器范围构造");
        std::vector<double> src = {1.1, 2.2, 3.3};
        // 没有推导指引: 编译器无法从两个迭代器推导出 T
        // 有了推导指引: InputIt → value_type → DynArray<double>
        DynArray arr(src.begin(), src.end());
        std::print("DynArray<double>: ");
        for (auto v : arr.data) std::print("{:.1f} ", v);
        std::println("");
    }

    // ── 2.3 C 风格字符串 → std::string ──
    {
        lesson::print_subtitle("2.3  推导指引: const char* → string");
        // 没有推导指引: const char* → DynArray<const char*> (几乎总不是我们想要的)
        // 有了推导指引: const char* → DynArray<string>
        // 注意: initializer_list<const char*> 不会被推导为 string
        // 需要使用 ""s 字面量来确保推导为 DynArray<string>
        using namespace std::string_literals;
        DynArray words = {"你好"s, "世界"s, "CTAD"s};
        std::print("DynArray<string>: ");
        for (const auto& w : words.data) std::print("'{}' ", w);
        std::println("");
        // 验证: 确实是 DynArray<string>, 不是 DynArray<const char*>
        static_assert(std::is_same_v<decltype(words), DynArray<std::string>>);
    }

    lesson::print_note("推导指引语法: template<...> ClassName(参数) -> ClassName<推导类型>;");
    lesson::print_note("标准库大量使用推导指引 (如 vector 的迭代器构造, string 的 C 串构造)");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: 聚合 CTAD (C++20)                                                 ║
// ║  WHAT: 聚合类型(无构造函数的结构体模板)也能用 CTAD                         ║
// ║  WHY:  聚合模板同样需要推导模板参数, C++17 做不到                         ║
// ║  WHEN: 模板化的 POD/聚合类型 (如 std::array, 自定义 Point<T>)             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_aggregate_ctad() {
    lesson::print_header("Part 3: 聚合 CTAD (C++20)");

    using namespace modern_syntax;

    // ── 3.1 聚合 CTAD 基本用法 ──
    {
        lesson::print_subtitle("3.1  聚合初始化 + CTAD (C++20)");
        // C++20: 聚合类型可以直接用 {} 并推导模板参数
        Point2D p1{1.5, 2.5};               // CTAD → Point2D<double>
        Point2D p2{10, 20, "起点"};          // CTAD → Point2D<int>

        std::println("p1: ({:.1f}, {:.1f})  label='{}'", p1.x, p1.y, p1.label);
        std::println("p2: ({}, {})  label='{}'", p2.x, p2.y, p2.label);

        static_assert(std::is_same_v<decltype(p1), Point2D<double>>);
        static_assert(std::is_same_v<decltype(p2), Point2D<int>>);
    }

    // ── 3.2 经典的 std::array 聚合 CTAD ──
    {
        lesson::print_subtitle("3.2  std::array 聚合 CTAD");
        // C++17: std::array<int, 3> arr = {1, 2, 3};  // 必须写 <int, 3>
        // C++20: 编译器同时推导 T 和 N
        std::array arr = {10, 20, 30, 40};   // CTAD → array<int, 4>
        std::print("array: ");
        for (auto v : arr) std::print("{} ", v);
        std::println("");
        std::println("元素数: {}", arr.size()); // 输出: 4
    }

    // ── 3.3 指定初始化 + CTAD 组合 ──
    {
        lesson::print_subtitle("3.3  指定初始化 + CTAD (C++20 双杀技)");
        // 指定初始化 + 聚合CTAD, 需要推导指引确保推导正确
        Point2D p{.x = 3.0, .y = 4.0, .label = "终点"};
        static_assert(std::is_same_v<decltype(p), Point2D<double>>);
        std::println("p: ({:.1f}, {:.1f})  label='{}'", p.x, p.y, p.label);
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void pitfalls() {
    lesson::print_header("常见陷阱");

    // 陷阱1: vector 的 CTAD 歧义
    {
        lesson::print_subtitle("陷阱1: vector CTAD 的 {} vs () 歧义");
        // std::vector v1{1, 2, 3};      // CTAD → vector<int>, 3 个元素
        // std::vector v2(5);             // CTAD 不适用于 () 构造! 编译错误!
        // std::vector v2(5, 10);         // 编译错误: CTAD 不能从 () 推导
        // 解决方案: 当用 () 时，必须显式写模板参数
        // std::vector<int> v2(5, 10);
        lesson::print_warn("CTAD 仅从 {} 或构造函数的(显式)实参推导, () 需要模板参数");
    }

    // 陷阱2: C 风格字符串被推导为 const char*
    {
        lesson::print_subtitle("陷阱2: const char* vs std::string");
        // std::vector v = {"hello", "world"};
        // 推导为 vector<const char*>，不是 vector<string>!
        // 解决方法: 用 string_view 字面量 (C++17) 或显式转换
        using namespace std::string_literals;
        auto v = std::vector{"hello"s, "world"s};  // 显式 string 字面量
        static_assert(std::is_same_v<decltype(v), std::vector<std::string>>);
        std::println("v[0]='{}'  v[1]='{}'", v[0], v[1]);
        lesson::print_warn("const char* 不会自动推导为 string, 需用 \"\"s 字面量");
    }

    // 陷阱3: CTAD 不会进行隐式转换
    {
        lesson::print_subtitle("陷阱3: CTAD 不做隐式转换");
        // 编译器从实参推导 T，不会为匹配而转换实参类型
        // std::pair p(1, "hello");  → pair<int, const char*> (不是 pair<int,string>)
        auto p1 = std::pair(1, "hello");           // pair<int, const char*>
        auto p2 = std::pair(1, std::string("hello")); // pair<int, string>
        static_assert(!std::is_same_v<decltype(p1), decltype(p2)>); // 不同类型!
        lesson::print_note("CTAD 推导出的类型 = 实参的精确类型, 不做隐式转换");
    }

    // 陷阱4: 缺少推导指引导致编译失败
    {
        lesson::print_subtitle("陷阱4: 缺少推导指引 = 编译失败");
        // 如果没有推导指引, 某些构造方式会失败:
        // DynArray arr(src.begin(), src.end()); // 编译错误! 无法推导 T
        // 解决方案: 添加对应的推导指引 (如 Part 2 所示)
        lesson::print_warn("迭代器范围构造/某些构造函数模式需要手动添加推导指引");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习任务                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void exercises() {
    lesson::print_header("练习任务");

    // 练习1: 定义一个模板类 MyPair<T, U>，享受隐式 CTAD
    lesson::print_subtitle("练习1: 自定义模板类的隐式 CTAD");
    // 提示: template<typename T, typename U> struct MyPair { T first; U second; };
    //       MyPair p{42, 3.14}; // CTAD → MyPair<int, double>

    // 练习2: 为 MyPair 添加从 C 风格字符串到 std::string 的推导指引
    lesson::print_subtitle("练习2: 自定义推导指引");
    // 提示: MyPair(const char*, const char*) -> MyPair<std::string, std::string>;

    // 练习3: 使用聚合 CTAD 创建一个模板化的 RGB 颜色结构体
    lesson::print_subtitle("练习3: 聚合 CTAD 创建模板化 RGB");
    // 提示: template<typename T> struct RGB { T r, g, b; };
    //       RGB c{255, 128, 0};  // CTAD → RGB<int>
}

int main() {
    part1_ctad_basics();
    part2_deduction_guides();
    part3_aggregate_ctad();

    pitfalls();
    exercises();
    return 0;
}

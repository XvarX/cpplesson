// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 07_modern_syntax — 现代语法糖                                        ║
// ║  课题: 统一初始化 {} / initializer_list / 窄化转换阻止 / 指定初始化         ║
// ║  学习目标:                                                                  ║
// ║    1. 掌握统一初始化 {} 语法及与 () 的区别                                  ║
// ║    2. 理解 std::initializer_list 的用途与陷阱                               ║
// ║    3. 知道 {} 如何自动阻止窄化转换                                          ║
// ║    4. 学会 C++20 指定初始化 (designated initializers)                       ║
// ║    5. 理解聚合初始化的条件                                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "modern_syntax/init_and_designated.hpp"
#include "shared/lesson_utils.hpp"

#include <print>
#include <vector>
#include <string>
#include <initializer_list>
#include <map>
#include <numeric>  // std::accumulate

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: 统一初始化 {} —— 告别"最令人烦恼的解析"                           ║
// ║  WHAT: T obj{args} / T obj = {args} —— 使用花括号初始化一切               ║
// ║  WHY:  ① 解决"most vexing parse" (函数声明 vs 变量定义二义性)              ║
// ║        ② 一种语法初始化所有类型 (内置/聚合/类/容器)                       ║
// ║        ③ 自动阻止窄化转换                                                  ║
// ║  WHEN: 任何初始化场景; 聚合类型; 容器; 避免解析二义性                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_uniform_init() {
    lesson::print_header("Part 1: 统一初始化 {} 语法");

    // ── 1.1 基本用法: 一种语法初始化所有 ──
    {
        lesson::print_subtitle("1.1  统一语法初始化各种类型");
        int    a{42};                    // 内置类型
        double b{3.14};
        std::string s{"你好"};           // 类类型
        std::vector<int> v{1, 2, 3};     // 容器

        std::println("a={}  b={:.2f}  s={}  v[0]={}", a, b, s, v[0]);
    }

    // ── 1.2 解决"最令人烦恼的解析" ──
    {
        lesson::print_subtitle("1.2  解决 Most Vexing Parse");

        // 经典陷阱: 以下代码声明了一个函数而不是变量!
        // std::string s();               // 函数声明! 不是变量定义!

        // {} 可以解决:
        std::string s{};                  // 明确: 默认构造一个 string
        std::vector<int>  v{};            // 明确: 空 vector

        // 另一个经典示例: 传入临时对象
        using modern_syntax::Timer;
        // Timer t(Timer());              // 又是函数声明!
        Timer t{Timer{}};                 // OK: 明确初始化
        std::println("s='{}'  v.size()={}  t.seconds={}", s, v.size(), t.seconds);
    }

    // ── 1.3 {} vs () 的关键区别 ──
    {
        lesson::print_subtitle("1.3  {} 和 () 的区别");
        // () 允许窄化转换, {} 禁止窄化转换
        // int x(3.14);                   // OK: 但截断为 3 (危险)
        // int y{3.14};                   // 编译错误! 窄化转换被阻止

        // () 优先匹配构造函数, {} 优先匹配 initializer_list 构造函数
        std::vector<int> va(3, 10);       // () → 3 个元素,每个都是 10: [10,10,10]
        std::vector<int> vb{3, 10};       // {} → initializer_list: [3, 10]
        std::println("va: [{}, {}, {}]  (3个元素)", va[0], va[1], va[2]);
        std::println("vb: [{}, {}]        (2个元素)", vb[0], vb[1]);
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: std::initializer_list —— 让自定义类也能用 {}                       ║
// ║  WHAT: 编译器为 {} 列表生成的轻量级数组视图                                ║
// ║  WHY:  自定义容器/类接受可变数量的同类型参数                               ║
// ║  WHEN: 构造函数接受可变参数; 数学向量/矩阵库; DSL-like API                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_initializer_list() {
    lesson::print_header("Part 2: std::initializer_list");

    // ── 2.1 自定义类接受 {} 初始化 ──
    {
        lesson::print_subtitle("2.1  自定义类接受 {} 语法");
        modern_syntax::Vec3 v1{1.0, 2.0, 3.0};      // 调用 initializer_list 构造函数
        modern_syntax::Vec3 v2{4.0, 5.0};            // 只提供 2 个值也 OK (z=0)
        v1.print();                    // 输出: Vec3(1.0, 2.0, 3.0)
        v2.print();                    // 输出: Vec3(4.0, 5.0, 0.0)
    }

    // ── 2.2 initializer_list 的特性 ──
    {
        lesson::print_subtitle("2.2  initializer_list 只是轻量视图");
        std::initializer_list<int> il = {10, 20, 30, 40, 50};

        // 可以遍历
        std::print("元素: ");
        for (auto v : il) std::print("{} ", v);
        std::println("");

        // 只读 —— 元素是 const 的
        // *(il.begin()) = 99;         // 错误! initializer_list 元素不可修改

        // 不拥有数据 —— 拷贝 initializer_list 不复制底层数组
        auto il2 = il;                  // 浅拷贝: 指向同一块数据
        std::println("il.size()={}  il2.size()={}", il.size(), il2.size());

        lesson::print_note("initializer_list 不拥有数据,只是指向编译器生成的数组");
    }

    // ── 2.3 函数参数使用 initializer_list ──
    {
        lesson::print_subtitle("2.3  函数接受 initializer_list 参数");
        auto sum = [](std::initializer_list<int> nums) -> int {
            return std::accumulate(nums.begin(), nums.end(), 0);
        };
        std::println("sum(1,2,3,4,5) = {}", sum({1, 2, 3, 4, 5})); // 输出: 15
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: 窄化转换阻止                                                        ║
// ║  WHAT: {} 初始化自动禁止一切可能丢失信息的隐式转换                          ║
// ║  WHY:  防止数据截断和精度丢失 —— 编译器在编译期就捕获这类错误              ║
// ║  WHEN: 总是使用 {} 初始化来获得编译期安全检查                               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_narrowing_prevention() {
    lesson::print_header("Part 3: {} 自动阻止窄化转换");

    // ── 3.1 哪些转换会被阻止 ──
    {
        lesson::print_subtitle("3.1  编译器阻止的窄化转换示例");
        // 以下全部是编译错误(仅做演示,注释掉):
        // int a{3.14};                // double → int: 丢失小数部分
        // char c{9999};               // int → char: 超出范围
        // unsigned int u{-5};         // 负数 → 无符号: 语义改变
        // float f{1e100};             // double → float: 可能溢出

        // 正确的初始化方式:
        int  a = 3;                       // 或 int a{3}
        char c = 127;                     // 在 char 范围内
        unsigned int u = 0;
        float f = 1.0f;

        std::println("a={}  c={}  u={}  f={}", a, c, u, f);
        lesson::print_note("编译期安全检查 = 零运行时开销! 用 {} 受益");
    }

    // ── 3.2 什么时候用 () 更好 ──
    {
        lesson::print_subtitle("3.2  特殊情况: 用 () 避开 initializer_list 歧义");
        // 当类同时有 initializer_list 构造函数和其他构造函数时
        // {} 优先匹配 initializer_list 版本, 可能导致歧义
        std::vector<int> v1(10, 5);       // () → 10 个元素, 每个 = 5
        std::vector<int> v2{10, 5};       // {} → 2 个元素 [10, 5]
        std::println("v1.size()={}  v2.size()={}", v1.size(), v2.size());
        // 输出: v1.size()=10  v2.size()=2
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: 聚合初始化 + C++20 指定初始化                                     ║
// ║  WHAT: 直接用 {} 按成员顺序初始化聚合类型; C++20 支持 .name = value       ║
// ║  WHY:  简化结构体初始化; 指定初始化让代码自文档化，不依赖成员顺序          ║
// ║  WHEN: POD/聚合类型(无自定义构造函数的结构体); 配置结构体                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_aggregate_and_designated() {
    lesson::print_header("Part 4: 聚合初始化 & C++20 指定初始化");

    using namespace modern_syntax;

    // ── 4.1 传统聚合初始化 ──
    {
        lesson::print_subtitle("4.1  聚合初始化 (按顺序)");
        Point p1{3.0, 4.0, "原点"};      // 必须按声明顺序
        std::println("p1: ({:.1f}, {:.1f}) label='{}'", p1.x, p1.y, p1.label);
    }

    // ── 4.2 C++20 指定初始化 ──
    {
        lesson::print_subtitle("4.2  C++20 指定初始化 (.成员名 = 值)");
        // 显式指定成员名 —— 不依赖顺序! 自文档化!

        Point p2{.x = 10.0, .y = 20.0, .label = "终点"};
        std::println("p2: ({:.1f}, {:.1f}) label='{}'", p2.x, p2.y, p2.label);

        // 也可以只初始化部分成员 (其余使用默认值或零初始化)
        Config cfg{.port = 9090, .timeout = 60.0};
        std::println("Config: port={} threads={} use_ssl={} timeout={:.1f}",
                     cfg.port, cfg.threads, cfg.use_ssl, cfg.timeout);
        // 输出: port=9090 threads=4 use_ssl=true timeout=60.0

        // 跳过一些成员——其余零初始化
        Point p3{.x = 5.0};     // y = 0, label = ""
        std::println("p3: ({:.1f}, {:.1f}) label='{}'", p3.x, p3.y, p3.label);
    }

    // ── 4.3 指定初始化的约束 ──
    {
        lesson::print_subtitle("4.3  指定初始化的约束 (C++20)");
        // C++ 要求按声明顺序指定 (与 C99 不同, C 允许任意顺序)
        // Point p{.y = 1.0, .x = 2.0, .label = "错"}; // 错误! 顺序不对

        // 不能混合指定与非指定初始化
        // Point p{.x = 1.0, 2.0, "坏"};  // 错误! 不能混合

        lesson::print_note("C++20 指定初始化: 必须按声明顺序, 不能混合指定/非指定");
        lesson::print_note("注意: 这与 C99 不同 (C99 允许任意顺序)");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void pitfalls() {
    lesson::print_header("常见陷阱");

    // 陷阱1: initializer_list 构造函数的"劫持"
    {
        lesson::print_subtitle("陷阱1: initializer_list 构造函数优先级过高");
        std::vector<int> v1{5};       // 不是 5 个 0，而是 1 个元素 5!
        std::vector<int> v2(5);       // () → 5 个 0
        std::println("v1={}个元素[0]={}  v2={}个元素", v1.size(), (v1.empty()?0:v1[0]), v2.size());
        lesson::print_warn("{} 优先匹配 initializer_list 构造函数 ");
    }

    // 陷阱2: auto 与 {} 的歧义
    {
        lesson::print_subtitle("陷阱2: auto + {} 的类型推导");
        // auto x = {1, 2, 3};        // 在 C++17 中推导为 initializer_list<int>
        // 最好显式声明, 或使用 C++17 的 guaranteed copy elision
        auto x = 42;                   // auto + {} 的行为易变, 推荐直接用 =
        std::println("x = {}", x);
    }

    // 陷阱3: 指定初始化不能用于非聚合类型
    {
        lesson::print_subtitle("陷阱3: 指定初始化仅适用于聚合类型");
        // struct Bad { private: int x; public: Bad(int v):x(v){} };
        // Bad b{.x = 10};            // 错误! 有构造函数就不是聚合类型
        lesson::print_warn("有自定义构造函数/private成员的类不能用指定初始化");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习任务                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void exercises() {
    lesson::print_header("练习任务");

    // 练习1: 写一个接受 initializer_list 的 sum 函数 (已完成在2.3)
    // 练习2: 用指定初始化创建一个包含 name/age/score 的 Student 结构体
    lesson::print_subtitle("练习2: 指定初始化 Student 结构体");
    // 提示: struct Student { std::string name; int age; double score; };
    //       Student s{.name="小明", .age=18, .score=95.5};

    // 练习3: 比较 () 和 {} 对 vector 初始化的不同行为
    lesson::print_subtitle("练习3: vector () vs {}");
    // 提示: vector<int>(10) vs vector<int>{10}
}

int main() {
    part1_uniform_init();
    part2_initializer_list();
    part3_narrowing_prevention();
    part4_aggregate_and_designated();

    pitfalls();
    exercises();
    return 0;
}

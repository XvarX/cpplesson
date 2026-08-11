// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 08_move_and_lambda                                                     ║
// ║  主题: std::function、std::bind、std::mem_fn、std::invoke、reference_wrapper ║
// ║  学习目标:                                                                   ║
// ║    1. 掌握 std::function — 类型擦除的可调用对象包装器                        ║
// ║    2. 理解 std::bind 与 placeholders 的绑定机制                              ║
// ║    3. 学会 std::mem_fn — 成员函数指针的现代化替代                            ║
// ║    4. 使用 std::invoke(C++17) 统一调用各种可调用对象                         ║
// ║    5. 理解 std::reference_wrapper — 在值语义容器中保存引用                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "shared/lesson_utils.hpp"
#include "move_lambda/functional.hpp"
#include <print>
#include <string>
#include <vector>
#include <functional>   // function, bind, mem_fn, invoke, ref, cref
#include <algorithm>

using namespace move_lambda;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: std::function — 万能可调用对象包装器
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::function<ReturnType(ArgTypes...)> 可以包装任何可调用对象:
//        函数指针、Lambda、函数对象、std::bind 结果等
//        内部使用类型擦除(type erasure)技术
// WHY:   作为回调的通用类型, 解耦调用方和被调用方的具体类型
// WHEN:   需要存储回调(如事件系统、命令队列)、回调必须是多态的(不同具体类型)

void part1_std_function() {
    lesson::print_subtitle("Part 1: std::function — 万能包装器");

    // std::function 可以包装任何签名为 int(int) 的可调用对象
    std::vector<std::function<int(int)>> operations;

    // 1. 包装 Lambda
    operations.push_back([](int x) { return x + 1; });

    // 2. 包装自由函数
    operations.push_back(free_func);

    // 3. 包装函数对象
    operations.push_back(Functor{3});

    // 统一调用 — 调用方不需要知道具体类型
    for (size_t i = 0; i < operations.size(); ++i) {
        std::println("operations[{}](10) = {}", i, operations[i](10));
    }
    // 预期: 11(10+1), 20(10*2), 30(10*3)

    // 检查是否为空
    std::function<void()> empty_fn;          // 默认构造为空
    std::println("empty_fn 是否为空? {}", !empty_fn);  // true

    // 存储有捕获的 Lambda(无法转为函数指针, 但可以存入 function)
    int threshold = 5;
    std::function<bool(int)> predicate = [threshold](int x) {
        return x > threshold;
    };
    std::println("predicate(10) = {}, predicate(3) = {}",
                 predicate(10), predicate(3));
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: std::bind 与 placeholders — 参数绑定/重排
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::bind 创建一个"绑定后"的可调用对象: 固定部分参数, 重排参数顺序
//        _1, _2, ... 是占位符, 代表绑定后的参数位置
// WHY:   适配函数签名(如把 void(int,int) 适配成 void(int)), 参数重排
// WHEN:   需要适配旧接口时; 但现代 C++ 推荐用 Lambda 替代 bind(更直观)

void part2_bind() {
    lesson::print_subtitle("Part 2: std::bind 与 placeholders");

    using namespace std::placeholders;  // _1, _2, _3, ...

    // 绑定第一个参数, 只留两个参数
    auto bind_first = std::bind(print_three, 100, _1, _2);
    std::println("bind_first(10, 20): 第一个参数固定为 100");
    bind_first(10, 20);  // print_three(100, 10, 20)

    // 参数顺序重排: 翻转参数顺序
    auto reversed = std::bind(print_three, _3, _2, _1);
    std::println("reversed(1, 2, 3): 参数顺序翻转");
    reversed(1, 2, 3);   // print_three(3, 2, 1)

    // 绑定到成员变量或嵌套调用
    auto double_first = std::bind(print_three,
                                  std::bind(std::multiplies<int>{}, _1, 2),  // _1 * 2
                                  _2, _3);
    std::println("double_first(10, 20, 30): 第一个参数翻倍");
    double_first(10, 20, 30);  // print_three(20, 20, 30)

    // ⚠️ 现代 C++ 建议: 优先用 Lambda 代替 bind
    // bind 版本:  auto f = std::bind(foo, _2, _1);
    // Lambda 版: auto f = [](auto a, auto b) { foo(b, a); };  // 更清晰!
    std::println("\n现代建议: Lambda 比 bind 更清晰、更易读、编译器优化更好");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: std::mem_fn — 成员函数指针的现代包装
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::mem_fn 将成员函数指针包装成可调用对象
//        调用时第一个参数是对象(或其指针/智能指针/reference_wrapper)
// WHY:   替代晦涩的 .* 和 ->* 语法, 与标准算法更兼容
// WHEN:   需要将成员函数作为算法参数传递时

void part3_mem_fn() {
    lesson::print_subtitle("Part 3: std::mem_fn");

    std::vector<Person> people = {
        {"张三", 25},
        {"李四", 16},
        {"王五", 30},
        {"赵六", 12}
    };

    // 传统写法: 用 Lambda 调用成员函数
    std::println("传统 Lambda 方式:");
    std::ranges::for_each(people, [](const Person& p) { p.greet(); });

    // std::mem_fn 方式: 更简洁
    std::println("std::mem_fn 方式:");
    std::ranges::for_each(people, std::mem_fn(&Person::greet));

    // mem_fn 配合算法: 按年龄排序
    std::ranges::sort(people, std::less<int>{},
                      std::mem_fn(&Person::age));
    // 等价于: [](const Person& p) { return p.age; }

    // mem_fn 用于过滤: 统计成年人
    auto adult_count = std::ranges::count_if(people, std::mem_fn(&Person::isAdult));
    std::println("成年人数量: {}", adult_count);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: std::invoke(C++17) — 统一调用接口
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::invoke(f, args...) 能统一调用:
//        - 普通可调用对象: f(args...)
//        - 成员函数指针:   (obj.*pmf)(args...) 或 (ptr->*pmf)(args...)
//        - 成员数据指针:   obj.*pmd  或  ptr->*pmd
// WHY:   消除调用方式的不一致, 泛型代码中统一处理
// WHEN:   编写泛型库或需要统一调用方式时

void part4_invoke() {
    lesson::print_subtitle("Part 4: std::invoke — 统一调用");

    Calculator calc{10};

    // 1. 调用普通函数
    std::println("invoke(lambda): {}",
                 std::invoke([](int x) { return x * x; }, 5));

    // 2. 调用成员函数 — 传对象
    std::println("invoke(&Calculator::add, calc, 7): {}",
                 std::invoke(&Calculator::add, calc, 7));       // 10+7=17

    // 3. 调用成员函数 — 传指针
    std::println("invoke(&Calculator::multiply, &calc, 6): {}",
                 std::invoke(&Calculator::multiply, &calc, 6)); // 10*6=60

    // 4. 调用成员函数 — 传 reference_wrapper
    auto ref = std::ref(calc);
    std::println("invoke(&Calculator::add, ref, 3): {}",
                 std::invoke(&Calculator::add, ref, 3));        // 10+3=13

    // 5. 访问成员数据指针
    std::println("invoke(&Calculator::value, calc): {}",
                 std::invoke(&Calculator::value, calc));        // 100

    // 泛型代码中的 invoke — 不必区分调用方式
    auto call_on = [](auto&& obj, auto&& memfn, auto&&... args) {
        return std::invoke(std::forward<decltype(memfn)>(memfn),
                           std::forward<decltype(obj)>(obj),
                           std::forward<decltype(args)>(args)...);
    };
    std::println("泛型 invoke: {}", call_on(calc, &Calculator::add, 5));
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: std::reference_wrapper — 值语义容器中的引用
// ═══════════════════════════════════════════════════════════════════════════════
// WHAT:  std::ref(obj)   → std::reference_wrapper<T>, 像指针但保证非空
//        std::cref(obj)  → std::reference_wrapper<const T>
//        reference_wrapper 在值语义容器(如 vector)中保存"引用"
// WHY:   有些场景只能传值(如 vector 元素, std::bind 参数), 但你想传引用
// WHEN:   - 需要在 vector 中存储对象的引用(而非拷贝)
//         - 用 std::thread 传引用参数
//         - 用 std::bind 绑定引用参数

void part5_reference_wrapper() {
    lesson::print_subtitle("Part 5: std::reference_wrapper");

    // 问题: vector 只能存值, 不能存引用
    // std::vector<int&> refs;  // ❌ 编译错误! 引用不可作为容器元素

    // 解决: 用 reference_wrapper
    int a = 10, b = 20, c = 30;
    std::vector<std::reference_wrapper<int>> refs = {a, b, c};

    // 通过 reference_wrapper 修改原变量
    for (auto& ref : refs) {
        ref.get() *= 2;  // 或直接用 ref *= 2 (有隐式转换)
    }
    std::println("通过 reference_wrapper 修改后: a={}, b={}, c={}", a, b, c);
    // 预期: a=20, b=40, c=60

    // std::ref/cref 配合 std::bind
    // 默认 bind 按值捕获, 用 std::ref 强制按引用
    int val = 100;
    auto bound = std::bind(modify, std::ref(val));
    bound();
    std::println("bind + std::ref: val 从 100 变为 {}", val);

    // std::ref/cref 配合 std::thread
    // thread 按值传递参数, std::ref 实现真正的引用传递

    // 通过 get() 修改引用的原变量
    for (auto& r : refs) {
        r.get() = 0;  // 修改 ref 引用的 int 原值
    }
    std::println("归零后: a={}, b={}, c={}", a, b, c);

    std::println("核心: reference_wrapper = 安全指针(保证非空) + 引用语义");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
void pitfalls() {
    lesson::print_header("常见陷阱");

    std::println("陷阱1: std::function 的开销");
    std::println("  → 类型擦除有虚函数调用的开销(有时可忽略, 但热路径谨慎)");
    std::println("  → 小对象优化(SBO)可能失效, 导致堆分配");
    std::println("  替代: 使用模板参数(编译期多态)无额外开销");

    std::println("陷阱2: std::bind 的引用语义陷阱");
    std::println("  auto f = std::bind(foo, arg);  // arg 按值拷贝!");
    std::println("  auto f = std::bind(foo, std::ref(arg)); // 需要引用时显式用 ref");

    std::println("陷阱3: reference_wrapper 悬垂");
    std::println("  std::vector<std::reference_wrapper<int>> v;");
    std::println("  {{ int x=42; v.push_back(std::ref(x)); }}  // x 已销毁, 悬垂!");
    std::println("  → reference_wrapper 不延长生命周期, 和裸引用一样危险");

    std::println("陷阱4: std::mem_fn 不能替代 Lambda 做复杂操作");
    std::println("  → 需要额外参数或条件逻辑时, Lambda 更灵活");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
void exercises() {
    lesson::print_header("练习");
    std::println("1. 用 std::function 实现一个简单的回调注册系统(register/listener/call)");
    std::println("2. 用 std::bind 复现 Part 2 中的参数重排, 然后用 Lambda 改写, 对比可读性");
    std::println("3. 写一个函数, 接受 vector<Person>&, 用 std::mem_fn 删除所有未成年人");
    std::println("4. 用 std::invoke 实现一个泛型的 for_each_member 函数");
    std::println("5. 用 reference_wrapper 实现: vector 中存多个对象的引用, 统一修改它们");
}

int main() {
    lesson::print_header("05: std::function、bind、mem_fn、invoke、reference_wrapper");

    part1_std_function();
    part2_bind();
    part3_mem_fn();
    part4_invoke();
    part5_reference_wrapper();
    pitfalls();
    exercises();

    return 0;
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 07_modern_syntax — 现代语法糖                                        ║
// ║  课题: auto / decltype / decltype(auto) / 尾置返回类型                      ║
// ║  学习目标:                                                                  ║
// ║    1. 掌握 auto 的类型推导规则(去掉引用和顶层 cv 限定符)                    ║
// ║    2. 理解 decltype 的"照搬表达式类型"语义                                  ║
// ║    3. 掌握 decltype(auto) 的综合使用场景                                    ║
// ║    4. 学会尾置返回类型(->)让泛型代码更清晰                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "modern_syntax/auto_decltype.hpp"
#include "shared/lesson_utils.hpp"

#include <print>       // C++23 std::println
#include <string>
#include <vector>
#include <type_traits> // std::is_same_v
#include <utility>     // std::declval
#include <optional>

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: auto 类型推导规则                                                  ║
// ║  WHAT: auto 让编译器从初始化表达式推导变量类型                              ║
// ║  WHY:  减少冗长类型名、泛型编程时无需手写类型、避免类型不匹配               ║
// ║  WHEN: 初始化时类型显而易见; 迭代器/模板/泛型 lambda; 避免类型截断          ║
// ║  关键规则——auto 会:                                                         ║
// ║    - 去掉引用 (&)                                                           ║
// ║    - 去掉顶层 const (但保留底层 const, 如指向 const 的指针)                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_auto_deduction() {
    lesson::print_header("Part 1: auto 类型推导规则");

    // ── 1.1 基本推导 ──
    {
        lesson::print_subtitle("1.1  基本推导");
        auto x = 42;            // x 是 int
        auto y = 3.14;          // y 是 double
        auto s = "hello";       // s 是 const char*
        std::println("x={}  y={}  s={}", x, y, s);
    }

    // ── 1.2 去掉引用 ──
    {
        lesson::print_subtitle("1.2  去掉引用");
        int a = 10;
        int& ref = a;
        auto b = ref;           // b 是 int，不是 int&！引用被剥离了
        b = 20;                 // 修改 b，不影响 a
        std::println("a={}  b={}", a, b);       // 输出: a=10  b=20
        // 想要引用必须显式写 auto&
        auto& c = ref;          // c 是 int&，绑定到 a
        c = 30;
        std::println("a={}  c={}", a, c);       // 输出: a=30  c=30
    }

    // ── 1.3 去掉顶层 const ──
    {
        lesson::print_subtitle("1.3  去掉顶层 const");
        const int ci = 100;
        auto d = ci;            // d 是 int，顶层 const 被抛弃
        d = 200;                // OK
        std::println("ci={}  d={}", ci, d);     // 输出: ci=100  d=200

        // 但底层 const（指向 const 的指针）会保留
        const int* pci = &ci;   // 指向 const int 的指针
        auto e = pci;           // e 是 const int*，底层 const 保留
        // *e = 999;            // 错误！不能通过 const int* 修改
        std::println("*e={}", *e);
    }

    // ── 1.4 保留 const 和 ref 的方法 ──
    {
        lesson::print_subtitle("1.4  明确写 const auto& / auto&& 保留属性");
        const std::string name = "Ecks";
        const auto& cr = name;      // cr 是 const std::string& —— 都保留
        auto&&      fwd = name;     // fwd 是转发引用 (forwarding reference)
        std::println("cr={}  fwd={}", cr, fwd);
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: decltype —— "照搬表达式类型"                                      ║
// ║  WHAT: decltype(expr) 返回 expr 的精确类型（包括引用和 cv 限定符）          ║
// ║  WHY:  auto 会丢弃引用/cv，decltype 不丢弃; 泛型中需要精准匹配返回值      ║
// ║  WHEN: 从另一个表达式"继承"类型; 写类型萃取; 推导模板函数的返回类型         ║
// ║  decltype 的括号陷阱:                                                       ║
// ║    decltype(变量)   → 变量声明的类型                                        ║
// ║    decltype((变量)) → 变量的类型 + 左值引用 (双层括号始终得到引用!)        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_decltype() {
    lesson::print_header("Part 2: decltype 精确类型照搬");

    int x = 10;
    const int cx = 20;
    int& rx = x;

    // decltype(变量名) —— 照搬声明类型
    decltype(x)  a = 42;        // a 是 int
    decltype(cx) b = 99;        // b 是 const int
    decltype(rx) c = x;         // c 是 int&，绑定到 x

    // decltype(表达式) —— 表达式是什么类型就是什么类型
    decltype(x + 1.0) d = 3.14; // d 是 double (int + double → double)

    // ★ 括号陷阱 ★
    // decltype((x)) 是 int&，而不是 int！
    decltype((x)) e = x;        // e 是 int&，因为 (x) 是一个左值表达式
    e = 100;
    std::println("x={}  a={}  b={}  d={:.2f}", x, a, b, d);
    // 输出: x=100  a=42  b=99  d=3.14

    // 验证类型
    static_assert(std::is_same_v<decltype(a), int>);
    static_assert(std::is_same_v<decltype(b), const int>);
    static_assert(std::is_same_v<decltype(c), int&>);
    static_assert(std::is_same_v<decltype((x)), int&>);   // 双层括号得引用！
    lesson::print_note("static_assert 全部通过: 类型推导正确");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: decltype(auto) —— C++14 融合两者                                  ║
// ║  WHAT: decltype(auto) 是 auto 位置上的 decltype 语义                       ║
// ║  WHY:  auto 会丢引用 → 返回引用时出错; decltype 需要重复表达式(DRY)        ║
// ║  WHEN: 完美转发返回值的函数; 泛型代码返回引用/const 时                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_decltype_auto() {
    lesson::print_header("Part 3: decltype(auto) 完美保留类型");

    std::vector<int> nums = {10, 20, 30};

    // auto 返回拷贝 → 修改不影响原容器
    auto copy = modern_syntax::dangerous_get_element(nums, 1);
    copy = 999;
    std::println("nums[1] = {}  (危险版本未修改原值)", nums[1]); // 输出: 20

    // decltype(auto) 返回引用 → 修改直接写回容器
    decltype(auto) ref = modern_syntax::safe_get_element(nums, 1);
    ref = 999;
    std::println("nums[1] = {}  (decltype(auto) 正确修改了原值)", nums[1]); // 输出: 999

    // 同样适用于局部变量
    const std::string msg = "你好";
    decltype(auto) s = msg;             // s 是 const std::string (顶层 const 保留!)
    // s = "修改";                      // 错误，s 是 const
    std::println("s = {}", s);
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: 尾置返回类型 (trailing return type)                                ║
// ║  WHAT: auto func(args) -> ReturnType { ... }                                ║
// ║  WHY:  返回类型可能依赖参数名 → 必须先声明参数再写返回类型                  ║
// ║  WHEN: 模板函数返回类型依赖参数类型; Lambda 表达式; 提高可读性             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_trailing_return() {
    lesson::print_header("Part 4: 尾置返回类型");

    auto r1 = modern_syntax::add(3, 4.5);          // int + double → double
    auto r2 = modern_syntax::add(1.2f, 5);        // float + int → float
    auto r3 = modern_syntax::multiply(3L, 4.0);   // long * double → double

    std::println("add(3, 4.5)      = {:.1f}", r1);    // 输出: 7.5
    std::println("add(1.2f, 5)     = {:.1f}", r2);    // 输出: 6.2
    std::println("multiply(3L, 4.0)= {:.1f}", r3);    // 输出: 12.0

    // 尾置返回类型在 lambda 中的妙用
    auto safe_divide = [](double a, double b) -> std::optional<double> {
        if (b == 0.0) return std::nullopt;
        return a / b;
    };
    if (auto r = safe_divide(10, 3)) {
        std::println("10/3 = {:.4f}", *r);
    }
    if (auto r = safe_divide(1, 0)) {
        std::println("1/0 = {}", *r);
    } else {
        std::println("1/0 = 除零错误 (无值)");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void pitfalls() {
    lesson::print_header("常见陷阱");

    // 陷阱1: auto 与代理类 (如 vector<bool>)
    {
        lesson::print_subtitle("陷阱1: vector<bool> 的 auto 陷阱");
        std::vector<bool> flags = {true, false, true};
        auto bit = flags[0];   // bit 不是 bool&，而是代理对象 std::_Bit_reference！
        // 解决方案: 使用 auto&& 或显式 bool
        // static_assert(std::is_same_v<decltype(bit), bool>); // 会失败！
        lesson::print_warn("vector<bool>::operator[] 返回代理对象，非 bool&");
        lesson::print_note("修复: 使用 auto&& 或显式类型 bool b = flags[0];");
    }

    // 陷阱2: 大括号初始化列表
    {
        lesson::print_subtitle("陷阱2: auto 不能推导 initializer_list");
        // auto x = {1, 2, 3};    // 在 C++17 起在某些场景下有歧义
        auto x = 42;              // OK: 直接用值
        std::initializer_list<int> il = {1, 2, 3}; // 显式写明
        std::println("x={}  il.size()={}", x, il.size());
    }

    // 陷阱3: decltype(auto) 双层括号
    {
        lesson::print_subtitle("陷阱3: decltype(auto) 中的双层括号");
        int n = 5;
        decltype(auto) a = n;       // a 是 int
        decltype(auto) b = (n);     // b 是 int& ！—— 注意括号
        n = 10;
        std::println("n={}  a={}  b={}", n, a, b); // b 跟随 n 变化
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习任务                                                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void exercises() {
    lesson::print_header("练习任务");

    // 练习1: 写一个函数 getMaxValue，接收 vector 并返回最大值的引用
    //        使用 decltype(auto) 保留引用语义
    lesson::print_subtitle("练习1: decltype(auto) 返回引用");
    // 提示: template<typename T>
    //       decltype(auto) getMaxValue(std::vector<T>& v) { return v[...]; }

    // 练习2: 用 auto 和 decltype 实现一个简单的类型萃取
    lesson::print_subtitle("练习2: 判断两个变量相加后的类型");
    // 提示: 使用 decltype(a + b) 获取类型

    // 练习3: 写一个尾置返回类型的模板函数 min(T a, U b)
    lesson::print_subtitle("练习3: 尾置返回类型实现 min");
    // 提示: auto min(T a, U b) -> decltype(a < b ? a : b)
}

int main() {
    part1_auto_deduction();
    part2_decltype();
    part3_decltype_auto();
    part4_trailing_return();

    pitfalls();
    exercises();
    return 0;
}

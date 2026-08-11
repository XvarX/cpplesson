// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  01_fundamentals 示例 — 指针、引用、nullptr、函数指针、void*                  ║
// ║  目标: 掌握指针/引用的区别、const 修饰、函数指针、以及 void* 的使用场景     ║
// ║  前置: 01_types_and_literals, 03_arrays_and_strings                           ║
// ║  对应库头文件: fundamentals/pointers_refs.hpp                                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <memory>       // nullptr_t, std::unique_ptr (不在这里用, 仅展示存在)
#include <cstddef>      // std::size_t
#include <functional>   // std::function (更现代的函数包装)
#include "shared/lesson_utils.hpp"
#include "fundamentals/pointers_refs.hpp"

int main() {
    lesson::print_header("04 指针、引用、nullptr、函数指针、void*");

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 1: 指针 — 存储地址的变量
    // ═══════════════════════════════════════════════════════════════════════════════
    // 指针是什么: 一个变量，存储的是另一个对象的内存地址。
    // 为什么需要: 1) 间接访问和修改数据  2) 动态内存分配  3) 实现数据结构
    //             4) C 风格 API 调用  5) 多态(基类指针指向派生类)
    // 核心操作:
    //   &var  → 取 var 的地址 (address-of)
    //   *ptr  → 解引用，获取指针指向的值 (dereference)
    //   ptr->member  → 通过指针访问成员 (等价于 (*ptr).member)
    lesson::print_subtitle("Part 1: 指针基础");

    int value   = 42;
    int* ptr    = &value;   // ptr 存储 value 的地址
    // 指针本身也有地址!
    int** ptr_to_ptr = &ptr;  // 指向指针的指针

    std::println("value        = {}", value);
    std::println("&value       = {}", static_cast<const void*>(&value));
    std::println("ptr          = {} (就是 &value)", static_cast<const void*>(ptr));
    std::println("*ptr         = {} (解引用, 就是 value)", *ptr);
    std::println("&ptr         = {} (指针自己的地址)", static_cast<const void*>(&ptr));
    std::println("**ptr_to_ptr = {} (两次解引用)", **ptr_to_ptr);

    // ── 通过指针修改原值 ──
    *ptr = 100;   // 修改 ptr 指向的内容
    std::println("通过 *ptr = 100 修改后, value = {}", value);  // 期望: 100

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 2: nullptr — 空指针的正确写法 (C++11)
    // ═══════════════════════════════════════════════════════════════════════════════
    // nullptr 是什么: 空指针字面量，类型是 std::nullptr_t。
    // 为什么不用 NULL: NULL 本质是 0 或 (void*)0，会造成函数重载歧义。
    //                  nullptr 有独立类型，永不与整数混淆。
    // 什么时候用: 1) 初始化指针 2) 表示"不指向任何东西" 3) 判空检查
    lesson::print_subtitle("Part 2: nullptr vs NULL");

    int* np = nullptr;       // ✅ 现代写法 (C++11+)
    // int* bad = NULL;      // ⚠️ 不要这样写! 用 nullptr

    // nullptr 的类型安全
    static_assert(std::is_same_v<decltype(nullptr), std::nullptr_t>);

    // ── 判空检查: 使用指针前，检查它是否为空 ──
    if (np == nullptr) {
        std::println("np 是空指针, 不能解引用");
    }
    // 经典判空写法:
    // if (ptr) { /* 非空 */ }  或  if (!ptr) { /* 空 */ }

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 3: 引用 — 变量的别名 (不是对象!)
    // ═══════════════════════════════════════════════════════════════════════════════
    // 引用是什么: 已存在变量的另一个名字 (别名)，不是独立对象。
    // 为什么需要: 1) 函数参数避免拷贝  2) 让代码更简洁 (不需要 * 和解引用)
    //             3) 不能为空, 比指针更安全
    // 核心规则:
    //   - 引用必须在定义时初始化 (不能"空引用")
    //   - 引用不能"重绑定" — 一旦绑定，终身指向同一个对象
    //   - 语法上和普通变量一样使用 (不需要 *)
    //   引用 vs 指针:
    //   | 特性       | 指针 (T*)        | 引用 (T&)         |
    //   | 可为空     | ✅ (nullptr)     | ❌ 必须绑定到对象  |
    //   | 可重绑定   | ✅               | ❌ 终身绑定        |
    //   | 可做算术   | ✅               | ❌                 |
    //   | 语法       | 需要 * 解引用    | 直接当变量用      |
    lesson::print_subtitle("Part 3: 引用");

    int original = 10;
    int& ref = original;    // ref 是 original 的别名 (没有创建新对象)

    std::println("original = {}, ref = {}", original, ref);

    ref = 20;   // 修改 ref 就是修改 original
    std::println("ref = 20 后, original = {}", original);  // 期望: 20

    original = 30;
    std::println("original = 30 后, ref = {}", ref);       // 期望: 30

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 4: const 指针 与 const 引用
    // ═══════════════════════════════════════════════════════════════════════════════
    // const 修饰指针/引用的意义: 承诺不修改所指向/引用的对象。
    // 这是 C++ const-correctness 的核心 — 用类型系统保证不去改不该改的数据。
    lesson::print_subtitle("Part 4: const 指针与 const 引用");

    int x = 50;
    const int* cp = &x;      // 指向 const int: 不能通过 cp 修改 x
    // *cp = 60;             // ❌ 编译错误: cp 指向 const int
    int const* cp2 = &x;     // 等价写法: int const* 和 const int* 完全一样
    int* const pc = &x;      // const 指针: pc 不能指向别处, 但可以改 *pc
    *pc = 60;                // ✅ OK: 改的是 x 的值
    // pc = &value;          // ❌ 编译错误: pc 是 const 指针
    const int* const cpc = &x;  // 都不能改

    std::println("经过 *pc=60, x = {}", x);  // 60

    // ── const 引用 ──
    const int& cref = x;     // 不能通过 cref 修改 x
    // cref = 70;            // ❌ 编译错误
    // const 引用最常用于函数参数:
    // void print(const std::string& s);  // 不拷贝, 不修改 -> 高效且安全
    std::println("const 引用绑定到 x: {}", cref);

    // ── const 引用可以绑定到临时对象 (延长其生命周期) ──
    const int& temp_ref = 100;  // int 临时对象的生命周期被延长到 temp_ref 的生存期
    std::println("const 引用绑定到临时对象: {}", temp_ref);

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 5: 函数指针 — 把函数当作数据传递
    // ═══════════════════════════════════════════════════════════════════════════════
    // 函数指针是什么: 指向函数入口地址的指针，语法为 返回类型 (*指针名)(参数列表)
    // 为什么需要: 1) 回调函数 (callback)  2) 策略模式  3) C API
    // 现代替代: std::function (更灵活，支持 lambda)、auto + lambda (最简单)
    // 但看懂函数指针是读懂 C 库 / 旧代码的必要技能
    lesson::print_subtitle("Part 5: 函数指针");

    // ── 声明函数指针 ──
    int (*fp)(int, int) = nullptr;  // fp 是指向 int(int,int) 函数的指针
    fp = fundamentals::add;          // 使用库中的 add 函数
    std::println("fp(3, 5) = add(3, 5) = {}", fp(3, 5));   // 期望: 8

    fp = fundamentals::sub;
    std::println("fp(3, 5) = sub(3, 5) = {}", fp(3, 5));   // 期望: -2

    // ── 函数指针作为参数 (回调模式) ──
    std::println("calc(10, 5, add) = {}", fundamentals::calc(10, 5, fundamentals::add));  // 15
    std::println("calc(10, 5, mul) = {}", fundamentals::calc(10, 5, fundamentals::mul));  // 50

    // ── 函数指针数组: 运行时选择算法 ──
    int (*ops[])(int, int) = {fundamentals::add, fundamentals::sub, fundamentals::mul};
    std::println("函数指针数组 ops: add={}, sub={}, mul={}",
                 ops[0](8, 3), ops[1](8, 3), ops[2](8, 3));

    // ── 使用库中的类型别名 BinaryIntOp ──
    fundamentals::BinaryIntOp op_alias = fundamentals::add;
    std::println("通过类型别名调用: calc_v2(20, 10, add) = {}",
                 fundamentals::calc_v2(20, 10, op_alias));

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 6: 指针的指针 — 间接层
    // ═══════════════════════════════════════════════════════════════════════════════
    // 指针的指针在什么场景下有用:
    //   1) 函数需要修改调用者的指针 (例如 realloc 包装)
    //   2) 动态二维数组
    //   3) 某些 C API (例如 char** argv)
    lesson::print_subtitle("Part 6: 指针的指针");

    int val = 999;
    int*  p1 = &val;
    int** p2 = &p1;     // p2 存储 p1 的地址
    int*** p3 = &p2;    // 还可以继续 (但一般不超过两层)

    std::println("val = {}", val);
    std::println("*p1 = {}", *p1);        // 999
    std::println("**p2 = {}", **p2);      // 999
    std::println("***p3 = {}", ***p3);    // 999

    **p2 = 777;  // 通过两层指针修改 val
    std::println("**p2 = 777 后, val = {}", val);  // 777

    // ═══════════════════════════════════════════════════════════════════════════════
    // Part 7: void* — 无类型指针
    // ═══════════════════════════════════════════════════════════════════════════════
    // void* 是什么: 可以存放任意类型地址的指针，不能解引用 (编译器不知道类型)。
    // 为什么存在: C 语言遗留; 泛型回调 (qsort); 内存操作 (memcpy/memset)。
    // 什么时候用: 几乎只在和 C API 交互时需要。
    // 为什么避免: 丢失类型安全 — void* 可以指向任何东西，编译器无法帮你检查。
    lesson::print_subtitle("Part 7: void*");

    int    i = 10;
    double d = 3.14;
    void* vp = &i;     // void* 可以接收任何类型的地址
    std::println("void* 指向 int: {}", static_cast<const void*>(vp));
    vp = &d;           // 可以切换到另一个类型
    std::println("void* 指向 double: {}", static_cast<const void*>(vp));

    // 使用时必须转回具体类型 (危险! 你必须自己保证类型正确)
    int* ip = static_cast<int*>(vp);  // 把 void* 转回 int*
    // 但 vp 实际上指向 double! 这是未定义行为 ← 这就是 void* 的危险之处
    // 正确的做法:
    vp = &i;
    ip = static_cast<int*>(vp);
    std::println("正确转换: *ip = {}", *ip);  // 期望: 10

    // ── 常见陷阱 (Common Pitfalls) ────────────────────────────────────────────
    lesson::print_separator("常见陷阱");
    lesson::print_warn("陷阱1: 野指针 — 指向已释放/无效内存的指针。用 nullptr 初始化所有指针!");
    lesson::print_warn("陷阱2: 引用不能为空 — 不要试图创建空引用 (int& r = *ptr; ptr 是 nullptr 则 UB)");
    lesson::print_warn("陷阱3: 悬垂引用 — 引用指向的对象已被销毁, 常见于返回局部变量的引用");
    lesson::print_warn("陷阱4: void* 丢失类型信息 — 除非必要, 永远别用 void*");
    lesson::print_warn("陷阱5: 函数指针类型写错 — 导致未定义行为, 最好用 using 别名或 auto");
    lesson::print_warn("陷阱6: NULL 不是类型安全的 — 用 nullptr");
    lesson::print_warn("陷阱7: const 指针的声明从右往左读: int const* = 指向 const int");

    // ── 练习 ─────────────────────────────────────────────────────────────────
    lesson::print_separator("练习");
    std::println("1. 写一个 swap 函数, 分别用指针和引用实现, 对比用法差异");
    std::println("2. 模拟 linux 的 for_each: 遍历数组, 用函数指针对每个元素执行操作");
    std::println("3. 写一个 int** 二维动态数组的分配和释放 (模拟 C 代码)");
    std::println("4. 实验: 把一个 const int& 绑定到临时对象, 观察生命周期");
    std::println("5. 复习: 写出所有 const 指针的组合 (3种) 并标注每种的可读/可写方式");

    return 0;
}

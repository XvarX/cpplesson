// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — 类与面向对象编程                                      ║
// ║  课时: 08_friend_and_nested — 友元与嵌套类                                ║
// ║  学习目标:                                                                 ║
// ║    - 掌握友元函数的声明和使用场景                                          ║
// ║    - 理解友元类的用途和限制                                               ║
// ║    - 掌握嵌套类的定义和访问规则                                            ║
// ║    - 了解局部类和前向声明的使用场景                                        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "classes/friend_nested.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <vector>
#include <ostream>

using namespace lesson;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: 友元函数 — 授予函数访问私有成员的权限
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: friend 声明允许外部函数或类访问私有/保护成员
// 为什么: ① 运算符重载 (如 operator<<) 需要访问私有成员
//         ② 紧密协作的类/函数之间避免暴露过多公共接口
// 何时用: 非成员运算符重载、工厂函数、紧密耦合的工具函数

void part1_friend_function() {
    print_subtitle("Part 1: 友元函数");

    Point p1(0, 0), p2(3, 4);
    std::println("p1 和 p2 的距离: {:.0f}", distance(p1, p2)); // 期望: 5
    std::cout << "p1: " << p1 << ", p2: " << p2 << "\n";
    std::print("中点: ");
    midpoint(p1, p2).print();  // 期望: (1.5, 2)
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 友元类 — 一个类完全信任另一个类
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 声明 class B 为 class A 的友元 → B 的所有成员函数可访问 A 的私有成员
// 为什么: 两个类紧密耦合时 (如 LinkedList 和 Node)，避免暴露实现细节
// 何时用: 链表节点/迭代器、设计模式中的工厂、测试类访问内部状态

void part2_friend_class() {
    print_subtitle("Part 2: 友元类");

    Account alice("Alice", 1000), bob("Bob", 500);

    std::println("转账前:");
    AuditLogger::log(alice);
    AuditLogger::log(bob);

    Bank::transfer(alice, bob, 300);

    std::println("转账后:");
    AuditLogger::log(alice);
    AuditLogger::log(bob);

    // Account::adjust_balance() 是私有的
    // alice.adjust_balance(100);  // ❌ 不是友元则编译错误
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: 嵌套类 — 类中定义的类
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 一个类定义在另一个类的内部 (类似 namespace 的类版本)
// 为什么: ① 表达"从属"关系 (Node 属于 Tree)
//         ② 隐藏实现细节 (iterator 作为嵌套类)
//         ③ 嵌套类可以访问外部类的私有成员 (如果外部类声明它为友元)
// 何时用: 迭代器、节点、内部实现辅助类、Pimpl 惯用法

void part3_nested_class() {
    print_subtitle("Part 3: 嵌套类");

    Tree tree(10);
    tree.insert(5);
    tree.insert(15);

    std::println("根节点: {}", tree.root()->value);  // 期望: 10
    if (tree.root()->left)
        std::println("左子节点: {}", tree.root()->left->value);  // 期望: 5
    if (tree.root()->right)
        std::println("右子节点: {}", tree.root()->right->value); // 期望: 15

    // 使用嵌套迭代器
    auto it = tree.begin();
    std::println("迭代器指向: {}", *it);

    // 外部引用嵌套类需要全限定名
    Tree::Node node(42);  // ✅ OK
    std::println("外部创建 Node: {}", node.value);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: 前向声明与局部类
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 前向声明 → 只声明类名不定义; 局部类 → 函数内部定义的类
// 为什么: 前向声明 → 打破循环依赖, 减少编译时间
//         局部类 → 作用域限定, 临时封装 (C++中较少使用)
// 何时用: 前向声明常用; 局部类极少用 (lambda 已替代大部分场景)

void part4_forward_decl_and_local() {
    print_subtitle("Part 4: 前向声明与局部类");

    // ── 前向声明演示 ──
    Engine v8(400);
    Car sports_car;
    sports_car.set_engine(&v8);
    std::println("{}", sports_car.describe());

    // ── 局部类演示 ──
    // 在函数内部定义的类 (C++中一般用 lambda 替代)
    struct LocalHelper {
        static int add(int a, int b) { return a + b; }
    };
    std::println("局部类计算结果: {}", LocalHelper::add(3, 4));
}

// ═══════════════════════════════════════════════════════════════════════════════
// ⚠️ 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 友元关系不可传递 (A 是 B 的友元, B 是 C 的友元 → A 不是 C 的友元)
// 2. 友元关系不可继承 (基类的友元不是派生类的友元)
// 3. 滥用友元破坏封装 — 友元是"必要的恶", 只在确实需要时使用
// 4. 嵌套类默认不是外部类的友元 — 需要显式声明 (但 C++11 起嵌套类可访问外层私有)
// 5. 局部类不能有静态成员, 不能定义模板成员, 不能访问外围函数的局部变量

// ═══════════════════════════════════════════════════════════════════════════════
// ✏️ 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 为 Vec2 类添加 friend operator<< 实现格式化输出
// 2. 实现 LinkedList 类，内部用嵌套 Node 类
// 3. 实现 Wallet 类和 TransactionLogger 友元类 (记录所有交易)
// 4. 用前向声明解决两个互相引用的类 (Parent ↔ Child)

int main() {
    print_header("08 友元与嵌套类");
    part1_friend_function();
    part2_friend_class();
    part3_nested_class();
    part4_forward_decl_and_local();
    print_separator();
    std::println("🎉 08_friend_and_nested 完成！");
    return 0;
}

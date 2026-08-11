// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — 类与面向对象编程                                      ║
// ║  课时: 01_class_declaration — 类的声明与基础                              ║
// ║  学习目标:                                                                 ║
// ║    - 掌握 class 与 struct 的定义和区别                                     ║
// ║    - 理解 public / private / protected 访问控制                            ║
// ║    - 理解 this 指针的作用和使用场景                                        ║
// ║    - 掌握 static 成员变量和成员函数                                        ║
// ║    - 掌握 const 成员函数及其意义                                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "classes/class_basics.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <vector>

using namespace lesson;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: class vs struct — 定义类的两种方式
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: class 和 struct 都能定义类，语法几乎相同
// 为什么: struct 默认 public 继承和 public 成员，适合 POD/纯数据结构
//         class  默认 private 继承和 private 成员，适合封装型对象
// 何时用: 纯数据聚合用 struct；有行为和数据隐藏用 class

void part1_struct_vs_class() {
    print_subtitle("Part 1: class vs struct");

    Point p{3.0, 4.0};       // struct 成员可直接访问
    std::println("点坐标: ({}, {})", p.x, p.y);

    Circle c;
    c.set_radius(5.0);
    std::println("圆面积: {:.2f}", c.area());
    // c.radius_;  // ❌ 编译错误: radius_ 是 private
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 访问控制 — public / private / protected
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 三种访问修饰符控制类成员的可见性
// 为什么: 封装是实现"最小暴露原则"的基础，隐藏内部细节
// 何时用: public: 对外接口; private: 内部实现; protected: 给子类访问

void part2_access_control() {
    print_subtitle("Part 2: public / private / protected");

    BankAccount acc("艾克斯", 1000.0);
    acc.deposit(500.0);
    std::println("余额: {:.0f}", acc.balance());
    // acc.balance_  // ❌ private 成员无法从外部访问
    // acc.log_transaction(...)  // ❌ private 方法无法从外部调用
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: this 指针 — 指向当前对象的指针
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: this 是成员函数内部隐含的指针，指向调用该函数的对象
// 为什么: 区分成员和参数、返回自身引用、链式调用
// 何时用: 参数名与成员同名时、链式调用、需要返回 *this

void part3_this_pointer() {
    print_subtitle("Part 3: this 指针");

    Counter counter;
    counter.reset().increment().increment().add(5);  // 链式调用
    std::println("计数值: {}", counter.value());      // 期望: 7

    counter.set_count(42);
    std::println("设置后: {}", counter.value());      // 期望: 42
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: static 成员 — 属于类而非对象的成员
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: static 成员变量在所有对象间共享，static 方法无需对象即可调用
// 为什么: 计数、缓存、单例、不需要访问实例状态的辅助函数
// 何时用: 需要类级别的共享状态或工具函数时

void part4_static_members() {
    print_subtitle("Part 4: static 成员");

    std::println("初始学生数: {}", Student::total()); // 不创建对象即可调用
    {
        Student s1("张三");
        Student s2("李四");
        std::println("两人在校: {}", Student::total()); // 期望: 2
    }
    std::println("出作用域后: {}", Student::total());   // 期望: 0
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: const 成员函数 — 承诺不修改对象状态
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 函数签名后加 const，承诺不修改任何非 mutable 成员
// 为什么: 允许在 const 对象上调用；编译器强制检查，防止意外修改
// 何时用: 所有不修改对象状态的访问器(getter)都应标记为 const

void part5_const_member() {
    print_subtitle("Part 5: const 成员函数");

    Temperature t(25.0);
    std::println("摄氏: {:.1f}, 华氏: {:.1f}", t.get_celsius(), t.get_fahrenheit());

    const Temperature ct(0.0);
    std::println("冰点华氏: {:.1f}", ct.get_fahrenheit()); // 可在 const 对象上调用
    // ct.set_celsius(10.0);  // ❌ const 对象只能调用 const 成员函数

    for (int i = 0; i < 3; ++i) t.cached_fahrenheit();
    std::println("缓存访问次数: {}", t.read_count);        // 期望: 3
}

// ═══════════════════════════════════════════════════════════════════════════════
// ⚠️ 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 忘记在 getter 后加 const → const 对象无法调用
// 2. 误以为 struct 只能有数据成员 — struct 同样可以有成员函数
// 3. static 成员变量需要在类外定义 (C++17 前) 或加 inline (C++17+)
// 4. 在 const 成员函数中修改非 mutable 成员会导致编译错误
// 5. this 是指针而非引用，使用时需 -> 而非 .

// ═══════════════════════════════════════════════════════════════════════════════
// ✏️ 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 定义一个 Rectangle 类，包含 width/height，提供 area() 和 perimeter() 方法
// 2. 为 Rectangle 添加 static 成员统计已创建的矩形数量
// 3. 实现一个 Person 类，用 this 实现链式调用的 setter (set_name, set_age)
// 4. 将 BankAccount 的 getter 全部改为 const 成员函数

int main() {
    print_header("01 类的声明与基础");
    part1_struct_vs_class();
    part2_access_control();
    part3_this_pointer();
    part4_static_members();
    part5_const_member();
    print_separator();
    std::println("🎉 01_class_declaration 完成！");
    return 0;
}

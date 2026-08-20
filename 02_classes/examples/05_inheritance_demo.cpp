// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — 类与面向对象编程                                      ║
// ║  课时: 05_inheritance — 继承                                              ║
// ║  学习目标:                                                                 ║
// ║    - 掌握类继承语法和三种继承方式 (public/private/protected)               ║
// ║    - 理解 using 声明在继承中的作用                                        ║
// ║    - 理解多重继承的利弊和解决方案                                         ║
// ║    - 掌握虚基类解决菱形继承问题                                           ║
// ║    - 理解 final 类的作用                                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "classes/inheritance.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <memory>

using namespace lesson;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: 继承基础 — IS-A 关系
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 派生类继承基类的成员，表达"是一个"的关系
// 为什么: 代码复用 + 多态; 将共有逻辑抽取到基类，派生类专注差异
// 何时用: 当 B "是一个" A 时 (Dog is an Animal)，而非 B "有一个" A 时

void part1_inheritance_basics() {
    print_subtitle("Part 1: 继承基础");

    Dog dog("旺财");
    dog.eat();      // 继承自 Animal
    dog.sleep();    // 继承自 Animal
    dog.bark();     // Dog 特有
    std::println("狗的名字: {}", dog.name());
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 三种继承方式 — public / protected / private
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 继承访问修饰符决定基类成员在派生类中的可见性
// 为什么: 控制接口的传递范围，实现不同的设计意图
// 何时用: public: IS-A 关系 (最常用, 99%); protected/private: HAS-A 或实现继承

void part2_inheritance_access() {
    print_subtitle("Part 2: public / protected / private 继承");

    PublicDerived pd;
    pd.pub();        // ✅ public 继承, pub 仍是 public
    // pd.prot();    // ❌ protected 继承, 外部不可访问

    // ProtectedDerived prd;
    // prd.pub();    // ❌ protected 继承, pub 变为 protected

    std::println("public 继承最常用 (IS-A)");
    std::println("private 继承表示 '用...实现' (HAS-A 的替代)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: using 声明 — 调整访问级别和解决隐藏
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: using 将基类的名字引入派生类作用域
// 为什么: ① 改变继承成员的访问级别 (如将 protected 提升为 public)
//         ② 解决派生类同名函数隐藏基类重载的问题
// 何时用: 需要调整访问控制或暴露基类重载时

void part3_using_declaration() {
    print_subtitle("Part 3: using 声明");

    BetterPrinter bp;
    bp.print(42);     // ✅ 提升为 public
    bp.print(3.14);   // ✅ 提升为 public

    FileLogger fl;
    fl.log("文件打开成功");  // using 使得基类重载可见
    fl.log(404);              // 派生类自己的重载
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: 多重继承与虚基类 — 解决菱形继承
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 多重继承 → 同时继承多个基类; 虚基类 → 菱形继承中共享同一份基类
// 为什么: 多重继承表达"同时是"关系; 虚基类避免菱形继承中的二义性和冗余
// 何时用: 多重继承谨慎使用; 虚基类仅在菱形继承中必须时使用
//
/*
 * ── 菱形继承问题 ──
 *         Person
 *        /      \
 *   Student   Teacher
 *        \      /
 *      TeachingAssistant
 */

void part4_multiple_inheritance() {
    print_subtitle("Part 4: 多重继承与虚基类");

    TeachingAssistant ta("艾克斯", "计算机科学", "C++编程");
    std::println("名字: {}", ta.name());  // ✅ 只有一份 name_，无歧义
    ta.study();
    ta.teach();
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: final 类 — 禁止继续派生
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: final 关键字标记类或虚函数，阻止进一步继承/覆盖
// 为什么: 安全性：防止关键类被不当继承; 性能：允许编译器去虚拟化
// 何时用: 确定类不应被继承时; 虚函数行为已固定时

void part5_final() {
    print_subtitle("Part 5: final 类");

    FinalExample fe;
    fe.do_something();
    std::println("final 类确保不会被继承 — 安全且允许编译器优化");
}

// ═══════════════════════════════════════════════════════════════════════════════
// ⚠️ 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 非虚基类的菱形继承 → 数据冗余和歧义
// 2. 基类析构函数忘记 virtual → 通过基类指针 delete 时资源泄漏
// 3. private/protected 继承后忘记 using 暴露成员
// 4. 派生类同名函数隐藏基类所有重载 (忘记 using 声明)
// 5. 多重继承中的名字冲突和二义性

// ═══════════════════════════════════════════════════════════════════════════════
// ✏️ 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 实现 Vehicle → Car / Motorcycle 继承层次，提取公共逻辑到基类
// 2. 用 private 继承实现一个 Stack 类 (基于 vector)，用 using 暴露 push/pop/top
// 3. 实现一个菱形继承场景: Device → Printer/Scanner → AllInOne (虚基类)
// 4. 尝试用 final 阻止某个类被继承，观察编译错误

int main() {
    print_header("05 继承");
    part1_inheritance_basics();
    part2_inheritance_access();
    part3_using_declaration();
    part4_multiple_inheritance();
    part5_final();
    print_separator();
    std::println("🎉 05_inheritance 完成！");
    return 0;
}

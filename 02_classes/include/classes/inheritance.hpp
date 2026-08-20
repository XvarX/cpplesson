// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — 继承 (inheritance)                                      ║
// ║  内容: 继承基础/三种继承方式/using 声明/多重继承/虚基类/final               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#pragma once

#include <print>
#include <string>
#include <string_view>
#include <utility>
#include <memory>

// ═══════════════════════════════════════════════════════════════════════════════
// Animal / Dog — public 继承 (IS-A 关系)
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 派生类继承基类的成员，表达"是一个"的关系
// 为什么: 代码复用 + 多态; 将共有逻辑抽取到基类，派生类专注差异
// 何时用: 当 B "是一个" A 时 (Dog is an Animal)，而非 B "有一个" A 时

class Animal {
public:
    Animal(std::string name) : name_(std::move(name)) {}
    virtual ~Animal() = default;  // 基类析构声明 virtual

    void eat()    const { std::println("{} 在吃东西", name_); }
    void sleep()  const { std::println("{} 在睡觉", name_); }

    std::string_view name() const { return name_; }

protected:  // 派生类可访问，外部不可
    std::string name_;
};

// public 继承: 基类的 public → 派生类 public; protected → protected
class Dog : public Animal {
public:
    Dog(std::string name) : Animal(std::move(name)) {}

    void bark() const { std::println("{} 汪汪叫!", name_); } // 可访问 protected name_
};

// ═══════════════════════════════════════════════════════════════════════════════
// Base / PublicDerived / ProtectedDerived / PrivateDerived — 三种继承方式
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 继承访问修饰符决定基类成员在派生类中的可见性
// 为什么: 控制接口的传递范围，实现不同的设计意图
// 何时用: public: IS-A 关系 (最常用, 99%); protected/private: HAS-A 或实现继承

class Base {
public:
    void pub() {}
protected:
    void prot() {}
private:
    void priv() {}  // 无论哪种继承，派生类都不可直接访问
};

class PublicDerived  : public Base {
    // pub()    → public     (外部可见)
    // prot()   → protected  (子类可见)
    // priv()   → 不可访问
};

class ProtectedDerived : protected Base {
    // pub()    → protected  (外部不可见!)
    // prot()   → protected
    // priv()   → 不可访问
};

class PrivateDerived : private Base {
    // pub()    → private    (外部不可见, 子类也不可见!)
    // prot()   → private
    // priv()   → 不可访问
};

// ═══════════════════════════════════════════════════════════════════════════════
// Printer / BetterPrinter — using 声明调整访问级别
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: using 将基类的名字引入派生类作用域
// 为什么: ① 改变继承成员的访问级别 (如将 protected 提升为 public)
//         ② 解决派生类同名函数隐藏基类重载的问题
// 何时用: 需要调整访问控制或暴露基类重载时

class Printer {
protected:
    void print(int x)    { std::println("int: {}", x); }
    void print(double x) { std::println("double: {:.2f}", x); }
};

class BetterPrinter : private Printer {  // private 继承
public:
    using Printer::print;  // 将 print 提升为 public! (选择性暴露)
};

// ═══════════════════════════════════════════════════════════════════════════════
// Logger / FileLogger — using 解决隐藏
// ═══════════════════════════════════════════════════════════════════════════════

class Logger {
public:
    void log(const std::string& msg) { std::println("日志: {}", msg); }
};

class FileLogger : public Logger {
public:
    using Logger::log;  // 引入基类 log (避免被下面的 log(int) 隐藏)
    void log(int code) { std::println("错误码: {}", code); }
};

// ═══════════════════════════════════════════════════════════════════════════════
// Person / Student / Teacher / TeachingAssistant — 多重继承与虚基类
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 多重继承 → 同时继承多个基类; 虚基类 → 菱形继承中共享同一份基类
// 为什么: 多重继承表达"同时是"关系; 虚基类避免菱形继承中的二义性和冗余
// 何时用: 多重继承谨慎使用; 虚基类仅在菱形继承中必须时使用
//
/*
 *     ── 菱形继承问题 ──
 *            Person
 *           /      \
 *      Student   Teacher
 *           \      /
 *         TeachingAssistant
 */

class Person {
public:
    Person(std::string name) : name_(std::move(name)) {}
    virtual ~Person() = default;

    std::string_view name() const { return name_; }

protected:
    std::string name_;
};

// virtual 继承: Person 是虚基类，最终只会有一份
class Student : virtual public Person {
public:
    Student(std::string name, std::string major)
        : Person(std::move(name)), major_(std::move(major)) {}
    void study() const { std::println("{} 学习{}", name_, major_); }
protected:
    std::string major_;
};

class Teacher : virtual public Person {
public:
    Teacher(std::string name, std::string course)
        : Person(std::move(name)), course_(std::move(course)) {}
    void teach() const { std::println("{} 教{}", name_, course_); }
protected:
    std::string course_;
};

// 最派生类负责构造虚基类!
class TeachingAssistant : public Student, public Teacher {
public:
    TeachingAssistant(std::string name, std::string major, std::string course)
        : Person(std::move(name))       // 直接构造虚基类 Person
        , Student("", std::move(major)) // 间接基类
        , Teacher("", std::move(course)) {}
    // 没有 virtual，Person 会有两份 → name() 调用歧义!
};

// ═══════════════════════════════════════════════════════════════════════════════
// FinalExample — final 类禁止继续派生
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: final 关键字标记类或虚函数，阻止进一步继承/覆盖
// 为什么: 安全性：防止关键类被不当继承; 性能：允许编译器去虚拟化
// 何时用: 确定类不应被继承时; 虚函数行为已固定时

class FinalExample final {  // 此类不能被继承
public:
    void do_something() { std::println("FinalExample 工作"); }
};
// class Derived : public FinalExample {}; // ❌ 编译错误: 不能继承 final 类

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

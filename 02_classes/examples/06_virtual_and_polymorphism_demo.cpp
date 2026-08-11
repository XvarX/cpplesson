// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — 类与面向对象编程                                      ║
// ║  课时: 06_virtual_and_polymorphism — 虚函数与多态                         ║
// ║  学习目标:                                                                 ║
// ║    - 理解虚函数的原理 (vtable / vptr)                                     ║
// ║    - 掌握纯虚函数和抽象类的定义与使用                                      ║
// ║    - 掌握 override 和 final 关键字                                        ║
// ║    - 理解虚析构的必要性                                                   ║
// ║    - 掌握 dynamic_cast 进行安全的向下转型                                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "classes/polymorphism.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <vector>
#include <memory>

using namespace lesson;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: 虚函数基础 — 运行时多态的核心
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: virtual 标记的函数通过 vtable 在运行时决定调用哪个版本
// 为什么: 让"基类指针/引用调用派生类方法"成为可能 — 多态的核心
// 何时用: 希望派生类可以覆盖 (override) 的行为

void part1_virtual_basics() {
    print_subtitle("Part 1: 虚函数基础");

    // 非虚函数 → 静态绑定
    NonVirtualDerived nvd;
    NonVirtualBase& ref = nvd;
    ref.speak();  // 输出 "基类说话" — 不是我们想要的!

    // 虚函数 → 动态绑定
    Circle c(5.0);
    Rectangle r(4.0, 6.0);
    print_shape_info(c);  // 圆形 的面积: 78.54
    print_shape_info(r);  // 矩形 的面积: 24.00
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 纯虚函数与抽象类 — 定义接口
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 纯虚函数 ( = 0) 使类成为抽象类，不能实例化
// 为什么: 强制派生类实现特定接口 (类似于 Java 的 interface)
// 何时用: 定义接口规范，派生类必须实现

void part2_abstract_class() {
    print_subtitle("Part 2: 纯虚函数与抽象类");

    // AbstractAnimal a;  // ❌ 编译错误: 不能实例化抽象类

    Cat cat;
    Duck duck;
    cat.describe();   // 猫 的叫声是 喵喵
    duck.describe();  // 鸭子 的叫声是 嘎嘎

    // 多态地使用抽象基类
    std::vector<std::unique_ptr<AbstractAnimal>> zoo;
    zoo.push_back(std::make_unique<Cat>());
    zoo.push_back(std::make_unique<Duck>());
    for (const auto& animal : zoo) {
        animal->describe();
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: override 和 final — 安全的覆盖
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: override 标记覆盖基类虚函数; final 阻止进一步覆盖
// 为什么: override → 编译期检查，防止签名写错; final → 锁定行为
// 何时用: 每个覆盖基类虚函数的函数都应加 override

void part3_override_final() {
    print_subtitle("Part 3: override 和 final");

    SportsCar sc;
    sc.start();              // 跑车轰鸣启动!
    std::println("极速: {} km/h", sc.max_speed());

    std::println("override 捕获签名错误, final 锁定虚函数行为");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: 虚析构 — 基类析构必须 virtual
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 基类析构函数标记 virtual，确保 delete 基类指针时调用正确的析构
// 为什么: 非虚析构通过基类指针 delete → 只调用基类析构 → 资源泄漏!
// 何时用: 任何可能被继承并多态使用的类，析构函数必须 virtual

void part4_virtual_destructor() {
    print_subtitle("Part 4: 虚析构 (非常重要!)");

    std::println("--- 错误做法 (非虚析构) ---");
    BadBase* bad = new BadDerived();
    delete bad;  // ⚠️ 只输出 ~BadBase，~BadDerived 不会调用!

    std::println("\n--- 正确做法 (虚析构) ---");
    GoodBase* good = new GoodDerived();
    delete good;  // ✅ 依次输出 ~GoodDerived, ~GoodBase
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: dynamic_cast — 安全的向下转型
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: dynamic_cast<T*> 尝试将基类指针/引用转为派生类
// 为什么: 安全的运行时类型检查，失败返回 nullptr (指针) 或抛异常 (引用)
// 何时用: 需要确认具体派生类型时 (但优先用虚函数而非类型判断)

void part5_dynamic_cast() {
    print_subtitle("Part 5: dynamic_cast");

    std::unique_ptr<Media> media = std::make_unique<Audio>();

    // 尝试转换
    if (auto* audio = dynamic_cast<Audio*>(media.get())) {
        audio->play_audio();  // ✅ 转换成功
    }
    if (auto* video = dynamic_cast<Video*>(media.get())) {
        video->play_video();  // ← 不执行: 转换失败返回 nullptr
    }

    // 引用版本: 失败时抛出 std::bad_cast
    Audio& audio_ref = dynamic_cast<Audio&>(*media); // ✅ 安全
    audio_ref.play_audio();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ⚠️ 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 基类析构不是 virtual → delete 基类指针时派生类析构不调用 (严重泄漏!)
// 2. 忘记 override → 签名写错变成"隐藏"而非"覆盖"，编译不报错
// 3. 构造函数中调用虚函数 → 此时 vtable 未初始化，调用的是基类版本
// 4. 虚函数带默认参数 → 默认参数静态绑定，与实际调用的函数版本不匹配
// 5. 过度使用 dynamic_cast → 通常是设计问题，优先考虑虚函数

// ═══════════════════════════════════════════════════════════════════════════════
// ✏️ 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 实现 Employee 抽象类 → Manager/Engineer 派生类 (纯虚 calculate_salary)
// 2. 验证: 基类不加 virtual 析构 → 派生类 delete 时是否泄漏
// 3. 写一个故意把 override 函数签名写错的例子，观察 override 如何报错
// 4. 实现一个 Plugin 接口 (纯虚 init/execute/shutdown) 并用多态调用

int main() {
    print_header("06 虚函数与多态");
    part1_virtual_basics();
    part2_abstract_class();
    part3_override_final();
    part4_virtual_destructor();
    part5_dynamic_cast();
    print_separator();
    std::println("🎉 06_virtual_and_polymorphism 完成！");
    return 0;
}

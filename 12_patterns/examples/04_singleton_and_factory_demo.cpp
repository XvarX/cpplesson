// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  04_singleton_and_factory_demo.cpp — 单例模式与工厂模式演示                  ║
// ║  确保唯一实例 + 灵活创建对象                                               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
// 单例模式:
//   - 确保一个类只有一个实例,提供全局访问点
//   - Meyers 单例: C++11 起静态局部变量初始化是线程安全的
//   - 避免了 "静态初始化顺序问题" (Static Init Order Fiasco)
// 工厂模式:
//   - 将对象创建逻辑集中管理,解耦创建和使用
//   - 简单工厂: 函数根据参数创建不同子类
//   - 注册工厂: 运行时动态注册创建函数,支持插件扩展
// 注意:
//   - 单例本质是全局状态,过度使用会增加耦合
//   - 依赖注入(DI) 是更灵活的替代方案

#include "patterns/singleton_and_factory.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <stdexcept>

auto main() -> int {
    lesson::print_header("04  单例模式与工厂模式");

    // ── 单例演示 ──────────────────────────────────────────────────────────
    lesson::print_subtitle("1. Meyers 单例 — Logger");
    patterns::Logger::instance().log("启动");
    patterns::Logger::instance().log("加载配置");
    patterns::Logger::instance().log("就绪");
    std::println("  共 {} 条日志\n", patterns::Logger::instance().message_count());

    // ── 简单工厂 ──────────────────────────────────────────────────────────
    lesson::print_subtitle("2. 简单工厂函数");
    try {
        auto dog = patterns::create_animal("dog");
        auto cat = patterns::create_animal("cat");
        dog->speak();
        cat->speak();
        patterns::create_animal("elephant");  // 异常: 未知类型
    } catch (const std::exception& e) {
        std::println("  ❌ {}", e.what());
    }

    // ── 注册式工厂 ────────────────────────────────────────────────────────
    lesson::print_subtitle("3. 注册式工厂 — 运行时动态扩展");
    auto& factory = patterns::AnimalFactory::instance();

    // 运行时注册新类型(模拟插件加载)
    factory.register_type("robot_dog", []() -> std::unique_ptr<patterns::Animal> {
        struct RobotDog : patterns::Animal {
            void speak() const override { std::println("  🤖 哔哔! (RobotDog)"); }
        };
        return std::make_unique<RobotDog>();
    });

    factory.create("robot_dog")->speak();
    factory.create("cat")->speak();

    // ── 依赖注入简介 ──────────────────────────────────────────────────────
    lesson::print_subtitle("4. 替代方案: 依赖注入(DI)");
    std::println("  单例 = 全局状态 → 测试难,耦合高");
    std::println("  DI: 通过构造/参数显式传入依赖");
    std::println("  void do_work(Logger& log) {{ log.log(\"work\"); }}");
    std::println("  // 测试时传 mock,生产环境传真实 Logger");

    // ── 常见陷阱 ──────────────────────────────────────────────────────────
    lesson::print_subtitle("常见陷阱");
    std::println("⚠️  静态初始化顺序问题: 全局对象 A 构造时访问全局对象 B");
    std::println("   但 B 可能尚未初始化 → UB。解决: Meyers 单例(首次访问时初始化)");
    std::println("⚠️  单例析构顺序: 多个单例析构时相互访问 → UB");
    std::println("⚠️  测试噩梦: 单例共享状态 → 测试间相互影响");
    std::println("⚠️  工厂忘记注册类型 → 运行时异常");

    lesson::print_subtitle("练习");
    std::println("1. 为 Logger 添加日志级别(DEBUG/INFO/WARN/ERROR)过滤功能");
    std::println("2. 实现一个线程安全的单例(提示: Meyers 单例已经线程安全)");
    std::println("3. 将 ConfigManager(配置管理器)改造为单例");
    std::println("4. 将示例中的单例用法改写为依赖注入方式,体会差异");
    std::println("5. 实现模板工厂: template<typename Base> class GenericFactory");

    return 0;
}

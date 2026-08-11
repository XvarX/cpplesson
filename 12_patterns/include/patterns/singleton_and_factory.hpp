// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  singleton_and_factory.hpp — 单例模式与工厂模式                             ║
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

#pragma once

#include <print>
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <stdexcept>

namespace patterns {

// ═══════════════════════════════════════════════════════════════════════════════
// 第一部分: 单例模式
// ═══════════════════════════════════════════════════════════════════════════════

// ── Meyers 单例 (C++11+ 推荐) ────────────────────────────────────────────────
// 函数内的静态局部变量在第一次调用时初始化,C++11 保证线程安全
class Logger {
public:
    static Logger& instance() {
        // 多线程安全: C++11 保证这段代码只执行一次
        static Logger the_instance;
        return the_instance;
    }

    void log(std::string_view msg) {
        ++msg_count_;
        std::println("[LOG #{}] {}", msg_count_, msg);
    }

    int message_count() const { return msg_count_; }

    // 禁止拷贝和移动
    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&)                 = delete;
    Logger& operator=(Logger&&)      = delete;

private:
    Logger() { std::println("[Logger] 初始化"); }
    ~Logger() = default;
    int msg_count_ = 0;
};

// ── 全局变量的问题(对比) ─────────────────────────────────────────────────────
// ❌ 错误做法: int g_counter = 0;  // 不同 .cpp 中全局对象的初始化顺序不确定!
// ✅ 正确做法: 用 Meyers 单例替代全局变量

// ═══════════════════════════════════════════════════════════════════════════════
// 第二部分: 工厂模式
// ═══════════════════════════════════════════════════════════════════════════════

// ── 产品类层次 ───────────────────────────────────────────────────────────────
struct Animal {
    virtual ~Animal() = default;
    virtual void speak() const = 0;
};

struct Dog : Animal {
    void speak() const override { std::println("  🐕 汪汪! (Dog)"); }
};

struct Cat : Animal {
    void speak() const override { std::println("  🐈 喵喵! (Cat)"); }
};

// ── 工厂1: 简单工厂函数 ──────────────────────────────────────────────────────
inline std::unique_ptr<Animal> create_animal(std::string_view type) {
    if (type == "dog") return std::make_unique<Dog>();
    if (type == "cat") return std::make_unique<Cat>();
    throw std::runtime_error(std::string("未知动物: ") + std::string(type));
}

// ── 工厂2: 注册式工厂 — 运行时动态扩展 ──────────────────────────────────────
class AnimalFactory {
public:
    using Creator = std::function<std::unique_ptr<Animal>()>;

    static AnimalFactory& instance() {
        static AnimalFactory factory;
        return factory;
    }

    // 注册新的动物类型(可以在运行时、从插件中动态注册!)
    void register_type(std::string_view name, Creator creator);

    std::unique_ptr<Animal> create(std::string_view name) const;

private:
    AnimalFactory();  // 构造函数在 .cpp 中实现,注册默认类型
    std::map<std::string, Creator> creators_;
};

} // namespace patterns

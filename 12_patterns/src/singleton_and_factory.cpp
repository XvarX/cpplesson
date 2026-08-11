// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  singleton_and_factory.cpp — AnimalFactory 的实现                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "patterns/singleton_and_factory.hpp"

namespace patterns {

// ── AnimalFactory 构造函数: 注册默认类型 ──────────────────────────────────────
AnimalFactory::AnimalFactory() {
    register_type("dog", [] { return std::make_unique<Dog>(); });
    register_type("cat", [] { return std::make_unique<Cat>(); });
}

// ── 注册新的动物类型 ──────────────────────────────────────────────────────────
void AnimalFactory::register_type(std::string_view name, Creator creator) {
    creators_[std::string(name)] = std::move(creator);
}

// ── 根据名称创建动物 ──────────────────────────────────────────────────────────
std::unique_ptr<Animal> AnimalFactory::create(std::string_view name) const {
    auto it = creators_.find(std::string(name));
    if (it != creators_.end()) return it->second();
    throw std::runtime_error("未注册的动物: " + std::string(name));
}

} // namespace patterns

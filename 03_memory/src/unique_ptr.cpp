// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: src/unique_ptr.cpp — unique_ptr 辅助函数实现                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "memory/unique_ptr.hpp"

// ── 工厂函数: 返回 unique_ptr，简洁地转移所有权 ──────────────────────────────────
// make_unique 在函数内部创建，return 时自动移动所有权给调用者
std::unique_ptr<Resource> create_resource(const std::string& name) {
    return std::make_unique<Resource>(name);
}

// ── 接受 unique_ptr 作为参数的函数 (按值传递 = 接收所有权) ────────────────────────
void consume_resource(std::unique_ptr<Resource> res) {
    std::println("  消费资源...");
    res->use();
    // res 离开作用域 → 资源自动释放
}

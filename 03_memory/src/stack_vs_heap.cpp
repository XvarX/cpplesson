// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: src/stack_vs_heap.cpp — 全局 operator new/delete 实现          ║
// ║  定义全局 operator new/delete，追踪每次堆分配                                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "memory/stack_vs_heap.hpp"

// ── 自定义 operator new/delete，观察内存分配行为 ─────────────────────────────────
// 重载全局 operator new 来追踪每一次堆分配
void* operator new(std::size_t size) {
    void* p = std::malloc(size);
    std::println("  [new] 分配 {} 字节 → 地址 {}", size, p);
    return p;
}

void operator delete(void* p) noexcept {
    std::println("  [delete] 释放地址 {}", p);
    std::free(p);
}

void operator delete(void* p, std::size_t /*size*/) noexcept {
    std::println("  [delete] 释放地址 {} (sized)", p);
    std::free(p);
}

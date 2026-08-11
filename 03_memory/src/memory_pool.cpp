// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: src/memory_pool.cpp — MemoryPool 非模板实现辅助函数             ║
// ║  MemoryPool 模板本身是 header-only，此文件提供:                               ║
// ║   1. Widget 静态成员的 ODR 锚点 (C++17 inline static 已内联，保留供参考)      ║
// ║   2. 池内存管理辅助工具函数 (非模板)                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "memory/memory_pool.hpp"
#include <print>
#include <cstddef>
#include <algorithm>    // std::fill

// ── 池对齐检查工具 ──────────────────────────────────────────────────────────────
// 验证指定类型与内存池的对齐兼容性
// 如果对齐要求超过池的默认对齐，placement new 时可能触发未定义行为

void check_pool_alignment(std::size_t type_align, std::size_t type_size) {
    std::println("--- 池对齐检查 ---");
    std::println("  类型对齐要求: {} 字节", type_align);
    std::println("  类型大小: {} 字节", type_size);
    if (type_align > alignof(std::max_align_t)) {
        std::println("  ⚠️  对齐要求超过 max_align_t ({}) — 需要特殊处理",
                     alignof(std::max_align_t));
    } else {
        std::println("  ✓ 对齐在标准范围内，无需特殊处理");
    }
}

// ── 计算所需池大小 ──────────────────────────────────────────────────────────────
// 给定每个块的大小和块数，计算需要分配的池存储大小 (考虑对齐)
// 公式: block_count * sizeof(T)，但需要保证对齐到 alignof(T)

std::size_t compute_pool_storage_size(std::size_t block_size,
                                      std::size_t block_count,
                                      std::size_t alignment) {
    // 每个块至少需要 sizeof(FreelistNode*) 大小 (freelist 的侵入式链表要求)
    if (block_size < sizeof(void*)) {
        std::println("  [警告] 块大小 {} < sizeof(void*) {}，无法使用 freelist",
                     block_size, sizeof(void*));
    }
    // 简单的存储大小: block_count * block_size，对齐由 alignas 保证
    std::size_t raw = block_count * block_size;
    // 确保对齐边界
    std::size_t aligned = ((raw + alignment - 1) / alignment) * alignment;
    return aligned;
}

// ── 演示: 对比 new/delete vs 内存池的性能 ─────────────────────────────────────────
// 这是一个概念演示，实际 benchmark 应使用 Google Benchmark 或类似工具
// 这里展示思路: 内存池避免了每次 new 的系统调用和堆碎片

void demonstrate_pool_vs_new_idea() {
    std::println("── 内存池 vs new/delete 性能对比思路 ──");
    std::println("  new/delete 每次分配:");
    std::println("    1. 系统调用 (mmap/brk/VirtualAlloc)");
    std::println("    2. 堆管理器查找空闲块 (best-fit/first-fit)");
    std::println("    3. 可能的锁竞争 (多线程)");
    std::println("    4. 堆碎片随时间增加");
    std::println("");
    std::println("  MemoryPool 每次分配:");
    std::println("    1. 从 freelist 头部取一个指针 (O(1))");
    std::println("    2. 无锁 (单线程场景)");
    std::println("    3. 无碎片 (连续存储)");
    std::println("    4. 缓存友好 (所有对象在连续内存中)");
    std::println("");
    std::println("  典型加速比: 5x ~ 100x (取决于对象大小和分配模式)");
}

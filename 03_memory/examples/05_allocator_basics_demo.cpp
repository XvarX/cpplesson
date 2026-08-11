// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: 05_allocator_basics_demo — 分配器入门                           ║
// ║  目标: 理解 std::allocator、自定义分配器、pmr (C++17)、memory_resource        ║
// ║  前置: 理解 new/delete、placement new、容器 (vector/map) 的基本用法            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <iostream>
#include <print>
#include <memory>           // std::allocator, allocator_traits
#include <memory_resource>  // std::pmr::polymorphic_allocator, monotonic_buffer_resource (C++17)
#include <vector>
#include <list>
#include <string>
#include <cstddef>
#include <cassert>
#include "shared/lesson_utils.hpp"
#include "memory/allocator_basics.hpp"

int main() {
    lesson::print_header("内存管理 (5): 分配器基础");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: std::allocator — 标准分配器的角色
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: std::allocator<T> 是标准容器的默认内存分配器，封装了 new/delete
    // 为什么: 将内存分配与对象构造分离 (allocate 只分内存, construct 在内存上构造)
    //         容器通过 allocator 接口操作内存，可以替换实现 (如内存池)
    // 何时接触: 98% 的时间你不需要碰它 — 默认即可; 2% 的场景需要自定义
    {
        lesson::print_subtitle("Part 1: std::allocator 基础");

        std::allocator<Point> alloc;

        // allocate 只分配内存，不调用构造函数
        Point* p = alloc.allocate(3);  // 分配 3 个 Point 的裸内存
        std::println("分配了 {} 个 Point 的内存 @ {}", 3, static_cast<void*>(p));

        // 在已分配的内存上逐个构造对象 (用 allocator_traits 或 construct_at)
        for (int i = 0; i < 3; ++i) {
            std::construct_at(&p[i], i * 10, i * 10 + 5);  // C++20
        }
        std::println("构造完成: p[0]=({},{}), p[1]=({},{}), p[2]=({},{})",
                     p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y);

        // 逆序析构
        for (int i = 2; i >= 0; --i) {
            std::destroy_at(&p[i]);
        }

        // 释放内存 (不调用析构 — 已经手动析构了)
        alloc.deallocate(p, 3);
        std::println("(已析构并释放内存)\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: 为什么需要分配器 — 容器的灵活内存管理
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: 分配器让容器不直接调用 new/delete，而是通过分配器接口
    // 为什么: (1) 自定义内存来源 (共享内存/显存/栈) (2) 内存池复用 (3) 统计/调试
    // 何时用: 标准分配器可应对 98% 场景; 需要精确控制时用自定义分配器
    {
        lesson::print_subtitle("Part 2: 分配器在容器中的角色");

        std::println("std::vector<T> = std::vector<T, std::allocator<T>>");
        std::println("所以 vector<int> 实际上是 vector<int, allocator<int>>");
        std::println("");
        std::println("分配器接口 (Allocator 必须提供):");
        std::println("  - allocate(n)   → 分配 n 个 T 的内存");
        std::println("  - deallocate(p, n) → 释放之前分配的内存");
        std::println("  - 两个分配器是否等价 (== 运算符)");
        std::println("  - rebind 内嵌类型 (让 allocator<int> 分配 string)");
        std::println("");
        std::println("C++11 后大部分工作由 allocator_traits 自动处理, 不需要手动实现 construct/destroy\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: 使用自定义分配器
    // ═══════════════════════════════════════════════════════════════════════════
    {
        lesson::print_subtitle("Part 3: 自定义统计分配器演示");

        // 用 CountingAllocator 创建一个 vector
        using CountingVector = std::vector<int, CountingAllocator<int>>;

        CountingVector v;
        v.reserve(100);           // 预分配 100 个 int
        for (int i = 0; i < 50; ++i) {
            v.push_back(i);
        }
        // 可能触发 realloc: 容量增长时, 旧内存释放 + 新内存分配

        std::println("vector 操作完成:");
        CountingAllocator<int>::print_stats();
        std::println();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: pmr (Polymorphic Memory Resource) — C++17 革命
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: std::pmr 命名空间提供了基于多态的内存分配方案
    //         通过 memory_resource 抽象基类，运行时切换分配策略
    // 为什么: 传统分配器是"模板参数"，不同类型产生不同容器的类型
    //         pmr 是"值语义"的，同类型容器可以有不同的分配策略
    // 何时用: 需要为不同场景选择不同分配策略 (栈、池、共享内存) 但不想要不同容器类型
    {
        lesson::print_subtitle("Part 4: pmr — C++17 多态分配器");

        // monotonic_buffer_resource: 只分配不释放的快速分配器
        // 适合: 一次性处理大量临时对象，最后统一释放
        {
            std::println("--- monotonic_buffer_resource (单调缓冲区) ---");
            // 在栈上预先分配 4096 字节的缓冲区
            std::byte buffer[4096];

            // 创建单调资源: 先用自己的 buffer，不够时从堆分配
            std::pmr::monotonic_buffer_resource pool{
                buffer, sizeof(buffer)
            };

            // pmr::vector — 使用 polymorphic_allocator 的 vector
            std::pmr::vector<int> numbers{&pool};

            // 往 pmr::vector 添加大量数据
            for (int i = 0; i < 1000; ++i) {
                numbers.push_back(i);
            }
            std::println("pmr::vector 包含 {} 个元素", numbers.size());
            std::println("注意: monotonic 不会在 pop_back 时释放内存");
            std::println("  所有内存在 pool 销毁时统一释放 (RAII)");

            // pool 离开作用域 → 所有在 pool 上分配的内存自动释放
        }

        // 对比: 传统分配器与 pmr 的类型差异
        std::println("\n传统分配器 vs pmr 分配器:");
        std::println("  传统: vector<int, MyAlloc<int>> 和 vector<int> 是不同类型!");
        std::println("  pmr:  vector<int> (分配器由资源指针决定, 类型相同)");
        std::println("  → pmr 更适合需要运行时切换分配策略的场景\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: 常用 pmr memory_resource
    // ═══════════════════════════════════════════════════════════════════════════
    {
        lesson::print_subtitle("Part 5: 常用 memory_resource 一览");

        std::println("C++17 标准库提供的 memory_resource:");
        std::println("  1. new_delete_resource()      — 使用 new/delete (默认)");
        std::println("  2. monotonic_buffer_resource  — 只增不减, 适合临时数据");
        std::println("  3. unsynchronized_pool_resource — 非线程安全的内存池");
        std::println("  4. synchronized_pool_resource   — 线程安全的内存池");
        std::println("");
        std::println("使用场景选择:");
        std::println("  - 解析 JSON/XML               → monotonic  (大量临时对象)");
        std::println("  - 游戏每帧的临时分配           → monotonic");
        std::println("  - 服务器多线程处理             → synchronized_pool_resource");
        std::println("  - 单线程批处理                 → unsynchronized_pool_resource\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_separator("常见陷阱");
    std::println("1. 自定义分配器忘记 rebind 内嵌类型 → 容器无法使用 allocator<int> 分配 string");
    std::println("2. monotonic_buffer_resource 的缓冲区太小 → 溢出后从堆分配 (慢!)");
    std::println("3. monotonic_buffer_resource 上频繁 pop_back 不会释放内存 → 内存只增不减");
    std::println("4. 分配器不等价 (operator== 返回 false) → 两个容器不能 swap/merge");
    std::println("5. 忘记 pmr 需要链接 std::pmr (如果使用独立的 pmr 库) → 链接错误");
    std::println("6. C++17 之前的分配器: construct/destroy 需要自己实现 → 现代 C++ 用 allocator_traits");

    // ═══════════════════════════════════════════════════════════════════════════
    // 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_separator("练习");
    std::println("1. 用 std::allocator 手动管理一个 int 数组 (allocate → construct_at → destroy_at → deallocate)");
    std::println("2. 写一个简单的 ArenaAllocator，在构造函数中分配大块内存，每次 allocate 从中切分");
    std::println("3. 用 std::pmr 重写一个原来使用 std::vector 的函数，对比性能和内存使用");
    std::println("4. 实现一个 logging_allocator，记录每次分配/释放的文件名和行号");
    std::println("5. (进阶) 阅读 EASTL 或 llvm::BumpPtrAllocator 的源码, 理解工业级分配器设计");
    std::println("6. (进阶) 使用 monotonic_buffer_resource 实现一个 JSON DOM 解析器");

    return 0;
}

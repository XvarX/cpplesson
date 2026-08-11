// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: 06_memory_pool_demo — 固定大小内存池                             ║
// ║  目标: 理解 freelist 内存池原理、placement new 实战、与 std::allocator 的关系  ║
// ║  前置: placement new、new/delete、std::allocator 基础                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <memory>       // std::construct_at, std::destroy_at
#include <cstddef>
#include <cassert>
#include "shared/lesson_utils.hpp"
#include "memory/memory_pool.hpp"

int main() {
    lesson::print_header("内存管理 (6): 固定大小 MemoryPool");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: MemoryPool 基础 — 分配/构造/析构/释放 四步分离
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: MemoryPool 预先分配一块连续内存，通过 freelist 管理空闲块
    // 为什么: 分离"内存管理"和"对象生命周期" — 内存归池管，构造归用户管
    // 何时用: 大量固定大小对象的频繁创建/销毁 (粒子、网络包、节点)
    {
        lesson::print_subtitle("Part 1: MemoryPool 基础 — 四步操作");

        constexpr std::size_t PoolSize = 4;
        MemoryPool<Widget, PoolSize> pool;

        std::println("池初始状态: 容量={}, 已用={}", pool.capacity(), pool.used_count());

        // 四步模式: allocate → construct → ... 使用 ... → destroy → deallocate
        // 第 1 步: 从池中拿一块裸内存 (不调用构造函数!)
        Widget* w1 = pool.allocate();
        assert(w1 != nullptr);

        // 第 2 步: placement new 在裸内存上构造对象
        pool.construct(w1, 3.14);

        // ... 使用对象 ...
        w1->show();

        // 第 3 步: 手动析构 (释放对象资源，但不释放内存)
        pool.destroy(w1);

        // 第 4 步: 归还内存块到池
        pool.deallocate(w1);

        std::println("归还后: 已用={}", pool.used_count());
        assert(pool.used_count() == 0);
        std::println();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: freelist 的内部机制 — 侵入式链表
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: 空闲块的头部存储指向下一个空闲块的指针 (union 复用内存)
    // 为什么: 避免额外内存开销 — 空闲块本身就是链表节点
    // 何时用: 所有基于 freelist 的内存池都使用这个技巧
    {
        lesson::print_subtitle("Part 2: freelist 侵入式链表机制");

        constexpr std::size_t N = 3;
        MemoryPool<Widget, N> pool;

        std::println("freelist 原理:");
        std::println("  空闲块: [next→] [next→] [next→nullptr]");
        std::println("  分配时: 取头节点, freelist 前进到 next");
        std::println("  归还时: 将节点插回列表头部 (O(1))");
        std::println("");

        // 演示分配多个块
        Widget* widgets[N];
        for (std::size_t i = 0; i < N; ++i) {
            widgets[i] = pool.allocate();
            pool.construct(widgets[i], static_cast<double>(i + 1) * 1.1);
        }
        std::println("分配 {} 个块后: 已用={}, 空闲={}", N, pool.used_count(), pool.free_count());

        // 尝试超量分配
        Widget* extra = pool.allocate();
        std::println("超量分配: {} (nullptr=池已满)", static_cast<void*>(extra));

        // 全部归还
        for (std::size_t i = 0; i < N; ++i) {
            pool.destroy(widgets[i]);
            pool.deallocate(widgets[i]);
        }
        std::println("全部归还后: 已用={}", pool.used_count());
        std::println();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: placement new 实战 — 手控构造与析构的对齐
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: placement new 在指定地址上调用构造函数; 对应地显式调用析构函数
    // 为什么: 内存池只管理原始内存，对象的创建/销毁完全由用户控制
    // 何时用: 当你需要精确控制对象在哪块内存上创建时
    {
        lesson::print_subtitle("Part 3: placement new 实战 — 构造/析构分离");

        // 使用 C++20 std::construct_at / std::destroy_at 作为 placement new 的替代
        // 它们等价于 ::new(p) T(args) 和 p->~T()，但更安全、更易读
        std::println("placement new 的等价形式:");
        std::println("  ::new (p) T(args)  ←→  std::construct_at(p, args)  (C++20)");
        std::println("  p->~T()            ←→  std::destroy_at(p)         (C++17)");
        std::println("");

        // 使用 std::byte 数组作为原始存储
        alignas(Widget) std::byte storage[sizeof(Widget)];

        // C++20: 使用 construct_at (底层仍是 placement new)
        Widget* w = std::construct_at(
            reinterpret_cast<Widget*>(storage), 42.0
        );
        w->show();

        // 显式析构 — 内存 (storage) 仍然存在且有效
        std::destroy_at(w);
        std::println("对象已析构, 但 storage 数组仍然存在 (栈变量)");

        // 可以在同一地址上重新构造另一个对象 (只要旧对象已析构)
        Widget* w2 = std::construct_at(
            reinterpret_cast<Widget*>(storage), 99.0
        );
        w2->show();
        std::destroy_at(w2);
        std::println("同一块内存上构造/析构了两次不同对象\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: PoolAllocator — 将 MemoryPool 适配为 std::allocator 接口
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: PoolAllocator 实现标准 Allocator 接口，让标准容器使用 MemoryPool
    // 为什么: 展示分配器的角色 — 作为容器和底层内存管理之间的"适配层"
    // 何时用: 想让 std::vector/list 等容器使用你的自定义内存池时
    {
        lesson::print_subtitle("Part 4: PoolAllocator — 适配 std::allocator 接口");

        constexpr std::size_t PoolSize = 16;
        MemoryPool<Widget, PoolSize> pool;

        // 创建绑定到内存池的分配器
        PoolAllocator<Widget, PoolSize> alloc(pool);

        std::println("分配器接口: allocate() / deallocate()");
        Widget* p = alloc.allocate(1);  // 从池中分配 1 个
        std::println("allocate(1) → {} (已用={})", static_cast<void*>(p), pool.used_count());

        // 在该内存上构造 (用池的 construct)
        pool.construct(p, 7.7);
        p->show();
        pool.destroy(p);

        alloc.deallocate(p, 1);          // 归还到池
        std::println("deallocate 后: 已用={}", pool.used_count());

        // 注意: 直接将 PoolAllocator 传给 std::vector 需要满足完整的 Allocator 要求
        // 这里仅展示核心 allocate/deallocate 接口
        std::println("\n将 MemoryPool 用于标准容器的注意事项:");
        std::println("  1. 容量固定 — vector 增长可能超出池容量");
        std::println("  2. list 的节点需要额外的指针存储 — 每个节点 > sizeof(T)");
        std::println("  3. pmr::polymorphic_allocator (C++17) 更适合与标准容器集成");
        std::println();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: 综合演示 — 内存池的完整生命周期
    // ═══════════════════════════════════════════════════════════════════════════
    {
        lesson::print_subtitle("Part 5: 综合演示 — Widget 批量分配与回收");

        constexpr std::size_t PoolSize = 6;
        MemoryPool<Widget, PoolSize> pool;

        Widget::reset_stats();

        // 批量分配 + 构造
        std::println("--- 批量分配 {} 个 Widget ---", PoolSize);
        Widget* items[PoolSize];
        for (std::size_t i = 0; i < PoolSize; ++i) {
            items[i] = pool.allocate();
            pool.construct(items[i], static_cast<double>(i) * 10.0);
        }
        pool.dump_stats();

        // 使用所有 Widget
        std::println("\n所有 Widget:");
        for (std::size_t i = 0; i < PoolSize; ++i) {
            items[i]->show();
        }

        // 逆序析构 + 归还 (遵循 RAII 的逆序原则)
        std::println("\n--- 逆序析构并归还 ---");
        for (int i = static_cast<int>(PoolSize) - 1; i >= 0; --i) {
            pool.destroy(items[i]);
            pool.deallocate(items[i]);
        }

        std::println("构造计数: {}, 析构计数: {} (应相等)",
                     Widget::total_constructed, Widget::total_destructed);
        assert(Widget::total_constructed == Widget::total_destructed);
        pool.dump_stats();
        std::println();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_separator("常见陷阱");
    std::println("1. 忘记手动析构 → 内存被归还但对象资源泄漏 (文件句柄、锁等)");
    std::println("2. 使用已归还的指针 → 悬垂指针, 未定义行为 (freelist 覆盖了数据)");
    std::println("3. 池容量固定 → 分配失败返回 nullptr, 生产代码需处理或实现扩容");
    std::println("4. freelist 的 union 对 T < sizeof(void*) 的类型不适用 → 需要特殊方案");
    std::println("5. 跨线程使用无锁 → 竞态条件, 需要加锁或 thread_local 池");
    std::println("6. 对齐不匹配 → 如果 T 的对齐 > alignof(void*), 可能 misaligned 访问");

    // ═══════════════════════════════════════════════════════════════════════════
    // 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_separator("练习");
    std::println("1. 让 MemoryPool 支持 allocate_array(n) — 分配连续 n 个块");
    std::println("2. 实现线程安全的 ThreadSafeMemoryPool<T, N> (加 std::mutex)");
    std::println("3. 实现动态扩容策略: 池满时从堆申请新 chunk 并链入 freelist");
    std::println("4. 用 std::chrono 对比 MemoryPool vs new/delete 100万次分配的性能");
    std::println("5. (进阶) 阅读 mimalloc (Microsoft) 或 jemalloc (FreeBSD) 的源码");
    std::println("6. (进阶) 用 pmr::memory_resource 重写 MemoryPool, 使其与 pmr 容器兼容");

    return 0;
}

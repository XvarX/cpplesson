#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: memory/memory_pool.hpp — 固定大小内存池 (freelist 实现) 头文件 ║
// ║  提供: MemoryPool 固定大小内存池模板、PoolAllocator 标准分配器适配器          ║
// ║        演示结构体 Widget (带构造/析构追踪)                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <cstddef>      // std::size_t, std::byte
#include <cstdint>      // std::uintptr_t
#include <new>          // std::launder (C++17)
#include <type_traits>  // std::is_trivially_destructible_v
#include <utility>      // std::forward
#include <cassert>
#include <memory>       // std::allocator_traits

/// @brief 演示用结构体 — 构造/析构时打印消息，方便观察内存池行为
/// 追踪每个 Widget 的 ID 及其构造/析构地址
struct Widget {
    int id;
    double value;
    inline static int next_id = 0;
    inline static int total_constructed = 0;
    inline static int total_destructed = 0;

    Widget(double v = 0.0) : id(++next_id), value(v) {
        ++total_constructed;
        std::println("    Widget #{} 构造 (value={}) @ {}", id, value, static_cast<void*>(this));
    }
    ~Widget() {
        ++total_destructed;
        std::println("    Widget #{} 析构 @ {}", id, static_cast<void*>(this));
    }
    void show() const { std::println("    Widget #{}: value={}", id, value); }

    static void reset_stats() { next_id = 0; total_constructed = 0; total_destructed = 0; }
};

// ═══════════════════════════════════════════════════════════════════════════════
// MemoryPool — 固定大小内存池 (freelist 实现)
// ═══════════════════════════════════════════════════════════════════════════════
//
// 是什么: MemoryPool<T, BlockCount> 管理一个固定大小的内存块数组，
//         通过空闲链表 (freelist) 实现 O(1) 的分配和释放。
//
// 为什么: (1) 避免频繁 new/delete 的堆碎片和系统调用开销
//         (2) 缓存友好 — 所有对象存储在连续内存中
//         (3) 确定性 — 无动态内存分配，适合实时/嵌入式/游戏开发
//
// 何时用: (1) 大量固定大小对象的频繁创建/销毁 (粒子系统、网络包、游戏实体)
//         (2) 需要确定性分配性能的场景 (实时系统)
//         (3) 需要控制对象内存布局 (连续存储可大幅提升缓存命中率)
//
// 常见陷阱:
//   [TRAP 1] 忘记手动析构: pool 只管理内存，不自动调用析构函数!
//            用 destroy(p) 或 std::destroy_at(p) 手动析构。所有块在
//            destroy() 析构后，必须由 MemoryPool 析构函数统一释放。
//   [TRAP 2] 使用已归还的指针: release(p) 后继续使用 p 是未定义行为。
//   [TRAP 3] 内存池容量固定: 超出 BlockCount 的分配请求会返回 nullptr，
//            需要在调用方检查。生产环境可采用动态扩容策略。
//   [TRAP 4] 跨线程使用不保证安全: 本实现不是线程安全的。
//            多线程场景需要加锁或使用 thread_local 池。
//   [TRAP 5] 对齐问题: 如果 T 的对齐要求超过 sizeof(void*)，
//            freelist 的 union 方案可能不满足对齐。生产代码应使用 alignas。
//
// 练习:
//   1. 为 MemoryPool 添加 allocate_array(n) 方法，分配连续 n 个块
//   2. 实现线程安全的 ThreadSafeMemoryPool<T, N> (使用 std::mutex)
//   3. 实现动态扩容的内存池，超出容量时从堆分配新的 chunk
//   4. 用 benchmark 对比 MemoryPool vs new/delete 的性能差异
//   5. (进阶) 阅读 mimalloc 或 jemalloc 的源码，理解工业级内存池设计
// ═══════════════════════════════════════════════════════════════════════════════

template <typename T, std::size_t BlockCount>
class MemoryPool {
public:
    /// @brief 构造函数 — 分配一块连续的大内存作为所有块的存储
    /// 使用 placement new 在这块内存上逐个构造 T 对象时需要手动管理
    MemoryPool();
    ~MemoryPool();

    // 禁止拷贝和移动 (管理了原始内存)
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&&) = delete;
    MemoryPool& operator=(MemoryPool&&) = delete;

    /// @brief 从池中分配一个块的裸内存指针 (不调用构造函数)
    /// 返回空闲链表头部节点，将其从 freelist 中移除
    /// @return 指向可用内存的指针，池已满时返回 nullptr
    [[nodiscard]] T* allocate();

    /// @brief 将之前分配的块归还到空闲链表
    /// 调用者负责在归还前手动析构对象 (如果已构造)
    /// @param p 必须是通过 allocate() 获得的指针，否则行为未定义
    void deallocate(T* p) noexcept;

    /// @brief 在已分配的内存上构造 T 对象 (placement new 封装)
    /// @param p 必须是本池 allocate() 返回的有效指针
    /// @param args 传递给 T 构造函数的参数
    template <typename... Args>
    void construct(T* p, Args&&... args);

    /// @brief 析构之前构造的对象 (不释放内存)
    /// @param p 指向已构造的 T 对象
    void destroy(T* p) noexcept;

    // ── 查询接口 ──────────────────────────────────────────────────────────────

    /// @brief 池中已使用的块数
    [[nodiscard]] std::size_t used_count() const noexcept { return m_usedCount; }

    /// @brief 池中空闲的块数
    [[nodiscard]] std::size_t free_count() const noexcept { return BlockCount - m_usedCount; }

    /// @brief 池的总容量 (块数)
    [[nodiscard]] static constexpr std::size_t capacity() noexcept { return BlockCount; }

    /// @brief 是否已满 (无可分配块)
    [[nodiscard]] bool full() const noexcept { return m_usedCount >= BlockCount; }

    /// @brief 是否为空 (无已分配块)
    [[nodiscard]] bool empty() const noexcept { return m_usedCount == 0; }

    /// @brief 打印池的当前状态 (调试用)
    void dump_stats() const;

private:
    // ── freelist 节点: 用 union 节省空间 ─────────────────────────────────────
    // 空闲块的前几个字节存储指向下一个空闲块的指针 (侵入式链表)
    // union 让数据和指针共享同一块内存，空闲时存储指针，使用时存储数据
    struct FreelistNode {
        FreelistNode* next;  // 空闲时: 指向下一个空闲节点
    };

    // 最小块大小校验: 必须能容纳一个指针
    static_assert(sizeof(T) >= sizeof(FreelistNode*),
        "T 的大小必须 >= 指针大小才能使用 freelist 的 union 方案");

    // ── 数据成员 ──────────────────────────────────────────────────────────────
    alignas(T) std::byte m_storage[BlockCount * sizeof(T)]; // 原始存储 (对齐到 T)
    FreelistNode* m_freeList;    // 空闲链表头指针
    std::size_t   m_usedCount;   // 已分配块计数
};


// ═══════════════════════════════════════════════════════════════════════════════
// PoolAllocator — 将 MemoryPool 适配为 std::allocator 兼容接口
// ═══════════════════════════════════════════════════════════════════════════════
//
// 是什么: 符合 C++ 分配器 (Allocator) 要求的包装器，将 MemoryPool 暴露为
//         标准容器的可插拔分配器。
//
// 为什么: (1) 让 std::vector、std::list 等标准容器直接使用 MemoryPool 内存
//         (2) 展示自定义分配器与标准库的集成方式
//         (3) 理解 allocator 接口在真实场景中的角色
//
// 何时用: 想让标准容器使用内存池时，但需要注意:
//         1. 池容量固定 — 容器增长超出容量时会失败
//         2. 不同容器的 allocate(n) 中 n 可能大于 1 (如 vector 扩大容量)
//         3. 仅适用于已知元素数量上限的场景
//
// 注意: 这不是通用的分配器实现! 生产代码建议使用 std::pmr 或专门的内存池库。
// ═══════════════════════════════════════════════════════════════════════════════

template <typename T, std::size_t BlockCount>
class PoolAllocator {
public:
    using value_type = T;

    // 分配器必须可 rebind
    template <typename U>
    struct rebind { using other = PoolAllocator<U, BlockCount>; };

    /// @brief 构造 PoolAllocator — 必须绑定到外部 MemoryPool 实例
    /// @param pool 外部内存池的引用 (生命周期必须长于此分配器)
    explicit PoolAllocator(MemoryPool<T, BlockCount>& pool) noexcept
        : m_pool(&pool) {}

    // 从另一种 T 的 PoolAllocator 拷贝构造 (用于 rebind)
    template <typename U>
    PoolAllocator(const PoolAllocator<U, BlockCount>& other) noexcept
        : m_pool(reinterpret_cast<MemoryPool<T, BlockCount>*>(other.m_pool)) {
        static_assert(sizeof(T) == sizeof(U),
            "PoolAllocator rebind 要求 T 和 U 大小相同 (固定大小池)");
    }

    /// @brief 分配 n 个 T 的原始内存 (n == 1 时从池分配)
    /// @note 当 n > 1 时，池无法满足连续多块分配，退回 ::operator new
    [[nodiscard]] T* allocate(std::size_t n);

    /// @brief 释放之前分配的内存
    void deallocate(T* p, std::size_t n) noexcept;

    // 两个分配器是否等价 — 指向同一个池即等价
    bool operator==(const PoolAllocator& other) const noexcept {
        return m_pool == other.m_pool;
    }

private:
    MemoryPool<T, BlockCount>* m_pool;

    // 允许 rebind 访问私有成员
    template <typename U, std::size_t N>
    friend class PoolAllocator;
};


// ═══════════════════════════════════════════════════════════════════════════════
// 模板实现 (header-only 以便编译器实例化)
// ═══════════════════════════════════════════════════════════════════════════════

// ── MemoryPool 实现 ──────────────────────────────────────────────────────────────

template <typename T, std::size_t BlockCount>
MemoryPool<T, BlockCount>::MemoryPool()
    : m_freeList(nullptr)
    , m_usedCount(0)
{
    // 初始化 freelist: 将所有块串成单向链表
    // 每个空闲块的前 sizeof(FreelistNode*) 字节被重解释为 FreelistNode
    for (std::size_t i = 0; i < BlockCount; ++i) {
        T* block = reinterpret_cast<T*>(&m_storage[i * sizeof(T)]);
        FreelistNode* node = reinterpret_cast<FreelistNode*>(block);
        node->next = m_freeList;   // 头插法构建链表
        m_freeList = node;
    }
    // 此时 freelist: BlockCount-1 → BlockCount-2 → ... → 0 → nullptr
}

template <typename T, std::size_t BlockCount>
MemoryPool<T, BlockCount>::~MemoryPool()
{
    // 注意: 析构函数不负责析构池中的对象!
    // 调用者应在 destroy() 之前确保所有对象已被手动析构。
    // 这里仅负责释放 m_storage 占用的栈/堆内存 (由编译器自动管理)。
    // 如果 m_usedCount != 0，说明有对象未被归还，这是调用方的逻辑错误。
    if (m_usedCount != 0) {
        std::println("[MemoryPool] 警告: 析构时仍有 {} 个块未归还!", m_usedCount);
    }
}

template <typename T, std::size_t BlockCount>
[[nodiscard]] T* MemoryPool<T, BlockCount>::allocate()
{
    if (m_freeList == nullptr) {
        return nullptr;  // 池已满
    }
    // 从 freelist 头部弹出一个节点
    FreelistNode* node = m_freeList;
    m_freeList = node->next;
    ++m_usedCount;
    return reinterpret_cast<T*>(node);
}

template <typename T, std::size_t BlockCount>
void MemoryPool<T, BlockCount>::deallocate(T* p) noexcept
{
    if (p == nullptr) return;
    // 将节点插回 freelist 头部
    FreelistNode* node = reinterpret_cast<FreelistNode*>(p);
    node->next = m_freeList;
    m_freeList = node;
    --m_usedCount;
}

template <typename T, std::size_t BlockCount>
template <typename... Args>
void MemoryPool<T, BlockCount>::construct(T* p, Args&&... args)
{
    // placement new: 在已分配的内存上构造对象
    // 这是本课的核心: 将"内存分配"和"对象构造"分离
    ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
}

template <typename T, std::size_t BlockCount>
void MemoryPool<T, BlockCount>::destroy(T* p) noexcept
{
    // 显式调用析构函数，不释放内存 (内存由池管理)
    if constexpr (!std::is_trivially_destructible_v<T>) {
        p->~T();
    }
}

template <typename T, std::size_t BlockCount>
void MemoryPool<T, BlockCount>::dump_stats() const
{
    std::println("  MemoryPool<T={}> 状态:", typeid(T).name());
    std::println("    容量: {} 块, 已用: {} 块, 空闲: {} 块",
                 BlockCount, m_usedCount, BlockCount - m_usedCount);
    std::println("    总内存: {} 字节, 已用: {} 字节",
                 sizeof(m_storage), m_usedCount * sizeof(T));
}

// ── PoolAllocator 实现 ───────────────────────────────────────────────────────────

template <typename T, std::size_t BlockCount>
[[nodiscard]] T* PoolAllocator<T, BlockCount>::allocate(std::size_t n)
{
    if (n == 1) {
        // 单对象分配: 从池中拿
        T* p = m_pool->allocate();
        if (p != nullptr) return p;
        // 池已满，回退到默认堆分配
        std::println("  [PoolAllocator] 池已满! 回退到 ::operator new");
    }
    // 多对象分配 (如 std::vector 扩容): 跳过池，直接用堆
    return static_cast<T*>(::operator new(n * sizeof(T)));
}

template <typename T, std::size_t BlockCount>
void PoolAllocator<T, BlockCount>::deallocate(T* p, std::size_t n) noexcept
{
    if (n == 1) {
        // 尝试归还到池 — 如果 p 在池的范围内则成功
        // 注意: 这里的"归还到池"是尽力而为的 — 如果 p 不在池范围内则不处理
        m_pool->deallocate(p);
    } else {
        // 多对象释放: 使用全局 delete
        ::operator delete(p, n * sizeof(T));
    }
}

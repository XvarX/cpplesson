// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  03_memory :: 02_placement_new_demo — 定位 new、对齐、手动内存管理              ║
// ║  目标: 理解 placement new、alignas/alignof、std::aligned_*、手动对象生命周期    ║
// ║  前置: 理解 new/delete、构造函数与析构函数                                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <iostream>
#include <print>
#include <string>
#include <memory>       // std::construct_at / std::destroy_at (C++20)
#include <new>          // placement new 运算符声明
#include <cstddef>      // std::byte, std::max_align_t
#include <type_traits>  // std::aligned_storage, std::alignment_of
#include "shared/lesson_utils.hpp"
#include "memory/placement_new.hpp"

int main() {
    lesson::print_header("内存管理 (2): Placement New 与对齐");

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: placement new — 在指定地址构造对象
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: new(地址) 类型{参数} — 在已分配的原始内存上调用构造函数
    // 为什么: (1) 自定义内存池中构造对象 (2) 避免重复分配 (3) 硬件特定地址映射
    // 何时用: 内存池实现、嵌入式系统、高性能场景需要精确控制内存布局
    {
        lesson::print_subtitle("Part 1: placement new 基础");

        // 步骤 1: 分配原始内存 (不调用构造)
        // std::byte 是 C++17 引入的"字节"类型，专门用于原始存储
        alignas(Vec3) std::byte buffer[sizeof(Vec3)];  // 栈上的对齐缓冲区

        std::println("原始缓冲区地址: {}", static_cast<void*>(buffer));
        std::println("缓冲区大小: {} 字节", sizeof(buffer));

        // 步骤 2: 在原始内存上用 placement new 构造对象
        Vec3* pv = new(buffer) Vec3{1.0f, 2.0f, 3.0f};
        std::println("构造后: length = {:.3f}", pv->length());
        // 注意: pv 和 buffer 指向同一地址，但 pv 是 Vec3* 类型

        // 步骤 3: 必须手动调用析构函数 (placement new 创建的对象不能 delete!)
        // ❌ delete pv;  // 错误! buffer 是栈内存，不能 delete
        pv->~Vec3();      // ✅ 手动析构，内存由 buffer 管理
        std::println("(手动调用了析构函数)\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: 内存池模式 — placement new 的核心应用
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: 预分配一大块内存，然后在上面逐个构造对象，避免频繁向 OS 申请
    // 为什么: malloc/free 有开销(系统调用、碎片整理)，批量管理更高效
    // 何时用: 游戏引擎(每帧大量临时对象)、高频交易、嵌入式系统
    {
        lesson::print_subtitle("Part 2: 简易内存池演示");

        // 预分配一块能容纳 3 个 Vec3 的连续内存
        constexpr int pool_size = 3;
        alignas(Vec3) std::byte pool[pool_size * sizeof(Vec3)];

        // 在该内存块上逐个构造 3 个对象
        Vec3* slots[pool_size];
        for (int i = 0; i < pool_size; ++i) {
            // 计算第 i 个对象的偏移地址
            void* addr = pool + i * sizeof(Vec3);
            slots[i] = new(addr) Vec3{float(i), float(i+1), float(i+2)};
        }

        // 使用池中对象
        std::println("池中对象:");
        for (int i = 0; i < pool_size; ++i) {
            std::println("  slots[{}] @ {}  length={:.3f}", i,
                static_cast<void*>(slots[i]), slots[i]->length());
        }

        // 逆序析构 (通常是好习惯，非必须)
        for (int i = pool_size - 1; i >= 0; --i) {
            slots[i]->~Vec3();
        }
        // pool 是栈内存，自动回收
        std::println("(所有对象已析构，内存池在栈上自动回收)\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: alignas / alignof — 内存对齐控制
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: alignas 指定变量/类型的对齐要求; alignof 查询对齐要求
    // 为什么: CPU 访问未对齐的数据会慢(RISC)甚至崩溃(某些 ARM);
    //          SIMD 指令要求 16/32/64 字节对齐; 缓存行对齐避免伪共享(false sharing)
    // 何时用: SIMD 数据处理、多线程共享变量(避免缓存行伪共享)、DMA 缓冲区
    {
        lesson::print_subtitle("Part 3: 对齐控制 (alignas / alignof)");

        // 基本类型对齐
        std::println("基本类型对齐:");
        std::println("  char:      alignof={}, sizeof={}", alignof(char), sizeof(char));
        std::println("  int:       alignof={}, sizeof={}", alignof(int), sizeof(int));
        std::println("  double:    alignof={}, sizeof={}", alignof(double), sizeof(double));
        std::println("  max_align_t (标量最大对齐): {}", alignof(std::max_align_t));

        // 自定义对齐
        std::println("\nalignas(64) 自定义类型:");
        CacheLineAligned ca;
        ca.info();
        // 64 字节对齐确保不同线程的变量落在不同缓存行，避免伪共享

        // alignas 对栈变量的影响
        std::println("\n栈变量对齐示例:");
        alignas(32) int aligned_int = 42;
        std::println("  alignas(32) int 地址: {}", static_cast<void*>(&aligned_int));
        // 检查地址是否 32 的倍数
        auto addr_val = reinterpret_cast<std::uintptr_t>(&aligned_int);
        std::println("  地址是 32 的倍数: {}", (addr_val % 32 == 0));
        std::println();
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: std::construct_at / std::destroy_at (C++20) — 现代替代
    // ═══════════════════════════════════════════════════════════════════════════
    // 是什么: C++20 引入的函数，在指定地址构造/析构对象
    // 为什么: placement new 语法怪异(new(addr) T)，construct_at 更语义化
    //         且 constexpr 支持 (placement new 在 constexpr 中不可用!)
    // 何时用: C++20+ 项目中替代 placement new，尤其是 constexpr 容器实现
    {
        lesson::print_subtitle("Part 4: std::construct_at / std::destroy_at (C++20)");

        alignas(Vec3) std::byte buf[sizeof(Vec3)];

        // C++20 写法: 使用 std::construct_at (constexpr 友好)
        Vec3* p = std::construct_at(
            reinterpret_cast<Vec3*>(buf),  // 目标地址
            4.0f, 5.0f, 6.0f              // 转发给构造函数的参数
        );
        std::println("通过 construct_at 构造, length = {:.3f}", p->length());

        // C++20 写法: 使用 std::destroy_at (constexpr 友好)
        std::destroy_at(p);  // 等价于 p->~Vec3()
        std::println("通过 destroy_at 析构\n");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_separator("常见陷阱");
    std::println("1. 对 placement new 构造的对象使用 delete → 未定义行为，大概率崩溃");
    std::println("2. 忘记手动调用析构 → 资源泄漏(打开的文件、锁、数据库连接等不释放)");
    std::println("3. 缓冲区未正确对齐 → 某些架构上崩溃或性能严重下降");
    std::println("4. 缓冲区不够大 → 栈/堆破坏，极其危险的内存越界");
    std::println("5. 重复构造同地址而不先析构 → 旧对象资源泄漏，未定义行为");
    std::println("6. aligned_storage 已在 C++23 中弃用 → 改用 alignas + std::byte 数组");

    // ═══════════════════════════════════════════════════════════════════════════
    // 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_separator("练习");
    std::println("1. 用 placement new 在栈上的 char 数组中构造一个 std::string，使用后手动析构");
    std::println("2. 写一个固定大小的对象池类模板，支持 acquire() 和 release()");
    std::println("3. 对比 std::construct_at 和 placement new 的用法差异");
    std::println("4. 故意对 placement new 创建的对象调用 delete，观察程序行为");
    std::println("5. (进阶) 实现一个 SIMD 友好的 4x4 矩阵类，使用 alignas(16)");
    std::println("6. (进阶) 研究 std::aligned_alloc (C17/C++17) 与 aligned new 的区别");

    return 0;
}

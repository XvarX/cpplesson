// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — 类与面向对象编程                                      ║
// ║  课时: 07_raii_and_resource — RAII 与资源管理                            ║
// ║  学习目标:                                                                 ║
// ║    - 理解 RAII (Resource Acquisition Is Initialization) 的核心思想        ║
// ║    - 掌握 scope_guard / scope_exit 模式                                  ║
// ║    - 理解 lock_guard 的实现原理                                          ║
// ║    - 实践文件 RAII 包装的设计和实现                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "classes/raii.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <string>
#include <exception>
#include <functional>
#include <cstdio>
#include <cstring>
#include <cerrno>

using namespace lesson;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: RAII 核心思想 — 资源获取即初始化
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 将资源的生命周期绑定到对象的生命周期
//        构造时获取资源 → 析构时自动释放资源
// 为什么: 消除手动释放、异常安全、代码简洁
// 何时用: 管理任何需要配对获取/释放的资源

void bad_example() {
    int* data = new int[100]; // 获取资源
    // ... 复杂逻辑 ...
    // 如果中途抛异常或提前 return → delete[] 不会执行 → 内存泄漏!
    delete[] data;
}

void part1_raii_core() {
    print_subtitle("Part 1: RAII 核心思想");

    RaiiArray arr(5);  // 构造分配
    arr[0] = 42;
    std::println("arr[0] = {}", arr[0]);
    // 离开作用域 → 自动调用析构 → 无需手动 delete
    std::println("arr 离开作用域时自动释放 — 异常安全!");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: ScopeGuard — 确保清理代码执行
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 在作用域结束时自动执行指定操作 (类似 Go 的 defer)
// 为什么: 确保即使异常或提前返回，清理/回滚代码也一定执行
// 何时用: 需要在函数出口执行清理 (保存状态、回滚事务、关闭句柄)

void part2_scope_guard() {
    print_subtitle("Part 2: ScopeGuard 模式");

    std::println("--- 正常流程 ---");
    {
        bool succeeded = false;
        auto guard = ScopeGuard([&] {
            if (!succeeded) std::println("  ⚠️ 回滚: 操作失败, 执行清理");
        });

        // 模拟操作...
        succeeded = true;  // 成功了
        guard.dismiss();   // 取消清理
        std::println("  操作成功, 清理被取消");
    }

    std::println("\n--- 异常流程 ---");
    try {
        auto guard = ScopeGuard([] {
            std::println("  ⚠️ 回滚: 异常发生, 自动清理资源");
        });
        throw std::runtime_error("模拟错误");
        guard.dismiss();  // 永远不会执行到这里
    } catch (...) {
        std::println("  异常被捕获");
    }
    std::println("ScopeGuard 确保无论正常还是异常, 清理代码都会执行");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: LockGuard 原理 — 互斥锁的 RAII 包装
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 构造时加锁, 析构时解锁 (std::lock_guard / std::scoped_lock 的简化版)
// 为什么: 永远不用手动 unlock; 异常安全; 避免死锁 (scoped_lock)
// 何时用: 每次使用互斥锁时都应使用 RAII 包装而非手动 lock/unlock

void part3_lock_guard() {
    print_subtitle("Part 3: LockGuard 原理");

    SimpleMutex mtx;

    std::println("  临界区开始:");
    {
        LockGuard guard(mtx);
        // 持锁中...任何异常都会触发 guard 析构 → 解锁
        if (mtx.is_locked()) std::println("    🟢 已加锁, 安全执行");
    } // ← guard 析构自动解锁
    std::println("  临界区结束 (锁已释放)");

    // 对比手动操作:
    // mtx.lock();
    // // ... 如果这里抛异常 → 永远不会 unlock → 死锁!
    // mtx.unlock();
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: 文件 RAII 包装 — 实践完整示例
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 用 RAII 封装 FILE*，自动管理文件生命周期
// 为什么: 再也不用写 fclose — 文件绝不可能泄漏
// 何时用: 管理任何 OS 句柄 (文件/套接字/管道等)

void part4_file_raii() {
    print_subtitle("Part 4: 文件 RAII 包装");

    // 写文件
    {
        File f("test_raii.txt", "w");
        if (f.is_open()) {
            f.write("Hello RAII!\n");
            f.write("文件会在析构时自动关闭\n");
        }
    } // ← f 在此析构: 自动 flush 并关闭文件

    // 读文件
    {
        File f("test_raii.txt", "r");
        if (f.is_open()) {
            auto content = f.read_all();
            std::print("读取内容:\n{}", content);
        }
    } // ← 自动关闭

    // 清理
    std::remove("test_raii.txt");
    std::println("\n对比: 手动 fopen/fclose 容易在异常路径忘记关闭!");
}

// ═══════════════════════════════════════════════════════════════════════════════
// ⚠️ 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. RAII 对象在栈上 (自动存储期) 才有效 — 不要 new 一个 RAII 对象再忘记 delete
// 2. 拷贝语义要明确 — 资源通常应禁止拷贝 (如 unique_ptr) 或引用计数 (shared_ptr)
// 3. 析构函数中不要抛异常 — 导致 std::terminate
// 4. 移动后的对象状态要保证"有效但未指定" — 析构时不能出问题
// 5. ScopeGuard 记得 dismiss 的场景: 只有失败才需要回滚

// ═══════════════════════════════════════════════════════════════════════════════
// ✏️ 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 实现 TimerGuard: 构造时记录开始时间, 析构时打印耗时
// 2. 实现 RollbackGuard: 记录数据库操作, 析构时如果没 commit 则 rollback
// 3. 扩展 File 类, 添加 try_read_line / try_write_line
// 4. 实现 AutoCloser 模板: 接受任意资源和关闭函数 (类似 unique_ptr 的自定义删除器)

int main() {
    print_header("07 RAII 与资源管理");
    part1_raii_core();
    part2_scope_guard();
    part3_lock_guard();
    part4_file_raii();
    print_separator();
    std::println("🎉 07_raii_and_resource 完成！");
    return 0;
}

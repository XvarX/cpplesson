// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — 类与面向对象编程                                      ║
// ║  课时: 09_union_and_volatile — union 与 volatile                          ║
// ║  学习目标:                                                                 ║
// ║    - 理解 union 的存储模型与活跃成员规则                                   ║
// ║    - 掌握匿名 union 的使用方式                                            ║
// ║    - 学会管理带非平凡成员的 union (placement new / 显式析构)               ║
// ║    - 理解 volatile 的语义: 何时用 (MMIO/信号)、何时不用 (多线程)          ║
// ║    - 对比 union vs variant、volatile vs atomic                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "classes/union_and_volatile.hpp"
#include "shared/lesson_utils.hpp"
#include <print>
#include <variant>
#include <atomic>

using namespace lesson;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: union 基本用法与活跃成员规则
// ═══════════════════════════════════════════════════════════════════════════════
// union 的所有成员共享内存, 只有最后写入的成员处于活跃状态; 读取非活跃成员 = UB

void part1_basic_union() {
    print_subtitle("Part 1: union 基本用法与活跃成员规则");

    TaggedUnion val;
    val.set_int(42);
    val.print();

    val.set_float(3.14f);
    val.print();

    val.set_text("你好 C++23!");
    val.print();

    std::println("  活跃成员规则: 只能读最后写入的成员, 读取: \"{}\"", val.as_text());
    std::println("  当前 kind() == Int? {} (不是! 是 Text)", val.kind() == ValueKind::Int ? "是" : "否");
    std::println("  union 大小: {} 字节 (max of int/float/char[20])", sizeof(TaggedUnion));
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: 匿名 union — 无名称的 union
// ═══════════════════════════════════════════════════════════════════════════════
// 匿名 union 的成员直接暴露在包围作用域, 无需通过 union 变量名间接访问

void part2_anonymous_union() {
    print_subtitle("Part 2: 匿名 union");

    PacketHeader hdr;
    hdr.length = 64;
    hdr.checksum = 0xA5C3;

    hdr.flags.version = 2;   // 通过位域赋值 (高 4 位)
    hdr.flags.type    = 7;   // 通过位域赋值 (低 4 位)
    // version=2(b0010), type=7(b0111) → raw_flags=0b00100111=0x27
    hdr.describe();

    std::println("  直接读取 raw_flags: 0x{:02X} (与位域共享同一字节)", hdr.raw_flags);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: 带非平凡成员的 union (C++11+)
// ═══════════════════════════════════════════════════════════════════════════════
// union 中包含 std::string 等非平凡成员时, 必须手动 placement new / 显式析构

void part3_nontrivial_union() {
    print_subtitle("Part 3: 带非平凡成员的 union");

    StringOrDouble sd;
    std::println("  初始状态: Empty");

    sd.set_double(2.71828);
    std::println("  设为 double: {:.5f}", sd.as_double());

    sd.set_string("hello world");
    std::println("  设为 string: \"{}\"", sd.as_string());

    // 移动构造: 从现有对象窃取资源
    StringOrDouble sd2(std::move(sd));
    std::println("  移动后源为空? {}  目标: \"{}\"", sd.is_empty() ? "是" : "否", sd2.as_string());

    // ⚠ 拷贝构造被 delete — 非平凡 union 的拷贝语义复杂且易出错
    // StringOrDouble sd3(sd2);  // ❌ 编译错误
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: volatile — 硬件 MMIO 与信号处理
// ═══════════════════════════════════════════════════════════════════════════════
// volatile 禁止编译器优化该变量的读写; 用于 MMIO 和信号处理, 但绝不用于多线程!

void part4_volatile() {
    print_subtitle("Part 4: volatile — 硬件 MMIO 与信号处理");

    // ── MMIO 模拟 ──
    DeviceRegister status_reg;
    status_reg.write(0x00);
    std::println("  初始状态寄存器: 0x{:08X}", status_reg.read());

    status_reg.set_bits(0x01);           // 模拟硬件设置就绪位 (bit 0)
    std::println("  设置就绪位后: 0x{:08X}", status_reg.read());

    status_reg.clear_bits(0x01);
    std::println("  清除就绪位后: 0x{:08X}", status_reg.read());

    // 关键演示: 没有 volatile, 下面的轮询可能被优化成死循环!
    std::println("  开始轮询等待就绪位 (volatile 确保每次真正读取内存)...");
    status_reg.set_bits(0x01);
    status_reg.wait_until_ready(0x01, 10);

    // ── 信号处理演示 ──
    std::println("\n  ── 信号处理: volatile sig_atomic_t ──");
    std::println("  g_signal_received 初始: {}", static_cast<int>(g_signal_received));
    simulate_signal_arrival();
    if (g_signal_received)
        std::println("  主程序检测到信号, 安全退出 (volatile 确保从内存重读)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: 对比 — union vs variant, volatile vs atomic
// ═══════════════════════════════════════════════════════════════════════════════
// variant = 类型安全的 union; atomic = 线程安全的"volatile"

void part5_comparison() {
    print_subtitle("Part 5: 对比 — union vs variant / volatile vs atomic");

    // ── union vs variant: variant 自动追踪活跃成员, 错误访问抛异常而非 UB ──
    {
        std::variant<int, float, std::string> v;

        v = 42;
        std::println("  variant<int>: {}", std::get<int>(v));

        v = std::string("C++23 类型安全");
        std::println("  variant<string>: \"{}\"", std::get<std::string>(v));

        // 访问错误类型: 抛出 std::bad_variant_access, 而非 UB!
        try {
            std::get<int>(v);  // 当前持有 string
        } catch (const std::bad_variant_access& e) {
            std::println("  variant 类型错误检测: {}", e.what());
        }
        std::println("  variant 大小: {} 字节 (含类型索引)", sizeof(v));
    }

    // ── volatile vs atomic: atomic 保证原子性+内存顺序, volatile 只禁止编译器优化 ──
    {
        std::println("\n  ── volatile vs atomic ──");

        volatile int vol_counter = 0;
        vol_counter++;  // ❌ 读-改-写三步非原子, 多线程不安全!

        std::atomic<int> atom_counter{0};
        atom_counter.fetch_add(1, std::memory_order_relaxed);  // ✅ 原子操作

        std::println("  volatile int++ : 非原子 → 多线程不安全");
        std::println("  atomic<int>++  : 原子 → 线程安全");
        std::println("  记住: volatile ≠ 原子 ≠ 线程安全");
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// ⚠️ 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 读取 union 的非活跃成员 = UB (C++不允许"类型双关", 要用 std::bit_cast)
// 2. 带标签的 union 中标签与实际成员不一致 (忘记同步更新)
// 3. 非平凡成员的 union 忘记 placement new / 显式析构 → 泄漏或双重释放
// 4. 误用 volatile 做线程同步 → 数据竞争, 行为未定义
// 5. 过分依赖 volatile 阻止优化 → 正确方式是 std::atomic + memory_order

// ═══════════════════════════════════════════════════════════════════════════════
// ✏️ 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 实现 IpAddress union: 以 uint32_t 或 uint8_t[4] 两种形式读写 IPv4 地址
// 2. 为 StringOrDouble 实现拷贝构造和拷贝赋值
// 3. 实现 UartRegisters 结构体: 用 volatile 模拟 UART 寄存器
// 4. 用 std::variant 重写 StringOrDouble, 对比两种实现的复杂度
// 5. 写多线程程序验证: volatile 计数器有竞态, atomic 计数器正确

int main() {
    print_header("09 union 与 volatile");

    part1_basic_union();
    part2_anonymous_union();
    part3_nontrivial_union();
    part4_volatile();
    part5_comparison();

    print_separator();
    std::println("🎉 09_union_and_volatile 完成！");
    return 0;
}

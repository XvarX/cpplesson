// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 02_classes — union 与 volatile (union_and_volatile)                  ║
// ║  内容: union(活跃成员规则/匿名union/带非平凡成员的union)、                    ║
// ║        volatile(硬件MMIO/信号处理/为何不用于多线程)、                         ║
// ║        与 variant/atomic 对比                                                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#pragma once

#include <print>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <new>
#include <utility>
#include <csignal>

// ═══════════════════════════════════════════════════════════════════════════════
// TaggedUnion — union 基本用法与活跃成员规则
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: union 的所有成员共享同一块内存, 大小等于最大成员的大小
//        同一时间只有一个成员处于活跃状态 (active member)
// 活跃成员规则: 只能安全读取最后写入的成员; 读取其他成员是未定义行为
// 为什么: ① 节省内存 (多种类型复用同一空间)
//         ② 与 C API 交互 (很多 C 库用 union 表示变体数据)
//         ③ 实现 discriminated union (带标签的联合体)
// 何时用: 嵌入式/系统编程中的变体数据、协议解析、与 C 库互操作

enum class ValueKind : uint8_t { Int, Float, Text };

class TaggedUnion {
public:
    TaggedUnion() : kind_(ValueKind::Int) { data_.i = 0; }

    void set_int(int v) {
        kind_ = ValueKind::Int;
        data_.i = v;            // i 成为活跃成员
    }

    void set_float(float v) {
        kind_ = ValueKind::Float;
        data_.f = v;            // f 成为活跃成员
    }

    void set_text(const char* text) {
        kind_ = ValueKind::Text;
        // 安全拷贝: 最多容纳 19 个有效字符 + '\0'
        std::strncpy(data_.text, text, sizeof(data_.text) - 1);
        data_.text[sizeof(data_.text) - 1] = '\0';
    }

    ValueKind kind() const { return kind_; }

    int as_int() const { return data_.i; }

    float as_float() const { return data_.f; }

    std::string_view as_text() const { return data_.text; }

    void print() const;

private:
    // i, f, text 共用 20 字节的内存空间
    union Data {
        int i;
        float f;
        char text[20];

        Data() : i(0) {}   // C++11: union 可以有自己的构造/析构
        ~Data() {}
    };

    Data data_;
    ValueKind kind_;  // 标签: 手动跟踪当前活跃成员
};

// ═══════════════════════════════════════════════════════════════════════════════
// PacketHeader — 匿名 union 演示
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: 匿名 union 没有名称, 其成员直接作为外围作用域的变量访问
// 为什么: 省去一层命名空间的间接访问 (如 header..version), 代码更简洁
// 何时用: 需要以不同方式解释同一块内存的结构体 (协议头、颜色值等)

struct PacketHeader {
    uint16_t length;

    // 匿名 union: flags 和 raw_flags 共享同一个字节
    union {
        struct {
            uint8_t version : 4;  // 位域: 高 4 位
            uint8_t type    : 4;  // 位域: 低 4 位
        } flags;
        uint8_t raw_flags;        // 以原始字节读取
    };
    // 匿名 union 的成员直接提升到 PacketHeader 作用域
    // 可以用 header.flags.version 或 header.raw_flags 直接访问

    uint16_t checksum;

    void describe() const;
};

// ═══════════════════════════════════════════════════════════════════════════════
// StringOrDouble — 带非平凡成员的 union (C++11+)
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: C++11 起 union 可以包含具有非平凡构造/析构/拷贝的成员 (如 std::string)
// 为什么: 表达更丰富的变体数据, 无需通过指针间接持有复杂类型
// 何时用: 需要 union 但成员包含 std::string 等 RAII 类型时
//
// 关键约束: 如果 union 包含非平凡成员, 编译器会删除 union 的默认特殊成员函数;
//          你必须用 placement new 手动构造、用显式析构调用手动销毁

class StringOrDouble {
public:
    enum class Kind { String, Double, Empty };

    StringOrDouble() : kind_(Kind::Empty) {}
    ~StringOrDouble();

    // 不允许拷贝 — union 中包含非平凡成员时拷贝语义复杂且易出错
    StringOrDouble(const StringOrDouble&) = delete;
    StringOrDouble& operator=(const StringOrDouble&) = delete;

    // 允许移动
    StringOrDouble(StringOrDouble&& other) noexcept;
    StringOrDouble& operator=(StringOrDouble&& other) noexcept;

    void set_string(std::string_view s);
    void set_double(double d);
    void destroy();  // 手动销毁当前活跃成员

    Kind kind() const { return kind_; }
    std::string_view as_string() const;
    double as_double() const;

    bool is_empty() const { return kind_ == Kind::Empty; }

private:
    union Storage {
        std::string str;  // 非平凡成员 — 编译器不会自动调用其构造/析构!
        double d;

        Storage()  {}     // 什么都不做 — 由外部类 StringOrDouble 手动管理生命周期
        ~Storage() {}     // 同上
    };

    Storage storage_;
    Kind kind_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// DeviceRegister — volatile 模拟硬件 MMIO 寄存器
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: volatile 告诉编译器"每次访问必须从内存读取/写入, 禁止优化掉"
// 为什么: 硬件寄存器的值可能被外部设备异步修改, 编译器不知道这一点;
//        若没有 volatile, 编译器可能:
//          ① 优化掉看似"冗余"的连续读取 (认为值不可能被外部改变)
//          ② 将值缓存在寄存器中而不重新从内存加载
//          ③ 重排或合并读写操作
// 何时用: ① 内存映射 I/O (MMIO) — 读写硬件寄存器
//         ② 信号处理器 (signal handler) 中与主程序共享的变量 (配合 sig_atomic_t)
//         ③ setjmp/longjmp — 确保局部变量不被优化到寄存器
// 绝不用于: 多线程同步 — volatile 不保证原子性、不建立内存屏障、不阻止 CPU 乱序执行

class DeviceRegister {
public:
    // 构造: 模拟一个映射到特定地址的 32 位硬件状态寄存器
    explicit DeviceRegister();

    // 从"硬件寄存器"读取 (volatile 读取 — 编译器不会优化掉)
    volatile uint32_t read() const;

    // 向"硬件寄存器"写入 (volatile 写入 — 编译器不会重排或消除)
    void write(uint32_t value);

    // MMIO 常见操作: 设置掩码中的位
    void set_bits(uint32_t mask);

    // MMIO 常见操作: 清除掩码中的位
    void clear_bits(uint32_t mask);

    // 轮询等待某个状态位就绪 (模拟硬件驱动的自旋等待)
    void wait_until_ready(uint32_t ready_mask, int max_spin = 1000);

private:
    volatile uint32_t backing_store_ = 0;  // 模拟的硬件寄存器实际存储
    volatile uint32_t* reg_;               // 指向易变内存的指针
};

// ═══════════════════════════════════════════════════════════════════════════════
// 信号处理演示 — volatile sig_atomic_t 的正确用法
// ═══════════════════════════════════════════════════════════════════════════════
// 是什么: volatile sig_atomic_t 是信号处理器中唯一可移植的共享变量
// 为什么: ① sig_atomic_t — 平台保证对其读写是原子的 (单指令即可完成)
//         ② volatile — 确保编译器每次都从内存读取, 不会把值缓存在寄存器中
//         ③ 两者配合才是信号安全的 (各自只解决一半问题)
// 何时用: 信号处理器只设置标志, 主循环轮询检查 (信号处理器中能做的事极其有限)

extern volatile sig_atomic_t g_signal_received;  // 定义在源文件中

void simulate_signal_arrival();  // 模拟信号的到达 (设置 g_signal_received)

// ═══════════════════════════════════════════════════════════════════════════════
// ⚠️ 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 读取 union 的非活跃成员 = 未定义行为 (UB)
//    (C++ 允许通过"公共初始序列"读取, 但仅限于标准布局结构体, 实际中不推荐依赖)
// 2. 标签不一致 — 设置值后忘记更新 kind_ / 更新 kind_ 后忘记设置值
// 3. union 中包含非平凡成员忘记手动 placement new / 显式析构 → 泄漏或崩溃
// 4. union 中包含指针成员 → 拷贝/移动语义极易出错, 建议禁用拷贝
// 5. 误用 volatile 做多线程同步 → volatile 不保证原子性, 数据竞争仍然存在
// 6. volatile 读写不建立内存屏障 → CPU 仍可能乱序执行, 需要用 std::atomic 或内存栅栏
// 7. 过度 volatile → 阻止大量编译器优化, 性能显著下降

// ═══════════════════════════════════════════════════════════════════════════════
// ✏️ 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 实现 IpAddress union: 支持以 uint32_t 或 uint8_t[4] 两种形式读写 IPv4 地址
// 2. 为 StringOrDouble 实现拷贝构造和拷贝赋值 (提示: 根据 kind_ 分类处理)
// 3. 实现 UartRegisters 结构体: 用 volatile 指针模拟 UART 硬件寄存器的读写操作
// 4. 用 std::variant 重写 StringOrDouble, 对比两种实现的复杂度与安全性
// 5. 写一个多线程程序: 分别用 volatile int 和 std::atomic<int> 做计数器,
//    观察 volatile 版本的竞态条件导致的错误计数

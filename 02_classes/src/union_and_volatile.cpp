// union_and_volatile 模块 — 非平凡成员的 union 和 volatile 相关实现
#include "classes/union_and_volatile.hpp"

// ═══════════════════════════════════════════════════════════════════════════════
// TaggedUnion::print — 根据标签打印当前活跃成员的值
// ═══════════════════════════════════════════════════════════════════════════════

void TaggedUnion::print() const {
    switch (kind_) {
    case ValueKind::Int:
        std::println("  TaggedUnion[Int]: {}", data_.i);
        break;
    case ValueKind::Float:
        std::println("  TaggedUnion[Float]: {:.2f}", data_.f);
        break;
    case ValueKind::Text:
        std::println("  TaggedUnion[Text]: \"{}\"", data_.text);
        break;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// PacketHeader::describe — 以两种方式解读 flags 字节
// ═══════════════════════════════════════════════════════════════════════════════

void PacketHeader::describe() const {
    std::println("  长度: {}, 校验和: 0x{:04X}", length, checksum);
    // 通过位域解读
    std::println("  flags.version={}, flags.type={}  (位域方式)",
                 flags.version, flags.type);
    // 通过原始字节解读 (匿名 union 成员可以直接访问)
    std::println("  raw_flags=0x{:02X}  (原始字节方式)", raw_flags);
}

// ═══════════════════════════════════════════════════════════════════════════════
// StringOrDouble — 带非平凡成员的 union 生命周期管理
// ═══════════════════════════════════════════════════════════════════════════════

StringOrDouble::~StringOrDouble() {
    destroy();  // 确保非平凡成员被正确析构
}

// 移动构造: 从 other 窃取资源, 并将 other 置空
StringOrDouble::StringOrDouble(StringOrDouble&& other) noexcept
    : kind_(other.kind_)
{
    switch (kind_) {
    case Kind::String:
        // placement new: 在 union 存储中移动构造 std::string
        // 必须用 placement new — 直接赋值会破坏 union 语义
        new (&storage_.str) std::string(std::move(other.storage_.str));
        break;
    case Kind::Double:
        storage_.d = other.storage_.d;
        break;
    case Kind::Empty:
        break;
    }
    // 将源对象置空: 先销毁其活跃成员, 再标记为空
    other.kind_ = Kind::Empty;
    // 注意: 这里不调用 other.destroy() 因为 str 已经被 move 走了,
    //       空字符串的析构也可以, 但直接将 kind_ 设为 Empty 更高效
}

// 移动赋值: 先销毁自身, 再从 other 窃取
StringOrDouble& StringOrDouble::operator=(StringOrDouble&& other) noexcept {
    if (this != &other) {
        destroy();          // 先清理自身持有的资源
        kind_ = other.kind_;
        switch (kind_) {
        case Kind::String:
            new (&storage_.str) std::string(std::move(other.storage_.str));
            break;
        case Kind::Double:
            storage_.d = other.storage_.d;
            break;
        case Kind::Empty:
            break;
        }
        other.kind_ = Kind::Empty;  // 源对象标记为空
    }
    return *this;
}

// set_string: 先销毁旧成员, 再用 placement new 构造新字符串
void StringOrDouble::set_string(std::string_view s) {
    destroy();
    // placement new: 在 union 的内存地址上显式调用 std::string 的构造函数
    new (&storage_.str) std::string(s);
    kind_ = Kind::String;
}

// set_double: 先销毁旧成员, 再设置 double (double 是平凡类型, 直接赋值即可)
void StringOrDouble::set_double(double d) {
    destroy();
    storage_.d = d;  // 平凡类型: 不需要 placement new
    kind_ = Kind::Double;
}

// destroy: 如果当前持有 std::string, 显式调用其析构函数
// 这是 union 中包含非平凡成员时必须手动完成的工作
void StringOrDouble::destroy() {
    if (kind_ == Kind::String) {
        storage_.str.~basic_string();  // 显式析构调用 — 释放字符串的堆内存
    }
    kind_ = Kind::Empty;
}

// 访问器: 调用者应确保 kind_ 匹配 (否则是逻辑错误, 但不会 UB)
std::string_view StringOrDouble::as_string() const {
    return storage_.str;
}

double StringOrDouble::as_double() const {
    return storage_.d;
}

// ═══════════════════════════════════════════════════════════════════════════════
// DeviceRegister — volatile MMIO 模拟
// ═══════════════════════════════════════════════════════════════════════════════

DeviceRegister::DeviceRegister()
    : backing_store_(0)
    , reg_(&backing_store_)  // 指针指向模拟的硬件存储
{}

// volatile 读取: 每次调用都从 reg_ 指向的内存读取
// 即使编译器"看到"刚读过同一个地址, 也不能跳过这次读取
volatile uint32_t DeviceRegister::read() const {
    return *reg_;  // volatile 解引用 — 编译器保证生成实际的内存读取指令
}

// volatile 写入: 确保值被写入到内存, 不会被编译器优化掉
void DeviceRegister::write(uint32_t value) {
    *reg_ = value;  // volatile 存储 — 编译器保证生成实际的内存写入指令
}

// MMIO 常见模式: 读-修改-写 (RMW)
void DeviceRegister::set_bits(uint32_t mask) {
    write(read() | mask);   // 注意: 这里 read() 和 write() 都不可被优化掉
}

void DeviceRegister::clear_bits(uint32_t mask) {
    write(read() & ~mask);
}

// 模拟硬件驱动中的自旋等待: 反复读取状态寄存器直到就绪位被置位
// 这里 volatile 至关重要 — 如果没有 volatile, 编译器可能将循环优化成:
//   if (*reg_ & ready_mask == 0) { infinite_loop; }
// 因为编译器认为 *reg_ 的值不会在循环中改变
void DeviceRegister::wait_until_ready(uint32_t ready_mask, int max_spin) {
    int spins = 0;
    while (((read() & ready_mask) == 0) && spins < max_spin) {
        // volatile 确保每轮循环都真正从内存读取, 而不是用寄存器缓存的值
        ++spins;
    }
    if (spins >= max_spin) {
        std::println("  [警告] 等待就绪超时, 已空转 {} 轮", spins);
    } else {
        std::println("  状态寄存器就绪, 空转 {} 轮后检测到", spins);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// 信号处理演示 — 全局标志位
// ═══════════════════════════════════════════════════════════════════════════════
// 定义: 信号处理器和主程序之间共享的唯一可移植变量类型
// volatile:    确保编译器每次都从内存重新读取, 不会把值缓存在寄存器中
// sig_atomic_t: 确保读写操作本身是原子的 (单个 CPU 指令完成)
volatile sig_atomic_t g_signal_received = 0;

void simulate_signal_arrival() {
    // 在真实的信号处理器中, 你只能做极少的事情:
    //   - 设置 volatile sig_atomic_t 标志
    //   - 调用 _exit() 或 signal() (特定实现)
    //   - 不能调用 printf/malloc/任何非异步信号安全的函数!
    g_signal_received = 1;
    std::println("  [模拟信号] SIGINT 到达, 设置 g_signal_received = 1");
}

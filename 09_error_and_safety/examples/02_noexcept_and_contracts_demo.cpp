// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 09_error_and_safety — 错误处理与安全                                 ║
// ║  主题: 02_noexcept_and_contracts — noexcept 与契约                         ║
// ║  目标: 理解 noexcept 说明符/运算符、异常安全保证、noexcept 移动的重要性、  ║
// ║        [[nodiscard]]                                                        ║
// ║                                                                             ║
// ║  跟着敲: 理解编译期契约与现代 C++ 的异常安全设计                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <vector>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include "error_safety/noexcept_contracts.hpp"
#include "shared/lesson_utils.hpp"

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: noexcept 说明符 — 承诺函数不抛出异常                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part1_noexcept_specifier() {
    lesson::print_separator("Part 1: noexcept 说明符");

    // ── noexcept 运算符 (编译期查询) ──
    // 编译器可以在编译时检查一个表达式是否会抛出异常
    constexpr bool b1 = noexcept(safeAdd(1, 2));    // true: safeAdd 是 noexcept
    constexpr bool b2 = noexcept(std::string{} = std::string{});  // false: 可能分配内存

    std::println("  noexcept(safeAdd(1, 2)) = {}", b1);
    std::println("  noexcept(string 赋值) = {}", b2);

    // 在运行时代码中使用
    std::println("  safeAdd(3, 4) = {}", safeAdd(3, 4));

    // ── 如果 noexcept 函数抛出异常会怎样? ──
    // 下面的代码故意在 noexcept 函数中抛出异常（注释掉以免程序终止）：
    // auto badNoexcept = []() noexcept { throw std::runtime_error("oops"); };
    // badNoexcept();  // → std::terminate() — 程序直接终止，栈不展开
    std::println("  ℹ️  如果 noexcept 函数抛出异常 → std::terminate()");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: 异常安全保证的三个级别                                            ║
// ║                                                                             ║
// ║  1. 不抛出保证 (Nothrow / No-fail): 函数绝不抛出异常                      ║
// ║     - 用 noexcept 标记                                                     ║
// ║     - 例: 析构函数、swap、简单的赋值                                       ║
// ║                                                                             ║
// ║  2. 强保证 (Strong guarantee): 操作要么完全成功，要么回滚到操作前状态     ║
// ║     - 也称"提交或回滚"语义                                                ║
// ║     - 例: 先复制再交换 (copy-and-swap)                                    ║
// ║                                                                             ║
// ║  3. 基本保证 (Basic guarantee): 异常发生时无资源泄漏，对象处于有效状态    ║
// ║     - 是写函数的最低要求                                                   ║
// ║     - 对象的 invariant 可能不完全保持                                      ║
// ║                                                                             ║
// ║  4. 无保证 (No guarantee): 异常后不要使用该对象 — 尽量避免                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part2_exception_safety() {
    lesson::print_separator("Part 2: 异常安全保证");

    // ── 示例: 强保证 (copy-and-swap) ──
    SafeString original{"原始数据"};
    SafeString target{"目标数据"};

    std::println("  赋值前:");
    std::println("    original = {}", original.data());
    std::println("    target   = {}", target.data());

    target = original;  // copy-and-swap: 强异常安全

    std::println("  赋值后 (copy-and-swap):");
    std::println("    original = {}", original.data());
    std::println("    target   = {}", target.data());
    // 即使赋值过程中拷贝抛异常，original 和 target 都保持不变

    // ── 示例: 基本保证 ──
    std::vector<int> nums{1, 2, 3};
    try {
        nums.push_back(42);  // 可能重新分配内存
        std::println("  push_back 成功, size={}", nums.size());
    } catch (const std::bad_alloc&) {
        // 即使分配失败，nums 仍然处于有效状态 (内容可能是旧的)
        std::println("  push_back 失败，但 nums 仍然可用, size={}", nums.size());
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: noexcept 移动为什么重要                                           ║
// ║                                                                             ║
// ║  std::vector 在重新分配内存时需要移动元素。                                ║
// ║  如果元素的移动构造函数是 noexcept，vector 使用移动 (高效)。              ║
// ║  如果移动不是 noexcept，vector 退化为拷贝 (安全但慢)。                    ║
// ║                                                                             ║
// ║  这是强异常安全保证的应用: 移动过程中的异常无法回滚已移动的元素，          ║
// ║  所以 vector 只信任 noexcept 的移动操作。                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part3_noexcept_move() {
    lesson::print_separator("Part 3: noexcept 移动的重要性");

    // 编译期检查
    std::println("  移动构造为 noexcept?");
    std::println("    std::string          : {}",
        std::is_nothrow_move_constructible_v<std::string>);
    std::println("    std::vector<int>     : {}",
        std::is_nothrow_move_constructible_v<std::vector<int>>);
    std::println("    YesNoexceptMove      : {}",
        std::is_nothrow_move_constructible_v<YesNoexceptMove>);

    // 使用 noexcept 移动的类型放入 vector
    std::vector<YesNoexceptMove> v;
    v.emplace_back("元素1");
    v.emplace_back("元素2");
    v.emplace_back("元素3");
    std::println("  vector<YesNoexceptMove> size = {}", v.size());
    // 当 vector 重分配时，因为移动是 noexcept，使用移动 (高效)
    // 如果移动不是 noexcept，vector 会退化为拷贝，性能大打折扣
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: [[nodiscard]] — 不允许忽略返回值                                  ║
// ║                                                                             ║
// ║  C++17 引入的属性，标记函数返回值不应被丢弃。                              ║
// ║  如果忽略，编译器发出警告。                                                ║
// ║                                                                             ║
// ║  典型使用场景:                                                              ║
// ║  - 返回错误码的函数 (如 errno 风格)                                        ║
// ║  - 工厂函数 (忽视返回值就没有意义)                                         ║
// ║  - 纯计算函数 (如 empty() — 调用而不检查结果是 bug)                        ║
// ║  - 获取锁、分配资源等不能泄露的操作                                        ║
// ║                                                                             ║
// ║  C++20 [[nodiscard]] 可以带消息字符串:                                     ║
// ║  [[nodiscard("忽略此返回值可能导致资源泄漏")]]                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part4_nodiscard() {
    lesson::print_separator("Part 4: [[nodiscard]]");

    // 正确使用: 接收返回值
    int val = computeImportantValue();
    std::println("  computeImportantValue() = {}", val);

    // 正确使用: 检查错误码
    FileResult r = openFile("data.txt");
    if (r == FileResult::Success) {
        std::println("  文件操作成功");
    }

    // 错误示范 (注释掉以免编译警告):
    // computeImportantValue();       // 警告: 忽略了 [[nodiscard]] 返回值
    // openFile("config.txt");        // 警告: 忽略了 [[nodiscard]] 返回值

    // 如果真的想忽略，用 std::ignore (C++26) 或强转为 void:
    std::ignore = computeImportantValue();  // C++26 风格
    // 或旧式写法:
    // (void)computeImportantValue();

    // allocateBuffer 的演示 — 记得释放!
    void* buf = allocateBuffer(1024);
    std::println("  分配的缓冲区地址: {}", buf);
    std::free(buf);  // 不释放就是泄漏!
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                  ║
// ║                                                                             ║
// ║  1. 到处标记 noexcept "以防万一"                                          ║
// ║     → 限制了未来重构时添加可能抛异常的操作                                 ║
// ║     正确方式: 只在明确不应抛出异常的函数上标记 noexcept                    ║
// ║                                                                             ║
// ║  2. 在 noexcept 函数中调用可能抛异常的函数                                 ║
// ║     → 如果调用抛出异常 → std::terminate()                                  ║
// ║     正确方式: 确认所有被调用函数也是 noexcept，或用 try-catch 吞掉异常    ║
// ║                                                                             ║
// ║  3. 移动构造不标记 noexcept                                               ║
// ║     → std::vector 重分配时退化为拷贝，性能下降                             ║
// ║     正确方式: 移动构造和移动赋值都标记 noexcept                            ║
// ║                                                                             ║
// ║  4. 在 noexcept 移动构造中分配内存却没有 try-catch                        ║
// ║     → bad_alloc → terminate，违背了 noexcept 承诺                          ║
// ║     正确方式: noexcept 移动不应做任何可能抛异常的操作                      ║
// ║                                                                             ║
// ║  5. 忘记在返回 error_code 的函数上加 [[nodiscard]]                        ║
// ║     → 调用者可能忽略错误导致难以调试的 bug                                ║
// ║     正确方式: 所有返回错误码/状态值的函数都加 [[nodiscard]]               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                      ║
// ║                                                                             ║
// ║  1. 定义一个类，实现 copy-and-swap 的强异常安全赋值                        ║
// ║     验证: 如果拷贝构造抛出异常，原对象不受影响                             ║
// ║                                                                             ║
// ║  2. 创建两个 vector: 一个存 noexcept-movable 类型，一个存非 noexcept       ║
// ║     分别插入大量元素，对比性能 (关注内存重分配次数)                        ║
// ║                                                                             ║
// ║  3. 写一个 [[nodiscard]] 函数返回 std::error_code，验证编译器确实会警告   ║
// ║     然后用 std::ignore 或 (void) 消除警告                                 ║
// ║                                                                             ║
// ║  4. 思考: 析构函数默认 noexcept，如果析构中必须调用可能失败的函数         ║
// ║     (如关闭网络连接)，应该怎么处理？                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

int main() {
    lesson::print_header("02_noexcept_and_contracts — noexcept 与契约");

    part1_noexcept_specifier();
    part2_exception_safety();
    part3_noexcept_move();
    part4_nodiscard();

    std::println("\n✅ noexcept 与契约学习完成!");
    return 0;
}

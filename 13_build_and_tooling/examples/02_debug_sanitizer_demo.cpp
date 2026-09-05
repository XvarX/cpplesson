// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  13_build_and_tooling — examples/debug_sanitizer_demo.cpp                      ║
// ║  学习目标:                                                                    ║
// ║    1. 掌握 assert 和 static_assert 的用法和区别                               ║
// ║    2. 理解 AddressSanitizer (ASan) 如何检测内存错误                           ║
// ║    3. 理解 UndefinedBehaviorSanitizer (UBSan) 检测哪些未定义行为               ║
// ║    4. 了解 Valgrind 的概念和与 ASan 的对比                                    ║
// ║    5. 理解 _DEBUG / NDEBUG 宏与断言的关系                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <iostream>
#include <print>
#include <string>
#include <vector>
#include <array>
#include <cassert>
#include <memory>

#include "shared/lesson_utils.hpp"
#include "build_tooling/debug_log.hpp"
#include "build_tooling/platform_detect.hpp"

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: assert — 运行时断言，捕获"不该发生的事"
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是 assert?
//   assert(表达式): 表达式为 false 时程序立即终止，并打印文件名和行号。
//   来自 <cassert> (C 的 <assert.h>)。
//
// 为什么需要 assert?
//   - 在开发阶段尽早暴露 bug (fail fast, fail loud)
//   - 文档化前置条件和不变式 (代码即文档)
//   - 比注释强: 注释不会在你违反它时"喊停"
//
// assert 的致命特性:
//   NDEBUG 宏定义时 (Release 模式)，assert 被完全移除!
//   → assert 中不能放"有副作用"的代码 (如 assert(connect() == 0))
//   → 只检查逻辑，不检查用户输入、网络连接、文件是否存在
//
// 规则:
//   assert 检查的是"程序员保证正确"的事 → NDEBUG 下可移除
//   运行时错误 (用户输入、IO 失败) → 必须用 if + throw / std::expected

// ── static_assert: 编译期断言 ─────────────────────────────────────────────────
// static_assert(常量表达式, "错误信息") — C++11 起
// static_assert(常量表达式) — C++17 起 (信息可选)
// 编译期检查，不通过直接编译失败，零运行时开销

void demo_assert() {
    lesson::print_subtitle("Part 1: assert 与 static_assert");

    // ── 运行时 assert ──────────────────────────────────────────────────────
    std::println("  测试 assert (运行时检查):");

    // 前置条件检查: 除数为 0 是逻辑错误
    auto safe_divide = [](int a, int b) -> int {
        assert(b != 0 && "除数不能为 0!");  // C++ 对 assert 的扩展: 可以加字符串
        return a / b;
    };

    std::println("    safe_divide(10, 2) = {}", safe_divide(10, 2));

    // 后置条件检查
    auto factorial = [](int n) -> int {
        assert(n >= 0 && "阶乘参数必须 >= 0");
        int result = 1;
        for (int i = 2; i <= n; ++i) result *= i;
        assert(result > 0 && "结果溢出!");
        return result;
    };

    std::println("    factorial(5) = {}", factorial(5));
    std::println("  ✓ assert 帮你验证前置/后置条件");

    // ── 编译期 static_assert ───────────────────────────────────────────────
    std::println("");
    std::println("  测试 static_assert (编译期检查):");

    // 检查类型大小 (很多协议和文件格式依赖精确的大小)
    static_assert(sizeof(int) >= 4, "int 必须至少 4 字节");
    static_assert(sizeof(char) == 1, "char 必须是 1 字节");

    // 检查模板参数
    static_assert(sizeof(void*) == 8, "本程序假设 64 位平台");
    std::println("  ✓ 平台: 64 位 (void* == {} 字节)", sizeof(void*));

    // 检查编译器特性
    static_assert(__cplusplus >= 202302L, "需要 C++23 编译器");
    std::println("  ✓ C++ 标准: C++23");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: AddressSanitizer (ASan) — 自动检测内存错误
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是 AddressSanitizer?
//   GCC/Clang 内置的内存错误检测工具。
//   编译时加 -fsanitize=address，运行时自动检测:
//     - 堆缓冲区溢出 (heap buffer overflow)
//     - 栈缓冲区溢出 (stack buffer overflow)
//     - 使用已释放的内存 (use-after-free)
//     - 重复释放 (double-free)
//     - 内存泄漏 (需额外加 detect_leaks=1)
//
// 为什么需要 ASan?
//   内存错误是最难调试的 bug 之一:
//     - 症状和根因可能相距甚远 (段错误发生在很久之后)
//     - 行为不确定 (有时正常有时崩溃)
//     - 普通 debugger 不容易复现
//   ASan 在错误发生的第一时间精确报告位置和原因
//
// 编译方式:
//   g++ -fsanitize=address -g -O0 main.cpp -o main
//   必须加 -g (调试符号)，建议 -O0 (不优化，便于追踪)
//
// ASan 原理 (简要):
//   把内存划分为主内存和"影子内存"(shadow memory)
//   每次内存访问都检查影子内存，判断是否合法
//   开销: ~2x 慢, ~2-3x 内存
//
// 注: 以下代码演示"可能触发"内存错误的情境
//     但实际触发错误会终止进程，所以这里仅做模式展示
//     运行时请分别编译、分别运行

void demo_asan_concepts() {
    lesson::print_subtitle("Part 2: AddressSanitizer (ASan) 内存错误检测");

    std::println("  ASan 可检测的 6 类内存错误:");
    std::println("  ┌─────────────────────────┬─────────────────────────────────┐");
    std::println("  │ 错误类型                │ 示例                            │");
    std::println("  ├─────────────────────────┼─────────────────────────────────┤");
    std::println("  │ heap-buffer-overflow    │ int* p = new int[4]; p[4] = 0;  │");
    std::println("  │ stack-buffer-overflow   │ int arr[4]; arr[4] = 0;         │");
    std::println("  │ heap-use-after-free     │ delete p; *p = 42;              │");
    std::println("  │ double-free             │ delete p; delete p;             │");
    std::println("  │ stack-use-after-return  │ 返回局部变量的指针              │");
    std::println("  │ memory-leak             │ new 后忘记 delete               │");
    std::println("  └─────────────────────────┴─────────────────────────────────┘");
    std::println("");

    // ── 安全演示: 模拟 ASan 的检测行为 ────────────────────────────────────
    // 不会真正触发 ASan, 而是展示正确的写法 vs 错误的写法

    std::println("  示例 1: 堆缓冲区溢出 (heap-buffer-overflow)");
    std::println("    错误: int* p = new int[4]; p[4] = 10;  // 越界!");
    {
        // 正确写法
        std::vector<int> safe(4);
        // safe[4] = 10; // vector::operator[] 不做边界检查
        safe.at(0) = 10;  // .at() 会抛异常, 推荐在不确定时使用
        std::println("    ✓ 安全: std::vector<int>(4), 用 .at() 访问");
        std::println("    ⚠ 如果用 new[] + 越界: ASan 会立即报告越界位置和调用栈");
    }

    std::println("");
    std::println("  示例 2: 使用已释放内存 (heap-use-after-free)");
    {
        auto safe_ptr = std::make_unique<int>(42);
        std::println("    ✓ 安全: unique_ptr 自动管理, val={}", *safe_ptr);
        // 错误模式: int* p = new int(42); delete p; *p = 0;
        std::println("    ⚠ 如果 delete 后仍用: ASan 会报告 use-after-free");
    }

    std::println("");
    std::println("  编译并测试 ASan:");
    std::println("    g++ -fsanitize=address -g -O0 buggy.cpp -o buggy");
    std::println("    ./buggy  # ASan 会打印彩色报告, 精确到行号");
    std::println("");
    std::println("  常用 ASan 选项 (环境变量):");
    std::println("    ASAN_OPTIONS=detect_leaks=1           启用泄漏检测");
    std::println("    ASAN_OPTIONS=halt_on_error=0          发现错误后继续运行");
    std::println("    ASAN_OPTIONS=log_path=asan.log        报告输出到文件");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: UndefinedBehaviorSanitizer (UBSan) — 检测未定义行为
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是 UBSan?
//   检测 C++ 程序中"未定义行为" (Undefined Behavior, UB) 的 Sanitizer。
//   UB 是指 C++ 标准没有规定程序行为的代码 — 编译器可以生成任何结果。
//
// 为什么 UB 很危险?
//   - 在你的机器上"碰巧正常"，在别人的机器上崩溃
//   - 编译器假设 UB 不会发生，基于此做激进的优化 → 优化后的行为可能完全异常
//   - 安全漏洞的常见来源 (如有符号整数溢出用于绕过边界检查)
//
// UBSan 检测的主要 UB:
//   - 有符号整数溢出: int_max + 1
//   - 除以 0: x / 0
//   - 空指针解引用: *nullptr
//   - 非法类型转换 (如错误的 dynamic_cast, 不当的 reinterpret_cast)
//   - 未对齐的指针访问
//   - 移位溢出: 1 << 31 (对 32-bit int) 是 UB
//   - 返回局部变量的引用/指针
//
// 编译方式:
//   g++ -fsanitize=undefined -g main.cpp -o main
//   也可以和 ASan 组合: -fsanitize=address,undefined

void demo_ubsan() {
    lesson::print_subtitle("Part 3: UndefinedBehaviorSanitizer (UBSan)");

    std::println("  UBSan 可检测的常见未定义行为:");
    std::println("  ┌────────────────────────────┬───────────────────────────────────┐");
    std::println("  │ 未定义行为                 │ 示例                              │");
    std::println("  ├────────────────────────────┼───────────────────────────────────┤");
    std::println("  │ signed-integer-overflow    │ INT_MAX + 1  (有符号溢出 = UB)    │");
    std::println("  │ division-by-zero           │ int x = 1 / 0;                   │");
    std::println("  │ null-pointer-dereference   │ int* p = nullptr; *p = 42;       │");
    std::println("  │ shift-out-of-bounds        │ 1 << 31  (int 为 32 位 = UB)     │");
    std::println("  │ float-cast-overflow        │ (int)1e100  (超出 int 范围)      │");
    std::println("  │ misaligned-pointer-access  │ 非对齐地址解引用                 │");
    std::println("  │ vptr (虚表指针) 错误       │ 对未构造/已析构对象调用虚函数    │");
    std::println("  └────────────────────────────┴───────────────────────────────────┘");

    std::println("");
    std::println("  ⚠ 重点: 有符号整数溢出是 UB，无符号整数回绕是定义好的!");
    std::println("    unsigned int u = UINT_MAX; u += 1;  // OK, u = 0 (回绕)");
    std::println("    int s = INT_MAX; s += 1;            // UB! ASan/UBSan 会报告");
    std::println("");

    // ── 安全演示 ──────────────────────────────────────────────────────────
    // 展示如何安全地处理可能溢出的整数运算

    std::println("  安全实践:");
    {
        // 检查加法是否溢出 (GCC/Clang 内置函数, 不触发 UB)
        int a = 2'000'000'000;
        int b = 2'000'000'000;
        int result;

        // __builtin_add_overflow (GCC/Clang) 安全检测溢出
        #if defined(__GNUC__) || defined(__clang__)
        if (__builtin_add_overflow(a, b, &result)) {
            std::println("    ⚠ {} + {} 会溢出 int 范围!", a, b);
        } else {
            std::println("    {} + {} = {} (安全)", a, b, result);
        }
        #else
        std::println("    (本编译器不支持 __builtin_add_overflow)");
        #endif
    }

    std::println("");
    std::println("  编译并测试 UBSan:");
    std::println("    g++ -fsanitize=undefined -g -O0 main.cpp -o main");
    std::println("    # 或组合使用:");
    std::println("    g++ -fsanitize=address,undefined -g -O0 main.cpp -o main");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: Valgrind — 不需要重新编译的内存分析工具
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是 Valgrind?
//   一个 Linux 上的动态分析工具套件 (在虚拟机中运行你的程序)。
//   最常用的工具是 Memcheck (内存检查器)。
//
// 与 ASan 的对比:
//   ┌────────────┬───────────────────┬──────────────────────┐
//   │ 特性       │ ASan              │ Valgrind (Memcheck)  │
//   ├────────────┼───────────────────┼──────────────────────┤
//   │ 使用方式   │ 重新编译 (插桩)    │ 无需重新编译          │
//   │ 运行速度   │ ~2x 慢            │ ~10-30x 慢           │
//   │ 内存开销   │ ~2-3x             │ 较大                 │
//   │ 检测精度   │ 高 (编译器级插桩) │ 中等 (二进制插桩)     │
//   │ 栈越界     │ 可检测            │ 不能检测              │
//   │ 未初始化读 │ 部分支持          │ 精准检测              │
//   │ 平台支持   │ GCC/Clang 多平台  │ Linux/macOS (主要)    │
//   └────────────┴───────────────────┴──────────────────────┘
//
// 使用场景:
//   ASan:   开发阶段的日常调试 (快, 精准, 但需要重编译)
//   Valgrind: 排查已编译的二进制, CI 中的补充检查
//             (特别是需要 Memcheck 检测"读取未初始化变量"时)

void demo_valgrind() {
    lesson::print_subtitle("Part 4: Valgrind 概念与 ASan 对比");

    std::println("  Valgrind 是 Linux 上的动态分析工具, 无需重新编译。");
    std::println("  核心工具: Memcheck — 检测内存泄漏、非法访问、未初始化值");
    std::println("");
    std::println("  使用方式:");
    std::println("    valgrind --leak-check=full ./my_program");
    std::println("    valgrind --tool=memcheck --track-origins=yes ./my_program");
    std::println("");
    std::println("  选择指南:");
    std::println("    ┌────────────────────────────────────────────────────┐");
    std::println("    │ 日常开发, 能重编译  → ASan (快, 精准)              │");
    std::println("    │ 排查第三方二进制    → Valgrind (无需重编译)        │");
    std::println("    │ Windows 开发        → ASan (Valgrind 不支持 Win)  │");
    std::println("    │ 检测未初始化变量    → Valgrind Memcheck (更强)     │");
    std::println("    │ CI 流水线           → ASan + UBSan + Valgrind 都用 │");
    std::println("    └────────────────────────────────────────────────────┘");
    std::println("");
    std::println("  ⚠ Valgrind 不支持 Windows (需要 WSL2 或 Docker)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: _DEBUG / NDEBUG 宏 — 控制调试代码
// ═══════════════════════════════════════════════════════════════════════════════
// _DEBUG (MSVC) 和 NDEBUG (标准) 是两个层面:
//
//   NDEBUG (No Debug):
//     - C 标准定义的宏
//     - 定义后: assert() 失效
//     - CMake Release 模式自动定义 (CMAKE_BUILD_TYPE=Release)
//
//   _DEBUG:
//     - MSVC 编译器约定 (非标准, 但被广泛使用)
//     - 定义后: 调试版本的 CRT (C 运行时) 被链接
//     - assert 行为: MSVC 中 assert 依赖 NDEBUG (不是 _DEBUG)
//
//   最佳实践:
//     - 用 NDEBUG 控制 assert
//     - 用自定义宏 (如 ENABLE_DEBUG_LOG) 控制调试日志
//     - 不要在 assert 里放有副作用的代码!
//
// 注意: DEBUG_LOG, LOG_ERROR, LOG_WARN, LOG_INFO 定义在 debug_log.hpp 中
//       PLATFORM, IS_DEBUG 定义在 platform_detect.hpp 中

void demo_debug_macros() {
    lesson::print_subtitle("Part 5: _DEBUG / NDEBUG 宏与调试日志");

    std::println("  宏状态:");
#ifdef NDEBUG
    std::println("    NDEBUG:  已定义 (Release 模式)");
    std::println("    assert:  已禁用 (无运行时开销)");
#else
    std::println("    NDEBUG:  未定义 (Debug 模式)");
    std::println("    assert:  生效中");
#endif
    std::println("    LOG_LEVEL: {}", LOG_LEVEL);

    std::println("");
    std::println("  测试分级日志 (当前 LOG_LEVEL={}):", LOG_LEVEL);
    LOG_ERROR("这是一条错误消息 (始终显示)");
    LOG_WARN("这是一条警告消息");
    LOG_INFO("这是一条信息消息");
    DEBUG_LOG("这是一条调试日志, 当前函数: {}", __func__);

    // 演示: 构建类型对日志的影响
    std::println("");
    std::println("  CMake 控制构建类型:");
    std::println("    cmake -DCMAKE_BUILD_TYPE=Debug ..    → 未定义 NDEBUG");
    std::println("    cmake -DCMAKE_BUILD_TYPE=Release ..  → 定义 NDEBUG");
    std::println("    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. → 定义 NDEBUG + 调试符号");

    // ── assert 副作用陷阱演示 ──────────────────────────────────────────────
    std::println("");
    std::println("  ⚠ assert 副作用陷阱:");
    std::println("    错误写法: assert(close(fd) == 0);");
    std::println("              // Release 下 close() 不会被调用! fd 泄漏!");
    std::println("    正确写法: int ret = close(fd); assert(ret == 0);");
    std::println("    原则: assert 里只能放纯检查, 不能放有副作用的表达式");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. assert 中有副作用:
//    assert(database.connect())  // Release 下 connect() 被移除!
//    正确: bool ok = database.connect(); assert(ok);
//
// 2. 混淆 assert 和异常处理:
//    assert 检查程序员错误 (逻辑 bug)
//    异常/std::expected 处理运行时错误 (用户输入/IO/网络)
//    不可用 assert 检查用户输入!
//
// 3. ASan 没开 -g 或过度优化:
//    -fsanitize=address 必须配合 -g (调试符号) 才有行号
//    建议 -O0 或 -Og，-O2 可能因优化改变执行顺序
//
// 4. 多个 Sanitizer 冲突:
//    ASan 和 ThreadSanitizer (TSan) 不能同时使用
//    ASan 和 UBSan 可以组合 (-fsanitize=address,undefined)
//
// 5. 依赖 _DEBUG 判断调试模式 (平台相关):
//    MSVC 用 _DEBUG, GCC/Clang 不定义它
//    跨平台代码用 NDEBUG (标准)
//
// 6. UBSan 发现的"无害"溢出:
//    "它一直能跑"不代表正确 — UB 可能在编译器升级后行为剧变
//    只要 UBSan 报告了, 必须修复!

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 写一个函数 int binary_search(const vector<int>& v, int target)
//    用 assert 验证: 输入数组已排序, 结果索引在有效范围内
// 2. 写一个会触发 heap-buffer-overflow 的程序
//    用 g++ -fsanitize=address 编译, 运行, 阅读 ASan 的报告
// 3. 写一个会触发有符号整数溢出的程序
//    用 g++ -fsanitize=undefined 编译, 观察 UBSan 的输出
// 4. 写一个 DEBUG_ONLY 宏: 在 NDEBUG 未定义时执行一段代码
//    如: DEBUG_ONLY( std::println("检查中..."); validate_state(); )
// 5. 如果用的 Linux/macOS, 用 valgrind 运行一个释放后使用的小程序
//    (Windows 用户在 WSL2 中尝试)

// ═══════════════════════════════════════════════════════════════════════════════

int main() {
    lesson::print_header("03 — 调试工具: assert / ASan / UBSan / Valgrind");

    demo_assert();
    demo_asan_concepts();
    demo_ubsan();
    demo_valgrind();
    demo_debug_macros();

    lesson::print_separator("完成: 现在你掌握了 C++ 的核心调试和检测工具");

    return 0;
}

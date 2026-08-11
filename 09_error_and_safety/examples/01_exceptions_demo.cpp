// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 09_error_and_safety — 错误处理与安全                                 ║
// ║  主题: 01_exceptions — 异常机制基础                                        ║
// ║  目标: 理解 throw/try/catch、标准异常层次、栈展开、catch(...)、异常安全     ║
// ║                                                                             ║
// ║  跟着敲: 逐行理解异常的核心概念                                              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <vector>
#include <stdexcept>
#include "error_safety/exceptions.hpp"
#include "shared/lesson_utils.hpp"

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: 异常的基本概念 — throw / try / catch                              ║
// ║                                                                             ║
// ║  异常(exception)是什么？                                                    ║
// ║  - 一种"非本地"错误报告机制：错误可以在远离调用点的地方被捕获处理           ║
// ║  - 替代方案是返回错误码 — 但这容易被忽略，且无法在构造函数中返回            ║
// ║                                                                             ║
// ║  为什么需要异常？                                                           ║
// ║  1. 构造函数无法返回值 → 只能用异常报告构造失败                             ║
// ║  2. 错误码可以被忽略 → 异常如果不被捕获会终止程序，强制关注                 ║
// ║  3. 将"正常逻辑"和"错误处理"分开，代码更清晰                               ║
// ║                                                                             ║
// ║  何时使用异常？                                                             ║
// ║  - 构造函数中遇到无法恢复的错误                                             ║
// ║  - 函数无法完成其约定的语义 (例如 read() 连不上服务器)                      ║
// ║  - 深层调用链中的错误需要传递给上层                                         ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part1_basic_exceptions() {
    lesson::print_separator("Part 1: 基本异常 — throw / try / catch");

    // try 块包裹可能抛出异常的代码
    try {
        std::println("  状态: 准备创建连接...");
        DatabaseConnection conn("bad:localhost");  // 这里会抛出异常
        // 如果抛出了异常，这行不会执行
        std::println("  连接已就绪: {}", conn.connectionString());
    }
    // catch 块按类型匹配，按声明顺序尝试
    catch (const std::invalid_argument& e) {
        // 捕获特定的异常类型
        std::println("  [捕获] invalid_argument: {}", e.what());
    }
    catch (const std::runtime_error& e) {
        // 我们的 "bad:" 连接串会触发这个分支
        std::println("  [捕获] runtime_error: {}", e.what());
    }
    catch (const std::exception& e) {
        // 兜底：捕获所有标准异常
        std::println("  [捕获] 其他标准异常: {}", e.what());
    }
    catch (...) {
        // 捕获所有类型（包括非 std::exception 派生的）
        std::println("  [捕获] 未知类型异常");
    }
    std::println("  程序继续执行，没有崩溃!\n");

    // ── 示例2: 成功的情况 ──
    try {
        DatabaseConnection goodConn("mysql://localhost:3306");
        std::println("  使用连接: {}", goodConn.connectionString());
    } catch (const std::exception& e) {
        std::println("  [失败] {}", e.what());
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: 标准异常层次                                                     ║
// ║                                                                             ║
// ║  std::exception              ← 所有标准异常的基类                          ║
// ║  ├─ std::logic_error         ← 逻辑错误（可在编译/编写时检查出来）          ║
// ║  │   ├─ std::invalid_argument  ← 无效参数                                  ║
// ║  │   ├─ std::domain_error      ← 定义域外值                                ║
// ║  │   ├─ std::length_error      ← 超过最大长度                              ║
// ║  │   └─ std::out_of_range      ← 越界访问                                  ║
// ║  ├─ std::runtime_error       ← 运行时错误（不可预知）                      ║
// ║  │   ├─ std::range_error       ← 范围错误                                  ║
// ║  │   ├─ std::overflow_error    ← 溢出                                      ║
// ║  │   └─ std::underflow_error   ← 下溢                                      ║
// ║  └─ std::bad_alloc           ← new 失败                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// 演示标准库自身如何抛出异常
void part2_standard_hierarchy() {
    lesson::print_separator("Part 2: 标准异常层次");

    // 1. std::out_of_range — vector::at() 越界时抛出
    std::vector<int> v{1, 2, 3};
    try {
        auto x = v.at(5);  // at() 有边界检查，[] 没有
        std::println("{}", x);
    } catch (const std::out_of_range& e) {
        std::println("  [out_of_range] {}", e.what());
    }

    // 2. std::invalid_argument — stoi 遇到非法字符时抛出
    try {
        int n = std::stoi("not_a_number");
        std::println("{}", n);
    } catch (const std::invalid_argument& e) {
        std::println("  [invalid_argument] {}", e.what());
    } catch (const std::out_of_range& e) {
        std::println("  [out_of_range] {}", e.what());
    }

    // 3. 通过基类引用统一捕获 — 多态异常处理
    try {
        std::string s;
        s.resize(s.max_size() + 1);  // 超出最大长度
    } catch (const std::exception& e) {
        // 基类引用可以捕获所有派生类异常
        std::println("  [统一捕获 std::exception] {}", e.what());
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: 栈展开 (Stack Unwinding) 与 RAII                                  ║
// ║                                                                             ║
// ║  当异常被抛出时，运行时从 throw 点开始向上查找 catch 块。                   ║
// ║  在查找过程中，所有离开作用域的局部对象的析构函数会被调用                   ║
// ║  这就是 RAII 如此重要的原因 — 资源自动被正确释放                            ║
// ║                                                                             ║
// ║  关键规则：                                                                 ║
// ║  - 析构函数不应抛出异常 (noexcept by default since C++11)                   ║
// ║  - 如果栈展开过程中析构函数又抛出一个异常 → std::terminate()               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part3_stack_unwinding() {
    lesson::print_separator("Part 3: 栈展开与 RAII");

    try {
        ScopeGuard sg("part3 最外层");
        std::println("  开始调用 deepFunction(1)...");
        deepFunction(1);
        std::println("  这行不会执行");
    } catch (const std::runtime_error& e) {
        // 注意：输出会显示所有 ScopeGuard 按构造的逆序析构
        std::println("  [捕获] {}", e.what());
    }
    std::println("  ✅ 所有局部资源已通过析构函数自动释放");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: catch(...) — 捕获一切异常                                         ║
// ║                                                                             ║
// ║  catch(...) 捕获任意类型的异常，但无法获取异常对象本身                      ║
// ║  典型用途：                                                                 ║
// ║  1. 记录日志后重新抛出 (rethrow)                                            ║
// ║  2. 作为最后的安全网防止异常逃离 main()                                     ║
// ║  3. 跨语言边界 (C 接口) 时转换所有异常                                      ║
// ║                                                                             ║
// ║  重新抛出：throw; (不带参数) 在 catch 块中保留原始异常类型和栈追踪          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part4_catch_all() {
    lesson::print_separator("Part 4: catch(...) 与重新抛出");

    // ── 示例1: 捕获一切，记录日志后重新抛出 ──
    auto safeCall = [](bool fail) {
        try {
            maybeThrow(fail);
            std::println("  调用成功");
        } catch (...) {
            // 不论什么异常，先记录日志
            std::println("  [catch(...)] 记录日志: 操作发生异常");
            throw;  // 重新抛出原始异常，保留类型信息
            // 注意: 不能写 throw e; — 因为 catch(...) 没有命名变量
        }
    };

    try {
        safeCall(true);
    } catch (const std::exception& e) {
        std::println("  [最终捕获] {}", e.what());
    }
    // 输出说明：catch(...) 先捕获并记录日志，然后用 throw; 原样重新抛出，
    // 外层的 catch(std::exception&) 再次匹配到原始 logic_error

    // ── 示例2: 清理后继续 ──
    try {
        maybeThrow(true);
    } catch (...) {
        std::println("  [catch(...)] 异常已吞掉，程序继续运行");
        // 没有 throw; — 异常在此结束
    }
    std::println("  程序正常运行到结尾\n");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                  ║
// ║                                                                             ║
// ║  1. 按值捕获异常 (catch std::exception e) → 对象切片，丢失派生信息         ║
// ║     正确方式: catch (const std::exception& e)                               ║
// ║                                                                             ║
// ║  2. 析构函数中抛出异常 → 栈展开时双重异常 → std::terminate()               ║
// ║     正确方式: 析构函数默认 noexcept，用 try-catch 吞掉异常                  ║
// ║                                                                             ║
// ║  3. catch 块顺序错误: 基类在派生类前面 → 派生类永远不会被匹配              ║
// ║     正确方式: 最具体类型在前，基类在最后                                   ║
// ║                                                                             ║
// ║  4. 在构造函数中抛出异常时，已构造的子对象不会自动析构                     ║
// ║     正确方式: 使用智能指针 (unique_ptr/shared_ptr) 管理资源                ║
// ║                                                                             ║
// ║  5. 滥用异常做流程控制 → 性能差，语义混乱                                  ║
// ║     正确方式: 异常只用于"异常"情况，正常流程用 if/else                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                      ║
// ║                                                                             ║
// ║  1. 修改 DatabaseConnection 类，添加一个文件描述符成员 (用 int fd 模拟)   ║
// ║     确保异常发生时 fd 被正确关闭 (提示: 用 RAII 包装 fd)                   ║
// ║                                                                             ║
// ║  2. 写一个函数 parsePositiveInt(const std::string& s)，                   ║
// ║     抛出 std::invalid_argument (非数字) 或 std::domain_error (负数)        ║
// ║     在 main 中测试并区分两种异常                                           ║
// ║                                                                             ║
// ║  3. 观察如果将 catch(std::exception&) 放在 catch(std::runtime_error&)     ║
// ║     前面会发生什么？用代码验证编译器是否发出警告                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

int main() {
    lesson::print_header("01_exceptions — 异常机制基础");

    part1_basic_exceptions();
    part2_standard_hierarchy();
    part3_stack_unwinding();
    part4_catch_all();

    std::println("\n✅ 异常基础学习完成!");
    return 0;
}

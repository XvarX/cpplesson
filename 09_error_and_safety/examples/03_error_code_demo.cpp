// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  模块: 09_error_and_safety — 错误处理与安全                                 ║
// ║  主题: 03_error_code — std::error_code 与 system_error                     ║
// ║  目标: 理解 std::error_code/error_condition/system_error、                 ║
// ║        自定义错误类别 error_category、何时选择 error_code 而非异常         ║
// ║                                                                             ║
// ║  跟着敲: 掌握现代 C++ 中无异常错误处理的推荐方式                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <string>
#include <system_error>
#include <fstream>
#include <filesystem>
#include <cerrno>
#include "error_safety/error_code.hpp"
#include "shared/lesson_utils.hpp"

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: std::error_code — 平台无关的错误值                                ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part1_error_code_basics() {
    lesson::print_separator("Part 1: std::error_code 基础");

    // ── 构造 error_code ──
    // 方式1: 从 std::errc 枚举
    std::error_code ec1 = std::make_error_code(std::errc::permission_denied);
    std::println("  ec1: value={}, category={}, message={}",
        ec1.value(), ec1.category().name(), ec1.message());

    // 方式2: 从 errno (POSIX/C 系统调用)
    std::error_code ec2{static_cast<int>(ENOENT),
                        std::generic_category()};
    std::println("  ec2 (ENOENT): value={}, message={}",
        ec2.value(), ec2.message());

    // 方式3: 成功 — 默认构造
    std::error_code ec3{};  // value == 0, 没有错误
    std::println("  ec3 (默认): value={}, 是否有错误={}",
        ec3.value(), static_cast<bool>(ec3));  // 隐式转为 bool

    // ── 使用 error_code ──
    std::string content;
    auto ec = readFileContent("不存在的文件.txt", content);
    if (ec) {  // 隐式转为 bool: true 表示有错误
        std::println("  ❌ 读取失败: {}", ec.message());
        // 可以精确判断错误类型
        if (ec == std::errc::no_such_file_or_directory) {
            std::println("     具体原因: 文件或目录不存在");
        }
    } else {
        std::println("  ✅ 文件内容: {}", content);
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: std::error_condition — 可移植的错误判断                           ║
// ║                                                                             ║
// ║  error_code vs error_condition:                                             ║
// ║  ┌──────────────────┬─────────────────────────┬───────────────────────────┐ ║
// ║  │ error_code       │ 平台相关，具体实现      │ 例: EACCES=13 on Linux   │ ║
// ║  │ error_condition  │ 平台无关，语义抽象      │ 例: permission_denied    │ ║
// ║  └──────────────────┴─────────────────────────┴───────────────────────────┘ ║
// ║                                                                             ║
// ║  跨平台代码应该用 error_condition 判断错误语义，而不是直接比对值。         ║
// ║  error_code 可以与 error_condition 直接比较 (通过等价性检查)。              ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part2_error_condition() {
    lesson::print_separator("Part 2: std::error_condition");

    // error_condition: 平台无关的错误"类别"
    std::error_condition cond = std::errc::permission_denied;

    // 直接构造 error_code (平台相关)
    std::error_code ec{EACCES, std::generic_category()};

    std::println("  error_code: value={}, message={}",
        ec.value(), ec.message());
    std::println("  error_condition: value={}, message={}",
        cond.value(), cond.message());

    // 关键: error_code 可以和 error_condition 直接比较!
    if (ec == cond) {
        std::println("  ✅ ec == cond (跨平台等价比较)");
    }

    // 实际用法: 在跨平台代码中判断语义
    ec = std::make_error_code(std::errc::no_such_file_or_directory);
    if (ec == std::errc::no_such_file_or_directory) {
        // 这是推荐的写法: error_code 与 std::errc 比较
        // std::errc 会被隐式转换为 error_condition
        std::println("  ✅ 检测到: 文件不存在 (跨平台安全)");
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: 自定义错误类别 (error_category)                                   ║
// ║                                                                             ║
// ║  当标准错误码不够用时，可以定义自己的错误类别。                            ║
// ║  需要继承 std::error_category 并实现 name() 和 message()。                 ║
// ║                                                                             ║
// ║  为什么需要自定义类别？                                                     ║
// ║  - 领域特定的错误 (如 HTTP 状态码、数据库错误码)                           ║
// ║  - 避免与系统错误码冲突                                                    ║
// ║  - 提供更友好的错误消息                                                    ║
// ║                                                                             ║
// ║  注意: error_category 必须是单例式 — 通常返回静态局部变量的引用           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part3_custom_error_category() {
    lesson::print_separator("Part 3: 自定义错误类别");

    // 使用自定义错误码
    std::error_code gec = make_error_code(GameError::AssetNotFound);
    std::println("  自定义错误码:");
    std::println("    value    = {}", gec.value());
    std::println("    category = {}", gec.category().name());
    std::println("    message  = {}", gec.message());

    // 还可以像这样 (因为添加了 is_error_code_enum 特化):
    std::error_code gec2 = GameError::ShaderCompilationFailed;
    std::println("    shader error: {}", gec2.message());

    // 检查是否成功
    if (gec) {
        std::println("  ❌ 发生游戏错误: {}", gec.message());
    }

    auto success = make_error_code(GameError::Success);
    if (!success) {  // value == 0 → false
        std::println("  ✅ 操作成功 (GameError::Success)");
    }

    // ── 总结: error_category 的三个核心虚函数 ──
    // name()     → 返回类别的唯一标识字符串 (用于区分不同类别)
    // message()  → 将错误码值转为人类可读的消息
    // equivalent() → (可选) 比较 error_code 和 error_condition 是否等价
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: std::system_error — 将 error_code 当作异常抛出                    ║
// ║                                                                             ║
// ║  system_error 继承自 std::runtime_error，携带一个 error_code。             ║
// ║  它在"需要异常的行为 (栈展开) + error_code 的精确语义"之间的桥梁。        ║
// ║                                                                             ║
// ║  典型场景:                                                                  ║
// ║  - 使用 error_code 的底层库被上层异常风格的代码调用                        ║
// ║  - <filesystem> 的函数可以抛出 filesystem_error (继承自 system_error)     ║
// ║  - 想要用异常终止，但保留精确的错误码信息                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

void part4_system_error() {
    lesson::print_separator("Part 4: std::system_error");

    // ── 示例1: 主动抛出 system_error ──
    try {
        std::error_code ec = std::make_error_code(std::errc::broken_pipe);
        throw std::system_error{ec, "向已关闭的管道写入数据"};
    } catch (const std::system_error& e) {
        std::println("  [system_error] what(): {}", e.what());
        // system_error 携带了原始的 error_code
        const auto& ec = e.code();
        std::println("    error_code: value={}, category={}, message={}",
            ec.value(), ec.category().name(), ec.message());
    }

    // ── 示例2: <filesystem> 的异常也是 system_error ──
    try {
        // 检查一个不存在的路径
        namespace fs = std::filesystem;
        fs::path p{"no_such_directory/no_such_file"};
        // filesystem::file_size 对不存在的文件抛出 filesystem_error
        (void)fs::file_size(p);
    } catch (const std::filesystem::filesystem_error& e) {
        // filesystem_error 继承自 system_error
        std::println("  [filesystem_error] what(): {}", e.what());
        std::println("    path1: {}", e.path1().string());
        std::println("    code.value(): {}", e.code().value());
    }

    // ── 示例3: 用错误码判断的推荐模式 ──
    // 对于预期中的失败，直接返回 error_code 比抛异常更高效:
    std::error_code ec;
    // 许多 filesystem 函数有 noexcept 重载接受 error_code 引用
    auto size = std::filesystem::file_size("确实不存在的文件.txt", ec);
    if (ec) {
        std::println("  [error_code 模式] 获取文件大小失败: {}", ec.message());
        std::println("    返回的 size 值 (忽略): {}", size);
    }
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                  ║
// ║                                                                             ║
// ║  1. 直接用 error_code.value() 与魔法数字比较                               ║
// ║     → error_code e; if (e.value() == 2) ...  ← 不可移植                    ║
// ║     正确方式: if (e == std::errc::no_such_file_or_directory)               ║
// ║                                                                             ║
// ║  2. error_category 没有做成单例                                            ║
// ║     → error_code 的比较要求类别对象地址相同                                ║
// ║     正确方式: 使用静态局部变量 (Meyers Singleton)                          ║
// ║                                                                             ║
// ║  3. 忘记为自定义 enum 特化 is_error_code_enum                             ║
// ║     → 无法使用隐式转换，编译报错                                           ║
// ║     正确方式: template<> struct is_error_code_enum<MyEnum> : true_type {}; ║
// ║                                                                             ║
// ║  4. 混淆 error_code 和 error_condition                                    ║
// ║     → error_code 是实现的 (平台相关), error_condition 是语义的 (可移植)   ║
// ║     正确方式: 跨平台判断用 error_condition；返回给调用者用 error_code      ║
// ║                                                                             ║
// ║  5. 在性能敏感路径中使用异常 + error_code 的混合                          ║
// ║     → 不必要的 overhead                                                    ║
// ║     正确方式: 预期失败 → error_code; 意外错误 → 异常                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                      ║
// ║                                                                             ║
// ║  1. 写一个函数 openConfig(path)，失败时返回 error_code，成功时在 out      ║
// ║     参数中填充配置内容。区分"文件不存在"和"权限不足"两种错误。            ║
// ║                                                                             ║
// ║  2. 定义一个自定义 error_category "NetworkError"，枚举:                     ║
// ║     Timeout, ConnectionRefused, DNSFailure, SSLHandshakeFailed              ║
// ║     实现 name() 和 message()，并在 main 中测试                             ║
// ║                                                                             ║
// ║  3. 比较以下两种写法在不同场景下的优劣:                                    ║
// ║     写法A: throw std::system_error{ec, "msg"};                              ║
// ║     写法B: return ec;                                                       ║
// ║     思考: 何时用 A? 何时用 B?                                              ║
// ║                                                                             ║
// ║  4. 使用 std::filesystem 的 error_code 重载版本重写一个文件复制函数       ║
// ║     copy_file_safe(src, dst, ec)，正确处理所有错误情况                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

int main() {
    lesson::print_header("03_error_code — std::error_code 与 system_error");

    part1_error_code_basics();
    part2_error_condition();
    part3_custom_error_category();
    part4_system_error();

    std::println("\n✅ error_code 学习完成!");
    return 0;
}

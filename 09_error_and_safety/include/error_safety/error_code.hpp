#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  error_safety/error_code.hpp — std::error_code 与 system_error               ║
// ║  可复用的类型: GameErrorCategory (自定义错误类别), GameError 枚举           ║
// ║  可复用的函数: readFileContent, make_error_code                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <string>
#include <string_view>
#include <system_error>

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  readFileContent — 返回 error_code 的文件读取函数                          ║
// ║                                                                             ║
// ║  std::error_code 是什么？                                                   ║
// ║  - 一个轻量级的值类型 (类似 int + 指向 error_category 的指针)              ║
// ║  - 封装了 (错误值, 错误类别) 这对信息                                       ║
// ║  - 不携带任何字符串，只存储整数错误码和类别引用                            ║
// ║  - 拷贝开销极小 (两个指针大小)                                             ║
// ║                                                                             ║
// ║  为什么需要 error_code？                                                    ║
// ║  1. 异常有运行时开销，不适合"预期中的失败" (如文件不存在)                  ║
// ║  2. 跨模块/跨语言边界时异常难以传递                                        ║
// ║  3. errno (C 风格) 不安全 — 可能被其他系统调用覆盖                         ║
// ║  4. 需要可组合、可比较的错误表示                                           ║
// ║                                                                             ║
// ║  何时用 error_code vs 异常？                                                ║
// ║  - error_code: 预期中的、可恢复的、高频的错误 (文件 I/O, 网络)            ║
// ║  - 异常: 意外的、难以恢复的、构造失败等                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
[[nodiscard]] std::error_code readFileContent(std::string_view path,
                                               std::string& outContent);

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  GameError — 自定义游戏引擎的错误码枚举                                     ║
// ║                                                                             ║
// ║  当标准错误码不够用时，可以定义自己的错误类别。                            ║
// ║  需要继承 std::error_category 并实现 name() 和 message()。                 ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
enum class GameError {
    Success = 0,
    OutOfMemory = 1,
    AssetNotFound = 2,
    ShaderCompilationFailed = 3,
    PhysicsInitFailed = 4,
};

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  GameErrorCategory — 自定义 error_category (单例)                          ║
// ║                                                                             ║
// ║  error_category 的三个核心虚函数：                                          ║
// ║  - name()      → 返回类别的唯一标识字符串 (用于区分不同类别)               ║
// ║  - message()   → 将错误码值转为人类可读的消息                              ║
// ║  - equivalent()→ (可选) 比较 error_code 和 error_condition 是否等价        ║
// ║                                                                             ║
// ║  注意: error_category 必须是单例式 — 通常返回静态局部变量的引用           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
class GameErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override;
    std::string message(int ev) const override;

    // 单例访问 (Meyers Singleton)
    static const GameErrorCategory& instance();
};

// 便捷函数: 从 GameError 构造 error_code
std::error_code make_error_code(GameError e);

// 在 std 命名空间中启用 make_error_code 特化 (ADL 支持)
// 不特化这个模板会导致无法使用隐式转换: std::error_code ec = GameError::AssetNotFound;
namespace std {
    template<>
    struct is_error_code_enum<GameError> : true_type {};
}

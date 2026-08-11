// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  实现: error_safety/error_code.hpp                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "error_safety/error_code.hpp"
#include <print>
#include <fstream>
#include <iterator>

// ── readFileContent ────────────────────────────────────────────────────────────

[[nodiscard]] std::error_code readFileContent(std::string_view path,
                                               std::string& outContent) {
    std::ifstream file{std::string{path}};  // C++23: ifstream 接受 string
    if (!file.is_open()) {
        // 构造一个代表"文件未找到"的 error_code
        return std::make_error_code(std::errc::no_such_file_or_directory);
    }

    try {
        outContent.assign(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>());
    } catch (const std::bad_alloc&) {
        return std::make_error_code(std::errc::not_enough_memory);
    }

    if (file.bad()) {
        return std::make_error_code(std::errc::io_error);
    }

    return {};  // 默认构造的 error_code 表示"成功" (值为 0)
}

// ── GameErrorCategory ──────────────────────────────────────────────────────────

const char* GameErrorCategory::name() const noexcept {
    return "game-engine";
}

std::string GameErrorCategory::message(int ev) const {
    switch (static_cast<GameError>(ev)) {
        case GameError::Success:               return "成功";
        case GameError::OutOfMemory:           return "内存不足: 无法分配游戏资源";
        case GameError::AssetNotFound:         return "资源未找到: 请检查资源路径";
        case GameError::ShaderCompilationFailed: return "着色器编译失败";
        case GameError::PhysicsInitFailed:     return "物理引擎初始化失败";
        default:                               return "未知游戏错误";
    }
}

const GameErrorCategory& GameErrorCategory::instance() {
    static GameErrorCategory cat;
    return cat;
}

// ── make_error_code (GameError) ─────────────────────────────────────────────────

std::error_code make_error_code(GameError e) {
    return {static_cast<int>(e), GameErrorCategory::instance()};
}

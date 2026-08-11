// raii 模块 — File 类中非内联方法的实现
#include "classes/raii.hpp"
#include <cstdio>
#include <cstring>
#include <cerrno>

// ═══════════════════════════════════════════════════════════════════════════════
// File 类的非内联实现
// ═══════════════════════════════════════════════════════════════════════════════

File::File(const char* path, const char* mode) {
    file_ = std::fopen(path, mode);
    if (!file_) {
        std::println("❌ 无法打开文件: {} (errno: {})", path, std::strerror(errno));
    }
}

File::~File() {
    if (file_) {
        std::fclose(file_);
        std::println("📁 文件已自动关闭");
    }
}

std::string File::read_all() {
    if (!file_) return "";
    std::string content;
    char buf[256];
    while (std::fgets(buf, sizeof(buf), file_)) {
        content += buf;
    }
    return content;
}

void File::write(std::string_view data) {
    if (file_) std::fputs(data.data(), file_);
}

void File::close() {
    if (file_) { std::fclose(file_); file_ = nullptr; }
}

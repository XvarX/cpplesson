// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  binary_io.cpp — 二进制文件 I/O 实现                                         ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "streams/binary_io.hpp"
#include <iostream>  // std::cerr

namespace streams {

bool write_player(const std::string& path, const Player& p) {
    std::ofstream fout(path, std::ios::binary);
    if (!fout) {
        std::println(std::cerr, "[binary_io] 错误: 无法创建文件 {}", path);
        return false;
    }
    // reinterpret_cast<const char*> 是二进制 I/O 的标准用法
    // 将 Player 的内存表示视为原始字节序列
    fout.write(reinterpret_cast<const char*>(&p), sizeof(p));
    return static_cast<bool>(fout);
}

bool read_player(const std::string& path, Player& p) {
    std::ifstream fin(path, std::ios::binary);
    if (!fin) {
        std::println(std::cerr, "[binary_io] 错误: 无法打开文件 {}", path);
        return false;
    }
    fin.read(reinterpret_cast<char*>(&p), sizeof(p));
    // gcount() 返回上次非格式化输入操作实际读取的字节数
    return fin.gcount() == sizeof(Player);
}

bool validate_player(const Player& a, const Player& b) {
    return (a.id == b.id)
        && (a.score == b.score)
        && (std::strcmp(a.name, b.name) == 0);
}

void print_player(const Player& p, std::string_view label) {
    if (!label.empty()) {
        std::println("{}: id={}, score={:.1f}, name={}", label, p.id, p.score, p.name);
    } else {
        std::println("Player: id={}, score={:.1f}, name={}", p.id, p.score, p.name);
    }
}

} // namespace streams

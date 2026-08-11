#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  binary_io.hpp — 二进制文件 I/O 工具                                        ║
// ║  提供 Player 结构体及其二进制序列化/反序列化辅助函数                         ║
// ║  演示 read/write 的 POD 序列化模式 (不可跨平台, 仅用于学习)                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <fstream>
#include <string>
#include <print>
#include <cstring>

namespace streams {

// ── Player: 用于二进制 I/O 演示的 POD 结构体 ─────────────────────────────────
// 注意: 二进制序列化仅适用于 POD (Plain Old Data) 类型
//       - 不含指针、不含 STL 容器、不含虚函数
//       - 跨平台时需考虑字节序 (endianness) 和对齐 (alignment) 差异
struct Player {
    int id = 0;
    double score = 0.0;
    char name[16] = "";   // 固定大小字符数组, 不是 char* 指针
};

// ── 声明 (实现在 src/binary_io.cpp) ──────────────────────────────────────────

/// 将 Player 对象以二进制格式写入文件
/// 返回 true 表示写入成功
bool write_player(const std::string& path, const Player& p);

/// 从二进制文件中读取 Player 对象
/// 返回 true 表示读取成功
bool read_player(const std::string& path, Player& p);

/// 验证两个 Player 的数据是否完全一致
bool validate_player(const Player& a, const Player& b);

/// 打印 Player 信息到控制台
void print_player(const Player& p, std::string_view label = "");

} // namespace streams

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  06_streams_and_io :: 02_file_streams_demo                                   ║
// ║  学习目标:                                                                   ║
// ║    1. 用 ifstream/ofstream/fstream 读写文件                                  ║
// ║    2. 掌握打开模式: in/out/binary/ate/app/trunc                              ║
// ║    3. 用 read/write 进行二进制读写                                            ║
// ║    4. 用 seekg/seekp/tellg/tellp 进行文件随机访问                             ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <print>
#include <fstream>     // ifstream, ofstream, fstream
#include <filesystem>  // 创建/删除临时目录
#include <string>
#include <vector>
#include "shared/lesson_utils.hpp"
#include "streams/binary_io.hpp"      // Player + 二进制读写辅助函数

int main() {
    // ═══════════════════════════════════════════════════════════════════════════
    // Part 1: 文件输出 — ofstream
    // ═══════════════════════════════════════════════════════════════════════════
    // IS: ofstream (output file stream) 把数据写到文件
    // WHY: 程序运行的结果需要持久化 (配置文件、日志、报表、缓存)
    // WHEN: 任何需要"保存数据到磁盘"的场景
    // 构造时传文件名自动打开; 析构时自动关闭 (RAII — 资源获取即初始化)
    // 打开模式 (可组合: ofstream f("a.txt", std::ios::out | std::ios::app)):
    //   out    默认, 覆盖写入
    //   app    追加模式 (append), 写指针自动移到末尾
    //   ate    打开后立刻 seek 到末尾 (At The End), 但之后可 seek 到别处
    //   trunc  截断已有内容 (与 out 同时使用, 是默认行为)
    //   binary 二进制模式 (不转换换行符)
    lesson::print_header("06.2 — 文件流: 读写 + 二进制 + 随机访问");

    lesson::print_subtitle("Part 1: 文本文件写入");
    // 准备临时目录
    std::filesystem::path tmpdir = std::filesystem::temp_directory_path() / "cpplesson_06";
    std::filesystem::create_directories(tmpdir);

    {
        std::string path = (tmpdir / "notes.txt").string();
        // ofstream 构造时自动打开; 析构时自动关闭 → RAII, 不担心忘记关文件
        std::ofstream fout(path);
        if (!fout) {
            std::println(std::cerr, "错误: 无法创建文件 {}", path);
            return 1;
        }
        fout << "C++ 学习笔记\n";
        fout << "═══════════\n";
        fout << "1. iostream   — 控制台 I/O\n";
        fout << "2. fstream    — 文件 I/O\n";
        fout << "3. sstream    — 字符串 I/O\n";
        fout << "4. format     — 格式化 (C++20/23)\n";
        std::println("已写入: {}", path);
        // fout 在这里自动关闭 (离开作用域时析构)
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 2: 文件输入 — ifstream
    // ═══════════════════════════════════════════════════════════════════════════
    // IS: ifstream (input file stream) 从文件读取数据
    // 读取方式: >> (跳空白), getline (读整行), read (二进制), get (逐字符)
    // 检查: while (fin >> val) 在读取成功时进入循环, 推荐替代 while(!eof)
    lesson::print_subtitle("Part 2: 文本文件读取");

    {
        std::string path = (tmpdir / "notes.txt").string();
        std::ifstream fin(path);
        if (!fin) {
            std::println(std::cerr, "错误: 无法打开文件 {}", path);
            return 1;
        }
        std::println("读取 {}:", path);
        std::string line;
        int line_no = 0;
        // getline 返回流引用, 可以放在 while 条件中 — 读取成功才进循环
        while (std::getline(fin, line)) {
            std::println("  [{}] {}", ++line_no, line);
        }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 3: 打开模式详解
    // ═══════════════════════════════════════════════════════════════════════════
    // 模式标志 (位于 std::ios 命名空间, 本质是位掩码):
    //   in      只读 (ifstream 默认)
    //   out     只写 (ofstream 默认, 隐含 trunc)
    //   app     追加 — 每次写入前 seek 到末尾 (原子操作, 多进程日志安全)
    //   ate     打开时定位到末尾 — 只 seek 一次, 之后可写任意位置
    //   trunc   截断已有内容 (与 out 同时使用时是默认行为)
    //   binary  二进制模式 — 不转换换行符 (Windows 上 \n → \r\n 会破坏二进制数据)
    //   nocreate 打开已有文件, 不创建新文件 (非标准, 部分编译器支持)
    // WHY: 不同场景需要不同的打开行为 — 日志需要追加, 二进制文件不能转译换行符
    lesson::print_subtitle("Part 3: 打开模式");

    {
        std::string path = (tmpdir / "log.txt").string();
        // 追加模式: 多次运行程序, 每次的日志都追加到文件末尾
        for (int i = 1; i <= 3; ++i) {
            std::ofstream log(path, std::ios::app);
            log << "第 " << i << " 条日志\n";
        }

        // 验证: 文件中应该有 3 条记录
        std::ifstream fin(path);
        int count = 0;
        std::string dummy;
        while (std::getline(fin, dummy)) ++count;
        std::println("log.txt 中有 {} 条记录 (预期 3)", count);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 4: 二进制读写 — read / write
    // ═══════════════════════════════════════════════════════════════════════════
    // IS: read(char* buf, size) 从流读原始字节; write(char* buf, size) 写原始字节
    // WHY: 序列化结构体/数值 → 速度快, 体积小, 但不可跨平台 (字节序、对齐)
    // WHEN: 游戏存档、图像数据、网络包、嵌入式系统
    // 注意: 必须用 binary 模式 + reinterpret_cast<char*>
    lesson::print_subtitle("Part 4: 二进制读写");

    {
        // 使用 streams::Player — 来自 binary_io.hpp
        streams::Player src{1001, 99.5, "Ecks"};
        std::string path = (tmpdir / "player.bin").string();

        // 二进制写入 — 使用 streams::write_player()
        if (streams::write_player(path, src)) {
            std::println("二进制写入成功 (使用 streams::write_player)");
            streams::print_player(src, "写入的玩家");
        }

        // 二进制读取 — 使用 streams::read_player()
        streams::Player dst{};
        if (streams::read_player(path, dst)) {
            streams::print_player(dst, "读取的玩家");

            // 验证数据完整性 — 使用 streams::validate_player()
            bool ok = streams::validate_player(src, dst);
            std::println("数据完整性: {} ({})", ok ? "通过" : "失败",
                         ok ? "id/score/name 全部正确" : "数据不匹配");
        }

        // 你也可以直接使用底层 read/write:
        // std::ofstream fout(path, std::ios::binary);
        // fout.write(reinterpret_cast<const char*>(&src), sizeof(src));
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // Part 5: 随机访问 — seekg/seekp/tellg/tellp
    // ═══════════════════════════════════════════════════════════════════════════
    // IS: seekg(offset, dir) 移动读指针; seekp(offset, dir) 移动写指针
    //     tellg() 返回读指针位置; tellp() 返回写指针位置
    //     dir: beg(开头), cur(当前位置), end(末尾)
    // WHY: 不需要把整个文件读入内存, 直接跳到目标位置读写 — 适合大文件
    // WHEN: 数据库索引、日志查询、大文件编辑
    lesson::print_subtitle("Part 5: 随机访问 (seek/tell)");

    {
        std::string path = (tmpdir / "random.txt").string();
        // 先写一个已知布局的文件
        {
            std::ofstream fout(path);
            // 固定宽度: 每条记录 20 字节 (不含换行)
            fout << "Alice   1001 95.2\n";
            fout << "Bob     1002 87.0\n";
            fout << "Charlie 1003 92.8\n";
        }

        std::ifstream fin(path);
        // 查文件大小: seekg 到末尾, 然后 tellg
        fin.seekg(0, std::ios::end);
        auto file_size = fin.tellg();
        std::println("文件大小: {} 字节", file_size);

        // 直接跳到第 2 条记录 (偏移 21 字节, 因为有换行符)
        // 更实际的做法: 每条记录固定宽度, 用 seekg(offset) 直接跳转
        fin.seekg(0, std::ios::beg);  // 回到开头
        std::string first_line;
        std::getline(fin, first_line);
        std::println("seekg(0, beg) → 第 1 条: {}", first_line);

        // 从当前位置跳过 21 字节去读第 3 条
        fin.seekg(0, std::ios::beg);   // 先回开头
        fin.seekg(21 * 2, std::ios::beg); // 第 3 条记录的起始位置
        std::string third_line;
        std::getline(fin, third_line);
        std::println("seekg(42, beg) → 第 3 条: {}", third_line);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // 常见陷阱
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("常见陷阱");
    std::println("陷阱 1: 忘记 binary 模式 — Windows 上会把 0x0A 转成 \\r\\n, 破坏数据");
    std::println("  → 解决: std::ofstream f(path, std::ios::binary);");
    std::println("陷阱 2: 结构体含指针/容器时直接 write — 只写入了指针值, 不是数据");
    std::println("  → 解决: 二进制序列化只用于 POD 类型; 复杂数据用 protobuf/flatbuffers/JSON");
    std::println("陷阱 3: 用 while(!fin.eof()) 读文件 — 最后一条记录被读两次");
    std::println("  → 解决: while(fin >> val) 或 while(getline(fin, line))");
    std::println("陷阱 4: 没有检查文件是否成功打开 — 然后对无效流进行操作, 静默失败");
    std::println("  → 解决: if (!fin) { /* 处理错误 */ }");

    // ═══════════════════════════════════════════════════════════════════════════
    // 练习
    // ═══════════════════════════════════════════════════════════════════════════
    lesson::print_subtitle("练习");
    std::println("1. 写一个程序: 把 1~100 的平方数写入文件, 每行一个");
    std::println("2. 写一个程序: 用 seekg 读取上题文件中的第 50 个平方数, 不用遍历前 49 行");
    std::println("3. 写一个简单的日志类: 构造时打开文件 (追加模式), 析构时关闭");
    std::println("4. 尝试用 read/write 读写一个包含 int/double/bool 的结构体");

    // 清理临时文件
    std::filesystem::remove_all(tmpdir);
    return 0;
}

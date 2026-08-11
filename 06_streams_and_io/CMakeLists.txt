# ------------------------------------------------------------------------------
# 06_streams_and_io — I/O与格式化
#
# Library:  streams_lib
#   Sources: src/binary_io.cpp, src/simple_json.cpp
#   Headers: include/streams/binary_io.hpp, include/streams/point3d.hpp,
#            include/streams/simple_json.hpp, include/streams/stream_utils.hpp
#   Links:   lesson_utils (PUBLIC — 提供 lesson::print_header 等全局辅助函数)
#
#   point3d.hpp 和 stream_utils.hpp 为 header-only，无对应 .cpp
#
# Examples:
#   01_iostream_basics_demo  → 流基础 (cin/cout, 状态位, 格式化标志)
#   02_file_streams_demo     → 文件流 (ifstream/ofstream/fstream, 文本+二进制)
#   03_string_streams_demo   → 字符串流 (istringstream/ostringstream, 拼接与解析)
#   04_format_and_print_demo → C++23 std::print / std::format / std::formatter
#
# 所有示例均 PRIVATE 链接 streams_lib（示例不对外暴露库依赖）
# ------------------------------------------------------------------------------

# ---- Library ---------------------------------------------------------------
add_library(streams_lib STATIC
    src/binary_io.cpp
    src/simple_json.cpp
)

# PUBLIC: 任何链接 streams_lib 的目标都能通过 "streams/xxx.hpp" 引用头文件
target_include_directories(streams_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# PUBLIC: lesson_utils（全局辅助工具库）—— streams_lib 使用者间接获得 lesson_utils 符号
# 本模块头文件仅依赖标准库（<iostream>, <format>, <print>, <fstream>, <vector> 等），
# 不依赖任何前置模块（01_fundamentals, 02_classes 等），故无需额外 PUBLIC 链接
target_link_libraries(streams_lib PUBLIC
    lesson_utils
)

# ---- Examples --------------------------------------------------------------

# 01 — iostream 基础: cin/cout/cerr, 格式化标志, 流状态管理, 宽字符流
add_executable(01_iostream_basics_demo
    examples/01_iostream_basics_demo.cpp
)
target_link_libraries(01_iostream_basics_demo PRIVATE
    streams_lib
)

# 02 — 文件流: ifstream/ofstream/fstream, 文件模式, 指针安全操作, binary I/O
add_executable(02_file_streams_demo
    examples/02_file_streams_demo.cpp
)
target_link_libraries(02_file_streams_demo PRIVATE
    streams_lib
)

# 03 — 字符串流: istringstream 解析, ostringstream 拼接, 类型转换, CSV 解析
add_executable(03_string_streams_demo
    examples/03_string_streams_demo.cpp
)
target_link_libraries(03_string_streams_demo PRIVATE
    streams_lib
)

# 04 — C++23 格式化: std::print/std::println, std::format/std::format_to, std::formatter 特化
add_executable(04_format_and_print_demo
    examples/04_format_and_print_demo.cpp
)
target_link_libraries(04_format_and_print_demo PRIVATE
    streams_lib
)

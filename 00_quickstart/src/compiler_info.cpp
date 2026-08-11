// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  compiler_info.cpp — print_compiler_info() 实现                             ║
// ║  使用预处理宏判断编译器类型并打印版本信息                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "quickstart/compiler_info.hpp"

namespace lesson::quickstart {

void print_compiler_info() {
    // 预处理宏在编译期就已确定，编译器只会保留匹配的那一个分支
    // 其他分支的代码不会被编译到最终二进制中（这是"条件编译"）
#if defined(__GNUC__)
    std::println("编译器: GCC {}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif defined(__clang__)
    std::println("编译器: Clang {}.{}.{}", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(_MSC_VER)
    std::println("编译器: MSVC {}", _MSC_VER);
#else
    std::println("编译器: 未知");
#endif
}

} // namespace lesson::quickstart

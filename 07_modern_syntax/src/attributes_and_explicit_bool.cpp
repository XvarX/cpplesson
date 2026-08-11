// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/attributes_and_explicit_bool.cpp                                       ║
// ║  实现 [[no_unique_address]] / explicit(bool) / hardware_destructive_       ║
// ║  interference_size 相关的非内联函数                                         ║
// ║                                                                           ║
// ║  注: 大部分类型都在头文件里(模板 + 内联结构体),                            ║
// ║      此处仅实现少量非内联的工具函数                                         ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "modern_syntax/attributes_and_explicit_bool.hpp"

#include <print>
#include <string>

namespace modern_syntax {

// ── LoggingPolicy 的实现 ──
void LoggingPolicy::log(const std::string& msg) const {
    std::println("[LOG] {}", msg);
}

// ── Processor::process 的实现 ──
void Processor::process() const {
    logger.log("处理数据...");
    // 实际处理逻辑
}

// ── GenericProcessor::run 的实现 ──
template<typename LoggerPolicy>
void GenericProcessor<LoggerPolicy>::run() const {
    logger.log("GenericProcessor 运行中...");
}

// ── 显式实例化, 确保模板代码正确编译 ──
template void GenericProcessor<LoggingPolicy>::run() const;
template void GenericProcessor<NoOpPolicy>::run() const;

// ── 打印各类型大小信息 ──
// 直观展示 [[no_unique_address]] 和对齐优化的效果
void print_size_info() {
    std::println("══════════════ 类型大小一览 ══════════════");

    // ── [[no_unique_address]] 效果 ──
    std::println("[1] sizeof(LoggingPolicy)  = {} (空类,  最小占用)", sizeof(LoggingPolicy));
    std::println("[2] sizeof(NoOpPolicy)     = {} (空类,  最小占用)", sizeof(NoOpPolicy));
    std::println("[3] sizeof(int)            = {} (整型)",         sizeof(int));
    std::println("[4] sizeof(Processor)      = {} (带 [[no_unique_address]] 的处理器, "
                 "空 logger + int)", sizeof(Processor));
    // 预期: sizeof(Processor) ≈ sizeof(int), logger 不占额外空间

    // ── 对齐与伪共享 ──
    std::println("[5] sizeof(MisalignedCounters)     = {} (未对齐)",     sizeof(MisalignedCounters));
    std::println("[6] sizeof(AlignedCounters)        = {} (对齐到不同缓存行)", sizeof(AlignedCounters));
    std::println("[7] hardware_destructive_interference_size = {} 字节",
                 std::hardware_destructive_interference_size);

    // ── 模板版本对比 ──
    using StringBoolWrapper = SmartWrapper<std::string>;
    std::println("[8] sizeof(SmartWrapper<int>)    = {} (int 包装)",    sizeof(SmartWrapper<int>));
    std::println("[9] sizeof(SmartWrapper<string>) = {} (string 包装)", sizeof(StringBoolWrapper));

    std::println("══════════════════════════════════════════");
}

} // namespace modern_syntax

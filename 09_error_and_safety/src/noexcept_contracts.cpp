// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  实现: error_safety/noexcept_contracts.hpp                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "error_safety/noexcept_contracts.hpp"
#include <print>

// ── SafeString::operator= (copy-and-swap 惯用法) ───────────────────────────────

SafeString& SafeString::operator=(const SafeString& other) {
    SafeString tmp{other};   // 拷贝构造 — 可能抛异常
    swap(tmp);               // 交换 — noexcept，不会抛异常
    return *this;
    // 如果拷贝失败: tmp 没创建成功，*this 未被改变 → 强保证
}

// ── computeImportantValue ──────────────────────────────────────────────────────

[[nodiscard]] int computeImportantValue() {
    return 42;  // 调用者不应该忽略这个值
}

// ── allocateBuffer ─────────────────────────────────────────────────────────────

[[nodiscard("资源泄漏风险: 返回的句柄必须被管理")]]
void* allocateBuffer(size_t size) {
    return std::malloc(size);
}

// ── openFile (模拟) ────────────────────────────────────────────────────────────

[[nodiscard("必须检查文件操作的结果")]]
FileResult openFile(std::string_view path) {
    // 模拟: 总是成功
    std::println("  打开文件: {}", path);
    return FileResult::Success;
}

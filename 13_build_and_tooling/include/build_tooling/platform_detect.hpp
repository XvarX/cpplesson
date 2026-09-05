#pragma once
// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  build_tooling/platform_detect.hpp — 平台/编译器/头文件检测 (header-only)    ║
// ║  主题: 预定义宏检测平台, NDEBUG 检测构建类型, __has_include 检测头文件       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// ── 平台检测: 用编译器预定义宏 (跨平台代码的第一步) ──────────────────────────
#if defined(_WIN32)
#define PLATFORM "Windows"
#elif defined(__APPLE__)
#define PLATFORM "macOS"
#elif defined(__linux__)
#define PLATFORM "Linux"
#else
#define PLATFORM "Unknown"
#endif

// ── 构建类型检测 ──────────────────────────────────────────────────────────────
// NDEBUG 是 C 标准宏: Release 构建时定义, assert 失效
// _DEBUG 是 MSVC 约定的后备 (GCC/Clang 不定义它)
#if !defined(NDEBUG) || defined(_DEBUG)
#define IS_DEBUG 1
#else
#define IS_DEBUG 0
#endif

// ── 功能开关: 编译时 -DENABLE_LOGGING=1 开启 (0/1, 可用于 #if) ────────────────
#ifndef ENABLE_LOGGING
#define ENABLE_LOGGING 1
#endif

// ── __has_include (C++17): 编译期检测头文件是否存在 ──────────────────────────
// 用途: 可选依赖 — 有则用, 无则回退
#if defined(__has_include)
#  if __has_include(<optional>)
#    define HAS_OPTIONAL 1
#  else
#    define HAS_OPTIONAL 0
#  endif
#  if __has_include(<version>)
#    define HAS_VERSION_HEADER 1
#  else
#    define HAS_VERSION_HEADER 0
#  endif
#  if __has_include(<unistd.h>)
#    define HAS_UNISTD 1
#  else
#    define HAS_UNISTD 0
#  endif
#else
#  define HAS_OPTIONAL 0
#  define HAS_VERSION_HEADER 0
#  define HAS_UNISTD 0
#endif

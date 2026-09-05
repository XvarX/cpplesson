// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  13_build_and_tooling — examples/cmake_deep_dive_demo.cpp                      ║
// ║  学习目标:                                                                    ║
// ║    1. 理解 target 的 PUBLIC/PRIVATE/INTERFACE 属性传递机制                    ║
// ║    2. 了解 install/export 如何让库被其他项目使用                               ║
// ║    3. 掌握 find_package 的工作原理 (CONFIG 模式与 MODULE 模式)                 ║
// ║    4. 入门 CTest — 在 CMake 中注册和运行测试                                   ║
// ║  本文件通过 C++ 代码模拟这些 CMake 概念的行为，帮助你建立直觉                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include <iostream>
#include <print>
#include <string>
#include <vector>
#include <functional>

#include "shared/lesson_utils.hpp"
#include "build_tooling/cmake_sim.hpp"
#include "build_tooling/ctest_sim.hpp"

using build_tooling::Target;
using build_tooling::CTestRunner;

// ═══════════════════════════════════════════════════════════════════════════════
// Part 1: Target 属性传递 — PUBLIC / PRIVATE / INTERFACE
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是 target 属性?
//   在 CMake 中，target (可执行文件或库) 可以附加"属性"：
//     - include 目录  (target_include_directories)
//     - 编译定义      (target_compile_definitions)
//     - 链接库        (target_link_libraries)
//     - 编译选项      (target_compile_options)
//
// 为什么需要 PUBLIC/PRIVATE/INTERFACE 三种传递方式?
//   CMake 的依赖是"图"结构: libA → libB → libC (箭头表示"依赖")
//   当 libA 链接 libB 时，libB 的属性如何处理?
//     PRIVATE   — 只给 libB 自己用，不传给 libA
//     INTERFACE — libB 自己不用，但传给 libA (仅头文件库常用)
//     PUBLIC    — libB 自己用，也传给 libA
//
// 何时使用:
//   - PRIVATE:   内部实现细节 (如 .cpp 中才需要的头文件)
//   - INTERFACE: 头文件库 (header-only)，只有接口没有 .cpp
//   - PUBLIC:    你的头文件暴露的类型依赖了其他库

// ── 用 C++ 类模拟 CMake Target 的属性传递 ─────────────────────────────────────
// build_tooling::Target 类定义在 include/build_tooling/cmake_sim.hpp 中
// 这是一个教学模拟: 帮助你理解 CMake 中 link/include 传递规则

void demo_target_properties() {
    lesson::print_subtitle("Part 1: Target 属性 PUBLIC / PRIVATE / INTERFACE 模拟");

    // 模拟三层依赖: myapp → mylib → thirdparty
    Target thirdparty("thirdparty");
    thirdparty.iface_includes.push_back("thirdparty/");  // 头文件库, INTERFACE

    Target mylib("mylib");
    mylib.priv_includes.push_back("internal/");  // 内部实现, PRIVATE
    mylib.pub_includes.push_back("mylib/public/"); // mylib 的头文件引用了 thirdparty, PUBLIC

    Target myapp("myapp");

    // myapp 链接 mylib → 继承 mylib 的 PUBLIC+INTERFACE includes
    // mylib 链接 thirdparty → 继承 thirdparty 的 INTERFACE includes
    // 最终 myapp 应该能看到: mylib/public/ 和 thirdparty/
    auto inherited = mylib.inherited_includes();
    auto third_inherited = thirdparty.inherited_includes();

    std::println("  mylib 自己可见的 include: {}", mylib.my_includes());
    std::println("  myapp 从 mylib 继承的 include: {}", inherited);
    std::println("  mylib 从 thirdparty 继承的 include: {}", third_inherited);
    std::println("  ✓ myapp 最终可见: mylib/public/ (PUBLIC), thirdparty/ (INTERFACE)");
    std::println("  ✗ myapp 看不到: internal/ (PRIVATE)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 2: install / export — 让库可以被外部项目使用
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是 install/export?
//   install: 把编译产物 (库文件、头文件、CMake 配置文件) 拷贝到安装目录
//   export:  注册 target，让同个构建树内的其他项目通过 find_package 找到你
//
// 为什么需要?
//   你写了一个库，别人想用。你不能让他们复制你的源码和 CMakeLists.txt
//   而是把编译好的 .so/.a + 头文件 + CMake 配置文件打包"安装"到系统
//
// 典型流程:
//   cmake --install build --prefix /usr/local
//   然后其他项目就可以 find_package(MyLib REQUIRED)

void demo_install_concept() {
    lesson::print_subtitle("Part 2: install / export 概念演示");

    // 用一个结构体模拟 install 导出时的"配置文件"信息
    struct PackageConfig {
        std::string name;
        std::string version;
        std::string include_dir;
        std::string lib_dir;
        std::string cmake_config_path; // *Config.cmake 或 *-config.cmake
    };

    // 模拟 mylib 安装后的配置
    PackageConfig mylib_config{
        .name = "mylib",
        .version = "1.2.0",
        .include_dir = "/usr/local/include/mylib/",
        .lib_dir = "/usr/local/lib/",
        .cmake_config_path = "/usr/local/lib/cmake/mylib/mylibConfig.cmake"
    };

    std::println("  安装后的 mylib 配置:");
    std::println("    名称:        {}", mylib_config.name);
    std::println("    版本:        {}", mylib_config.version);
    std::println("    头文件目录:  {}", mylib_config.include_dir);
    std::println("    库文件目录:  {}", mylib_config.lib_dir);
    std::println("    CMake 配置:  {}", mylib_config.cmake_config_path);
    std::println("  ✓ find_package(mylib) 会搜索 *Config.cmake 或 *-config.cmake");
    std::println("  ⚠ install(EXPORT ...) 会生成这些 *-config.cmake 文件");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 3: find_package — 两种查找模式
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是 find_package?
//   CMake 提供的命令，用来查找并加载外部依赖 (库、工具)
//   find_package(OpenSSL REQUIRED)  → 找到 OpenSSL
//   find_package(Qt6 COMPONENTS Widgets Core) → 找到 Qt6 的 Widgets 和 Core 模块
//
// 两种模式:
//   MODULE 模式 — CMake 自带 Find<Package>.cmake 脚本查找
//                 (CMake 内置了对 Boost, OpenSSL, ZLIB 等的查找脚本)
//   CONFIG 模式 — 库作者提供的 <Package>Config.cmake 文件
//                 (现代 CMake 推荐，库作者通过 install(EXPORT) 生成)
//
// 查找顺序 (CONFIG 模式):
//   1. CMAKE_PREFIX_PATH 环境变量
//   2. <Package>_ROOT 变量 (如 Qt6_ROOT=/opt/Qt/6.5.0)
//   3. 系统默认路径 (/usr, /usr/local, C:/Program Files/...)

void demo_find_package() {
    lesson::print_subtitle("Part 3: find_package 工作机制");

    // 模拟 find_package 的搜索路径优先级
    struct SearchPath {
        int priority;
        std::string source;
        std::string path;
    };

    std::vector<SearchPath> search_order = {
        {1, "CMAKE_PREFIX_PATH",   "/home/user/custom_libs/"},
        {2, "<Package>_ROOT 变量", "/opt/mylib/3.0/"},
        {3, "系统默认路径",        "/usr/local/"},
        {4, "系统默认路径",        "/usr/"},
    };

    std::println("  CONFIG 模式搜索顺序 (优先级从高到低):");
    for (const auto& s : search_order) {
        std::println("    {} {} → {}", s.priority, s.source, s.path);
    }
    std::println("");
    std::println("  实用技巧:");
    std::println("    cmake -DCMAKE_PREFIX_PATH=/path/to/libs ..");
    std::println("    cmake -DMyLib_ROOT=/opt/mylib ..");
    std::println("  ✓ 推荐使用 presets (CMakePresets.json) 管理这些变量");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 4: CTest 入门 — 在 CMake 中注册并运行测试
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是 CTest?
//   CMake 自带的测试运行器。通过 add_test() 注册测试，ctest 命令运行
//
// 为什么需要?
//   不用自己写 shell 脚本跑测试; 与 CDash 集成可以做持续集成
//   ctest --output-on-failure 只显示失败的测试输出
//   ctest -R <pattern> 按名称筛选运行测试
//
// CMakeLists.txt 中的典型写法:
//   enable_testing()
//   add_executable(test_foo test_foo.cpp)
//   target_link_libraries(test_foo PRIVATE mylib)
//   add_test(NAME foo_test COMMAND test_foo)

// ── 用 C++ 模拟 CTest 的测试注册和运行 ───────────────────────────────────────
// build_tooling::CTestRunner 类定义在 include/build_tooling/ctest_sim.hpp 中

void demo_ctest() {
    lesson::print_subtitle("Part 4: CTest 入门 — 测试注册与运行模拟");

    CTestRunner ctest;

    // 模拟 add_test(NAME xxx COMMAND yyy)
    ctest.add_test("math_add", []() {
        return 1 + 1 == 2;
    });

    ctest.add_test("math_mul", []() {
        return 2 * 3 == 6;
    });

    ctest.add_test("string_concat", []() {
        std::string a = "Hello", b = "World";
        return a + " " + b == "Hello World";
    });

    std::println("  模拟 CTest 运行 (相当于 ctest 命令):");
    ctest.run_all();

    std::println("\n  实际 CMakeLists.txt 写法:");
    std::println("    enable_testing()");
    std::println("    add_test(NAME math_test COMMAND test_math)");
    std::println("  ✓ 运行: ctest (或 ctest --output-on-failure)");
    std::println("  ✓ 筛选: ctest -R math   (只跑名字含 math 的测试)");
    std::println("  ✓ 并行: ctest -j4       (4 个测试并行跑)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// Part 5: CMakePresets.json — 配置预设 (现代 CMake 推荐)
// ═══════════════════════════════════════════════════════════════════════════════
// 什么是 CMakePresets.json?
//   一个 JSON 文件，存储 cmake 配置参数，取代手写命令行参数
//   cmake --preset default   → 使用 preset 中定义的参数
//
// 为什么需要?
//   - 团队成员共享一致的构建配置
//   - 避免每次手敲 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=...
//   - CI/CD 中也用同一个 preset

void demo_presets() {
    lesson::print_subtitle("Part 5: CMakePresets.json 配置预设");

    std::println("  示例 CMakePresets.json:");
    std::println("{}", R"(  {
      "version": 3,
      "configurePresets": [
        {
          "name": "debug",
          "displayName": "Debug 构建",
          "generator": "Ninja",
          "binaryDir": "build/debug",
          "cacheVariables": {
            "CMAKE_BUILD_TYPE": "Debug",
            "CMAKE_CXX_COMPILER": "g++-14"
          }
        },
        {
          "name": "release",
          "displayName": "Release 构建",
          "binaryDir": "build/release",
          "cacheVariables": {
            "CMAKE_BUILD_TYPE": "Release"
          }
        }
      ]
    })");
    std::println("\n  使用:");
    std::println("    cmake --preset debug     # 用 debug preset 配置");
    std::println("    cmake --build --preset debug  # 构建 (CMake 3.23+)");
}

// ═══════════════════════════════════════════════════════════════════════════════
// 常见陷阱
// ═══════════════════════════════════════════════════════════════════════════════
// 1. PRIVATE/INTERFACE 搞反:
//    target_link_libraries(mylib PRIVATE otherlib)
//    如果你的头文件里 #include 了 otherlib 的头文件，这里必须用 PUBLIC
//    否则使用你库的人会编译失败 (找不到 otherlib 的头文件)
//
// 2. find_package 找不到库:
//    检查 CMAKE_PREFIX_PATH 或 <Package>_ROOT 是否设置正确
//    用 cmake --debug-find 查看搜索过程
//
// 3. install 后头文件路径不对:
//    install(FILES ... DESTINATION include) 会直接放到 include/
//    通常应该: install(DIRECTORY include/ DESTINATION include)
//    否则会丢失目录结构
//
// 4. CTest 忘记 enable_testing():
//    必须在顶层 CMakeLists.txt 中调用 enable_testing()
//    否则 add_test() 不会报错但 ctest 也找不到任何测试

// ═══════════════════════════════════════════════════════════════════════════════
// 练习
// ═══════════════════════════════════════════════════════════════════════════════
// 1. 用 CMake 创建一个小项目: 一个库 (mylib) + 一个可执行文件 (myapp)
//    分别用 PUBLIC / PRIVATE 链接，观察编译结果
// 2. 给上面的项目添加 install 指令，运行 cmake --install build --prefix ./out
//    观察安装后的文件结构
// 3. 给上面的项目添加 CTest: enable_testing() + add_test()
//    运行 ctest --output-on-failure
// 4. 为一个第三方库 (如 fmt) 编写 find_package 调用，理解 CMAKE_PREFIX_PATH

// ═══════════════════════════════════════════════════════════════════════════════

int main() {
    lesson::print_header("01 — CMake 进阶: Target 属性 / install / find_package / CTest");

    demo_target_properties();
    demo_install_concept();
    demo_find_package();
    demo_ctest();
    demo_presets();

    lesson::print_separator("完成: 现在你对 CMake 的核心机制有了深入理解");

    return 0;
}

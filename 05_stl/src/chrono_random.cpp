// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/chrono_random.cpp — 时间库与随机数库 教学演示函数实现                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/chrono_random.hpp"

using namespace std;
using namespace chrono;

namespace stl_learn {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: std::chrono::duration — 时间段                                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_duration() {
    lesson::print_subtitle("Part 1: duration — 时间段");

    // 常用 duration 类型别名
    seconds s{5};              // 5 秒
    milliseconds ms{250};      // 250 毫秒
    microseconds us{1000};     // 1000 微秒
    minutes m{2};              // 2 分钟
    hours h{1};                // 1 小时

    // 算术运算 — 自动处理单位转换
    auto total = s + ms;       // 结果是 milliseconds (精确保留)
    println("5s + 250ms = {}ms", total.count());

    // duration_cast: 强制转换 (可能丢失精度)
    auto as_sec = duration_cast<seconds>(total);
    println("{}ms = {}s (取整)", total.count(), as_sec.count());
    // 预期: 5250ms = 5s

    // 自定义 duration: 使用 ratio
    using frames = duration<int, ratio<1, 60>>;  // 1/60 秒 = 一帧 (60fps)
    frames f{180};  // 180 帧
    auto f_in_sec = duration_cast<seconds>(f);
    println("180帧 @60fps = {} 秒", f_in_sec.count());  // 预期: 3

    // count() 获取原始数值
    hours two_h{2};
    println("2 小时的分钟数: {}", duration_cast<minutes>(two_h).count());
    // 预期: 120
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: time_point 与 clock                                               ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_time_point() {
    lesson::print_subtitle("Part 2: time_point 与 clock");

    // C++ 标准提供 3 种时钟:
    // system_clock: 系统时间 (可转换为日历时间，受 NTP 调整影响)
    // steady_clock: 单调递增时钟 (适合测量时间间隔，不受系统时间调整影响)
    // high_resolution_clock: 最高精度时钟 (通常是 steady_clock 的别名)

    // ── steady_clock: 测量代码执行时间 ──
    auto start = steady_clock::now();

    // 模拟一些工作
    volatile int sum = 0;
    for (int i = 0; i < 1'000'000; ++i) sum += i;

    auto end = steady_clock::now();
    auto elapsed = duration_cast<microseconds>(end - start);
    println("循环耗时: {} μs", elapsed.count());

    // ── system_clock: 获取当前时间 ──
    auto now_sys = system_clock::now();
    // 转换为 C 风格时间用于打印
    // (C++23 有更好的 std::chrono::format, 这里用传统方式)
    auto tt = system_clock::to_time_t(now_sys);
    println("当前时间 (UTC): {}", ctime(&tt));  // 显示 UTC 时间

    // ── 时间点运算 ──
    auto future = now_sys + hours(24);
    auto diff = future - now_sys;
    println("24h = {}s", duration_cast<seconds>(diff).count());  // 预期: 86400
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: C++14 时间字面量                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_literals() {
    lesson::print_subtitle("Part 3: 时间字面量 (C++14)");

    using namespace chrono_literals;

    auto a = 5s;       // seconds
    auto b = 250ms;    // milliseconds
    auto c = 100us;    // microseconds
    auto d = 2h;       // hours
    auto e = 30min;    // minutes

    println("5s = {}ms", duration_cast<milliseconds>(5s).count());
    println("250ms = {}μs", duration_cast<microseconds>(250ms).count());

    // 混合运算 — 类型安全!
    auto total = 1h + 30min + 45s;  // 自动转换为最精确的单位
    println("1h30m45s = {}s", duration_cast<seconds>(total).count());
    // 预期: 5445

    // 实际用途: 超时设置
    auto timeout = 500ms;
    println("超时设置为 {}ms", duration_cast<milliseconds>(timeout).count());

    // sleep_for: 虽然参数用 duration，但这是 thread 库的函数
    // 这里只是演示 duration 的实际用途
    auto wait = 10ms;
    // this_thread::sleep_for(wait);  // 实际运行可取消注释
    println("等待 {}ms (演示)", wait.count());

    // 注意: chrono_literals 与自定义字面量冲突时的处理
    // 可以用 using chrono_literals::s; 只导入需要的后缀
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: <random> — 随机数引擎 (Engine)                                    ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_random_engine() {
    lesson::print_subtitle("Part 4: <random> — 随机数引擎");

    // ── 随机设备: 获取真随机种子 ──
    random_device rd;

    // ── 常用引擎 ──
    // mt19937: 梅森旋转算法 (质量好，速度适中，最推荐的通用引擎)
    // mt19937_64: 64位版本
    // minstd_rand: 线性同余 (快但质量差，不推荐)
    // ranlux48: 高质量但很慢

    mt19937 gen{rd()};  // 用真随机数做种子

    // 直接使用引擎 (生成 [0, 2^32-1] 范围内的数)
    println("引擎原始输出: {} {} {}", gen(), gen(), gen());

    // ── 均匀整数分布 ──
    uniform_int_distribution<int> dist1(1, 6);  // [1, 6] 闭区间
    print("骰子10次: ");
    for (int i = 0; i < 10; ++i) print("{} ", dist1(gen));
    println("");
    // 预期: 1-6 之间的 10 个随机数

    // ── 均匀实数分布 ──
    uniform_real_distribution<double> dist2(0.0, 1.0);  // [0.0, 1.0)
    println("3个随机浮点数: {:.4f} {:.4f} {:.4f}",
            dist2(gen), dist2(gen), dist2(gen));
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 5: 常用分布 (distributions)                                           ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part5_distributions() {
    lesson::print_subtitle("Part 5: 常用随机分布");

    random_device rd;
    mt19937 gen{rd()};

    // ── 正态分布 (高斯分布) ──
    normal_distribution<double> norm{0.0, 1.0};  // 均值=0, 标准差=1
    print("标准正态分布样本: ");
    for (int i = 0; i < 5; ++i) print("{:.3f} ", norm(gen));
    println("");

    // ── 伯努利分布: true/false (按概率) ──
    bernoulli_distribution coin{0.7};  // 70% true
    int heads = 0;
    for (int i = 0; i < 1000; ++i)
        if (coin(gen)) ++heads;
    println("1000次掷币 (p=0.7): {}次正面", heads);

    // ── 离散分布: 加权随机 ──
    vector<int> weights{1, 2, 3, 4};    // 权重
    discrete_distribution<int> weighted{weights.begin(), weights.end()};
    // 采样测试
    map<int, int> freq;
    for (int i = 0; i < 10000; ++i) ++freq[weighted(gen)];
    print("加权采样结果 (权1:2:3:4): ");
    for (auto& [k, v] : freq) print("{}:{} ", k, v / 100);
    println("");
    // 预期: 0≈10% 1≈20% 2≈30% 3≈40%

    // ── shuffle: 使用引擎洗牌 ──
    vector<int> cards{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    ranges::shuffle(cards, gen);
    print("洗牌结果: ");
    for (auto x : cards) print("{} ", x);
    println("");

    // ── sample: 从序列中随机采样 ──
    vector<int> sample;
    ranges::sample(cards, back_inserter(sample), 4, gen);
    print("随机抽取4张: ");
    for (auto x : sample) print("{} ", x);
    println("");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 6: std::ratio — 编译期有理数                                          ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part6_ratio() {
    lesson::print_subtitle("Part 6: std::ratio — 编译期有理数");

    // ratio<N, D> 表示 N/D
    using milli = ratio<1, 1000>;    // 1/1000
    using centi = ratio<1, 100>;     // 1/100
    using deci  = ratio<1, 10>;      // 1/10

    // ratio 运算 (编译期)
    using ratio_mul = ratio_multiply<milli, centi>;  // 1/100000
    using ratio_div = ratio_divide<milli, milli>;     // 1/1

    println("milli = 1/{}", milli::den);
    println("centi = 1/{}", centi::den);
    println("milli * centi = 1/{}", ratio_mul::den);  // 预期: 100000

    // 实际用途: 定义自定义时间单位
    // duration<int, ratio<1, 60>> 表示 1/60 秒 (帧)
    using Frame = duration<int, ratio<1, 60>>;
    Frame video_length{3600};  // 3600 帧
    println("3600 帧 @60fps = {} 秒",
            duration_cast<seconds>(video_length).count());
    // 预期: 60

    // 预定义的常用 ratio 别名:
    // std::nano  = ratio<1, 1000000000>
    // std::micro = ratio<1, 1000000>
    // std::milli = ratio<1, 1000>
    // std::centi = ratio<1, 100>
    // std::kilo  = ratio<1000, 1>
    // std::mega  = ratio<1000000, 1>
    println("kilo = {}/1, mega = {}/1", kilo::num, mega::num);
    // 预期: kilo = 1000/1, mega = 1000000/1
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part_pitfalls() {
    lesson::print_subtitle("常见陷阱");

    // 陷阱1: 不要用 rand()! 它范围不均匀、周期短、线程不安全
    lesson::print_note("永远用 <random> 替代 rand() / srand()");

    // 陷阱2: random_device 在某些平台(如 MinGW)可能是确定性的
    // 对密码学用途，需要专门的密码学安全随机库
    lesson::print_note("random_device 在某些老编译器(MinGW)上是伪随机, 不要用于密码学");

    // 陷阱3: uniform_real_distribution 的半开区间 [a, b)
    auto dist = uniform_real_distribution{0.0, 1.0};  // [0.0, 1.0)
    // 要包含 1.0, 用 uniform_real_distribution{0.0, nextafter(1.0, 2.0)}
    lesson::print_note("uniform_real_distribution 是半开区间 [a, b), 不包含上限");

    // 陷阱4: system_clock 不适合测量时间间隔
    // 用户/NTP 调整系统时间会导致跳跃
    lesson::print_note("测量代码耗时用 steady_clock, 不要用 system_clock");

    // 陷阱5: 每次调用都重新构造分布对象是可以的，但引擎应该复用
    // 错误做法:
    // for (...) { uniform_int_distribution<int> d(1,6); d(gen); }  // 浪费
    // 正确做法: 分布对象在循环外构造
    lesson::print_note("随机引擎应该复用，分布对象也可复用 (但轻量，影响不大)");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part_exercises() {
    lesson::print_subtitle("练习");

    println("1. 用 steady_clock 测量对一个 10 万元素的 vector 排序的耗时");
    println("2. 用 normal_distribution 模拟身高分布 (均值170cm, 标准差6cm) 生成1000个样本");
    println("3. 实现一个简单的游戏暴击系统: 每次攻击 20% 概率造成 2 倍伤害");
    println("4. 用 random_device + mt19937 实现一个随机密码生成器");
    println("5. 为什么 steady_clock 适合测量时间间隔而 system_clock 不适合?");
}

} // namespace stl_learn

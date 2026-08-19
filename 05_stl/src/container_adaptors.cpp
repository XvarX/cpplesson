// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  src/container_adaptors.cpp — 容器适配器 教学演示函数实现                     ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

#include "stl/container_adaptors.hpp"

using namespace std;

namespace stl_learn {

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 1: std::stack — 栈 (LIFO: 后进先出)                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part1_stack() {
    lesson::print_subtitle("Part 1: std::stack — 栈 LIFO");

    // 默认底层容器是 deque; 也可指定 vector 或 list
    stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);

    println("栈顶: {}", s.top());  // 预期: 3
    println("栈大小: {}", s.size()); // 预期: 3

    print("出栈顺序: ");
    while (!s.empty()) {
        print("{} ", s.top());
        s.pop();
    }
    println("");
    // 预期: 3 2 1

    // 指定底层容器为 vector
    stack<int, vector<int>> vec_stack;
    vec_stack.push(10);
    println("基于 vector 的栈顶: {}", vec_stack.top());
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 2: std::queue — 队列 (FIFO: 先进先出)                                  ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part2_queue() {
    lesson::print_subtitle("Part 2: std::queue — 队列 FIFO");

    // 默认底层容器是 deque
    queue<string> q;
    q.push("任务1");
    q.push("任务2");
    q.push("任务3");

    println("队首: {}", q.front());  // 预期: 任务1
    println("队尾: {}", q.back());   // 预期: 任务3
    println("队列大小: {}", q.size()); // 预期: 3

    print("出队顺序: ");
    while (!q.empty()) {
        print("{} ", q.front());
        q.pop();
    }
    println("");
    // 预期: 任务1 任务2 任务3

    // 注意: queue 不能用 vector 作为底层容器 (vector 没有 pop_front)
    // queue<int, vector<int>> bad;  // 编译错误!
    queue<int, list<int>> list_queue;  // OK, list 有 push_back 和 pop_front
    list_queue.push(1);
    println("基于 list 的队首: {}", list_queue.front());
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 3: std::priority_queue — 优先队列 (最大/最小堆)                        ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part3_priority_queue() {
    lesson::print_subtitle("Part 3: priority_queue — 优先队列 (堆)");

    // 默认: 最大堆 (大顶堆) — 最大的在 top
    priority_queue<int> pq;
    pq.push(3);
    pq.push(1);
    pq.push(5);
    pq.push(2);

    println("top (最大): {}", pq.top());  // 预期: 5

    print("出队顺序 (降序): ");
    while (!pq.empty()) {
        print("{} ", pq.top());
        pq.pop();
    }
    println("");
    // 预期: 5 3 2 1

    // 最小堆: 使用 greater<int> 作为比较器
    // 注意: 需要显式指定底层容器类型: vector<int>
    priority_queue<int, vector<int>, greater<int>> min_pq;
    min_pq.push(3);
    min_pq.push(1);
    min_pq.push(5);
    min_pq.push(2);

    println("最小堆 top: {}", min_pq.top());  // 预期: 1

    print("最小堆出队顺序 (升序): ");
    while (!min_pq.empty()) {
        print("{} ", min_pq.top());
        min_pq.pop();
    }
    println("");
    // 预期: 1 2 3 5
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 4: 优先队列 — 自定义比较器                                            ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part4_custom_pq() {
    lesson::print_subtitle("Part 4: 优先队列 — 自定义比较器");

    // 自定义比较器: 优先级高的在 top
    auto cmp = [](const Task& a, const Task& b) {
        return a.priority < b.priority;  // 注意: 返回 true 表示 a 在 b 之后
        // 等价于: priority_queue 默认用 less<T> 就是最大堆
        // 若 a.priority < b.priority 返回 true，则 b 排在 a 前面 (b 的优先级更高)
    };

    priority_queue<Task, vector<Task>, decltype(cmp)> task_pq(cmp);
    task_pq.push({"修Bug", 3});
    task_pq.push({"写文档", 1});
    task_pq.push({"新功能", 5});
    task_pq.push({"代码审查", 2});

    println("按优先级处理任务:");
    while (!task_pq.empty()) {
        auto t = task_pq.top();
        task_pq.pop();
        println("  [优先级{}] {}", t.priority, t.name);
    }
    // 预期: 新功能(5), 修Bug(3), 代码审查(2), 写文档(1)

    // C++20 起可以用 lambda 作为非类型模板参数 (更简洁)
    // 但需要 C++20 constexpr lambda
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  Part 5: 适配器模式总结                                                      ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void part5_summary() {
    lesson::print_subtitle("Part 5: 适配器模式总结");

    println("┌──────────────────┬──────────────┬─────────────────────────┬──────────────────┐");
    println("│      适配器       │   访问语义    │      默认底层容器        │     典型应用      │");
    println("├──────────────────┼──────────────┼─────────────────────────┼──────────────────┤");
    println("│  stack            │  LIFO         │  deque                  │  撤销/DFS/括号   │");
    println("│  queue            │  FIFO         │  deque                  │  调度/BFS        │");
    println("│  priority_queue   │  优先出队      │  vector                 │  Top-K/Dijkstra  │");
    println("└──────────────────┴──────────────┴─────────────────────────┴──────────────────┘");

    println("\n适配器模式的核心思想:");
    println("  1. 包装底层容器，只暴露特定接口");
    println("  2. 接口即文档 — 看类型就知道用法");
    println("  3. 防止误用 — 不会有人对 stack 做 sort");
    println("  4. 底层容器可替换 — deque/vector/list");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  常见陷阱                                                                   ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void container_adaptors_pitfalls() {
    lesson::print_subtitle("常见陷阱");

    // 陷阱1: 适配器不支持迭代器遍历
    stack<int> s;
    s.push(1);
    // for (auto x : s) {}  // 编译错误! stack 没有 begin()/end()
    lesson::print_note("stack/queue/priority_queue 不支持迭代器，不能直接遍历");

    // 陷阱2: pop() 不返回值 (为了异常安全)
    // int x = s.pop();  // 编译错误! pop() 返回 void
    // 正确做法: x = s.top(); s.pop();
    lesson::print_note("pop() 不返回值 — 先 access(top/front)，再 pop()");

    // 陷阱3: priority_queue 的比较器是"小于"语义
    // greater<int> 产生最小堆 (容易搞混!)
    lesson::print_note("priority_queue 的比较器: less<T> => 最大堆, greater<T> => 最小堆");
}

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║  练习                                                                       ║
// ╚══════════════════════════════════════════════════════════════════════════════╝
void container_adaptors_exercises() {
    lesson::print_subtitle("练习");

    println("1. 用两个 stack 实现一个 queue (FIFO)");
    println("2. 用 priority_queue 实现一个 Top-K 查找器 (维护最大的 K 个元素)");
    println("3. 用 stack 实现中缀表达式转后缀表达式（逆波兰表达式）");
    println("4. 为什么 priority_queue 的默认底层容器是 vector 而不是 deque?");
    println("5. 实现一个支持迭代器的简单 Stack 类（练习适配器模式）");
}

} // namespace stl_learn

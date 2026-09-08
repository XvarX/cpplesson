#pragma once

#include <print>
#include <string>
#include <string_view>
#include <utility>

class Person {
public:
    Person() : name_("未知"), age_(0) {}
    
    Person(std::string name, int age)
        : name_(std::move(name)), age_(age) {}

    void introduce() const {
        std::println("我叫{}, 今年{}岁", name_, age_);
    }

private:
    std::string name_;
    int age_;
};

class Config {
public:
    Config(std::string path, int port)
        : path_(std::move(path))
        , port_(port) {
        
    }

    void show() const {
        std::println("配置路径: {}, 端口: {}", path_, port_);
    }

private:
    std::string path_;
    int port_;
};

class MyRectangle {
public:
    MyRectangle(double side)
        : MyRectangle(side, side) {}
    
    MyRectangle(double w, double h) : width_(w), height_(h) {
    
    }

    double area() const {
        return width_ * height_;
    }

private:
    double width_, height_;
};

class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;

};

class Trivial {
public:
    Trivial() = default;
    int x = 0;
};

class Duration {
public:
    explicit Duration(int seconds)
        : seconds_(seconds) {}
    
    int total_seconds() const {
        return seconds_;
    }
private:
    int seconds_;
};

class Logger {
public:
    explicit Logger(std::string name) : name_(std::move(name)) {
        std::println("[{}] 构造 - 打开日志文件", name_);
    }

    ~Logger() {
        std::println("[{}] 析构 - 关闭日志文件", name_);
    }

    std::string_view name() const {
        return name_;
    }

private:
    std::string name_;
};

class Vector3D {
public:

    Vector3D(int i): Vector3D(i,i,i) {

    }
    Vector3D(int x, int y, int z) : x_(x), y_(y), z_(z) {

    }
private:
    int x_;
    int y_;
    int z_;
};

class DatabaseConnection{
    DatabaseConnection() {
        std::println("DatabaseConnection");
    }

    ~DatabaseConnection() {
        std::println("~DatabaseConnection");
    }
};

class UniquePtr {
    UniquePtr() = default;

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&&) = default;
    UniquePtr& operator=(UniquePtr&&) = default;

};
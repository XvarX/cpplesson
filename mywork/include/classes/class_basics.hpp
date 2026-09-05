#pragma once

#include <print>
#include <string>
#include <string_view>
#include <utility>

struct Point {
    double x, y;
};

class Circle {
    double radius_;
public:
    void set_radius(double r) {
        radius_ = r;
    }

    double area() const {
        return 3.14159 * radius_ * radius_;
    }

    double area() const {
        return 3.14159 * radius_ * radius_;
    }
};

class BankAccount {
public:
    BankAccount(std::string owner, double initial)
        : owner_(std::move(owner)), balance_(initial) {}
    
    void deposit(double amount) {
        if (amount > 0) balance_ += amount;
    }

    bool withdraw(double amount) {
        if (amount > 0 && amount <= balance_) {
            balance_ -= amount;
            return true;
        } else {
            return false;
        }
    }

    double balance() const {
        return balance_;
    }


private:
    std::string owner_;
    double balance_;

    void log_transaction(const std::string& type, double amount) {

    }
};

class Counter {
public:
    Counter& reset() { count_ = 0; return *this; }
    Counter& increment() { ++count_; return *this; }
    Counter& add(int n) { count_ += n; return *this; }

    Counter& set_count(int count) {
        this->count_ = count;
        return *this;
    }

    int value() const {
        return count_;
    }

private:
    int count_ = 0;

};

class Student {
public:
    Student(std::string name) : name_(std::move(name)) {
        ++total_count_;
    }
    ~Student() { --total_count_; }

    static int total() { return total_count_; }
    std::string_view name() const { return name_; }

private:
    std::string name_;
    static inline int total_count_ = 0;
};

class Temperature {
public:
    explicit Temperature(double celsius) : celsius_(celsius) {}

    double get_celsius() const { return celsius_; }
    double get_fahrenheit() const { return celsius_ * 9 / 5 + 32; }

    void set_celsius(double v) { celsius_ = v; }

    mutable int read_count = 0;
    double cached_fahrenheit() const {
        ++read_count;
        return get_fahrenheit();
    }


private:
    double celsius_;
};
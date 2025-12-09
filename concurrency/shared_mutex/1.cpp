#include <shared_mutex>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>

class ThreadSafeData {
public:
    int get() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return value_;
    }

    void set(int x) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        value_ = x;
    }

private:
    mutable std::shared_mutex mutex_;
    int value_ = 0;
};

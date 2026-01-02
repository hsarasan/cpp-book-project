#include <atomic>
#include <cstddef>
#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <x86intrin.h> // for rdtsc

template <typename T, size_t N>
struct SPSCQ {
    static_assert((N & (N-1)) == 0, "N must be power of two");

    bool push(const T& data) {
        auto h = head.load(std::memory_order_relaxed);
        auto next = (h + 1) & (N - 1);
        if (next == tail.load(std::memory_order_acquire))
            return false;
        data_[h] = data;
        head.store(next, std::memory_order_release);
        return true;
    }

    bool pop(T& data) {
        auto t = tail.load(std::memory_order_relaxed);
        if (t == head.load(std::memory_order_acquire))
            return false;
        data = data_[t];
        tail.store((t + 1) & (N - 1), std::memory_order_release);
        return true;
    }

private:
    alignas(64) std::atomic<int> head{0};
    alignas(64) std::atomic<int> tail{0};
    T data_[N];
};

template <typename T, size_t N>
struct SPSCQ_SC {
    static_assert((N & (N-1)) == 0, "N must be power of two");

    bool push(const T& data) {
        auto h = head.load();
        auto next = (h + 1) & (N - 1);
        if (next == tail.load())
            return false;
        data_[h] = data;
        head.store(next);
        return true;
    }

    bool pop(T& data) {
        auto t = tail.load();
        if (t == head.load())
            return false;
        data = data_[t];
        tail.store((t + 1) & (N - 1));
        return true;
    }

private:
    alignas(64) std::atomic<int> head{0};
    alignas(64) std::atomic<int> tail{0};
    T data_[N];
};

// Helper to pin thread to a CPU core
void pin_thread(int core) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

// SPSC latency measurement in CPU cycles
template <template<typename,size_t> class Queue>
void measure_latency(size_t count = 1'000'000) {
    Queue<int, 1024> q;
    volatile int sink = 0;
    std::vector<uint64_t> latencies;
    latencies.reserve(count);

    std::thread producer([&]{
        pin_thread(0);
        for(size_t i = 0; i < count; ++i){
            while(!q.push(static_cast<int>(i))) {}
        }
    });

    std::thread consumer([&]{
        pin_thread(1);
        int val;
        for(size_t i = 0; i < count; ++i){
            uint64_t start, end;
            do {
                start = __rdtsc();
            } while(!q.pop(val));
            end = __rdtsc();
            latencies.push_back(end - start);
            sink = val;
        }
    });

    producer.join();
    consumer.join();

    // Report statistics
    std::sort(latencies.begin(), latencies.end());
    auto total_cycles = std::accumulate(latencies.begin(), latencies.end(), uint64_t(0));
    std::cout << "Queue type: " << typeid(Queue<int,1024>).name() << "\n";
    std::cout << "Count: " << count << "\n";
    std::cout << "Average latency (cycles): " << total_cycles / double(count) << "\n";
    std::cout << "Min latency: " << latencies.front() << "\n";
    std::cout << "Max latency: " << latencies.back() << "\n";
    std::cout << "Median latency: " << latencies[latencies.size()/2] << "\n\n";
}

int main() {
    size_t count = 1'000'000;

    measure_latency<SPSCQ>(count);
    measure_latency<SPSCQ_SC>(count);
}

#include <atomic>
#include <type_traits>

template <typename T, size_t Capacity>
struct SPSCQ {

    static_assert((Capacity & (Capacity - 1)) == 0,
                  "Capacity must be power of two");
    static_assert(std::is_trivially_copyable_v<T>,
                  "T must be trivially copyable");

    [[nodiscard]] bool push(const T& data) noexcept {
        auto h = head.load(std::memory_order_relaxed);
        auto new_h = increment(h);

        if (new_h == tail.load(std::memory_order_acquire))
            return false;

        buffer[h] = data;
        head.store(new_h, std::memory_order_release);
        return true;
    }

    [[nodiscard]] bool pop(T& data) noexcept {
        auto t = tail.load(std::memory_order_relaxed);

        if (t == head.load(std::memory_order_acquire))
            return false;

        data = buffer[t];
        tail.store(increment(t), std::memory_order_release);
        return true;
    }

private:
    static constexpr size_t increment(size_t v) noexcept {
        return (v + 1) & (Capacity - 1);
    }

    alignas(64) std::atomic<size_t> head{0};
    alignas(64) std::atomic<size_t> tail{0};
    T buffer[Capacity];
};

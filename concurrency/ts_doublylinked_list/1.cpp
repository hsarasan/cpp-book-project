#include <memory>
#include <mutex>
#include <optional>

template <typename T>
class ConcurrentList {

    struct Node {
        T data;
        std::shared_ptr<Node> next;
        std::weak_ptr<Node> prev;
        std::mutex m;

        Node(T d) : data(std::move(d)), next(nullptr) {}
    };

public:
    ConcurrentList() = default;
    ConcurrentList(const ConcurrentList&) = delete;
    ConcurrentList& operator=(const ConcurrentList&) = delete;

    // ---------------------------------------------------------
    // PUSH FRONT
    // ---------------------------------------------------------
    void push_front(T value) {
        auto n = std::make_shared<Node>(std::move(value));

        std::lock_guard<std::mutex> lg(head_m);

        if (!head) {
            head = tail = n;
            return;
        }

        std::lock_guard<std::mutex> lg2(head->m);
        head->prev = n;
        n->next = head;
        head = n;
    }

    // ---------------------------------------------------------
    // PUSH BACK
    // ---------------------------------------------------------
    void push_back(T value) {
        auto n = std::make_shared<Node>(std::move(value));

        std::lock_guard<std::mutex> lg(tail_m);

        if (!tail) {
            head = tail = n;
            return;
        }

        std::lock_guard<std::mutex> lg2(tail->m);
        tail->next = n;
        n->prev = tail;
        tail = n;
    }

    // ---------------------------------------------------------
    // POP FRONT
    // ---------------------------------------------------------
    std::optional<T> pop_front() {
        std::lock_guard<std::mutex> lg(head_m);

        if (!head) return std::nullopt;

        auto old = head;
        std::optional<T> out(std::move(old->data));

        if (head == tail) {
            head = tail = nullptr;
            return out;
        }

        std::lock_guard<std::mutex> lg2(old->next->m);

        old->next->prev.reset();
        head = old->next;

        return out;
    }

    // ---------------------------------------------------------
    // POP BACK
    // ---------------------------------------------------------
    std::optional<T> pop_back() {
        std::lock_guard<std::mutex> lg(tail_m);

        if (!tail) return std::nullopt;

        auto old = tail;
        std::optional<T> out(std::move(old->data));

        if (head == tail) {
            head = tail = nullptr;
            return out;
        }

        auto prev = old->prev.lock();
        std::lock_guard<std::mutex> lg2(prev->m);

        prev->next.reset();
        tail = prev;

        return out;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lg_h(head_m);
        return head == nullptr;
    }

private:
    std::shared_ptr<Node> head{nullptr};
    std::shared_ptr<Node> tail{nullptr};

    mutable std::mutex head_m;
    mutable std::mutex tail_m;
};

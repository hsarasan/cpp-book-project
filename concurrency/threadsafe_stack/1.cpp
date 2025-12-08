template <typename T>
class ThreadSafeStack {

public:
    ThreadSafeStack() = default;
    ThreadSafeStack(const ThreadSafeStack&) = delete;
    ThreadSafeStack& operator=(const ThreadSafeStack&) = delete;

    void push(T value) {
        lock_guard<mutex> lg(m);
        S.push(std::move(value));
    }

    void pop(T& value) {
        lock_guard<mutex> lg(m);
        if (S.empty()) throw runtime_error("Stack Empty");
        value = std::move(S.top());
        S.pop();
    }

    shared_ptr<T> pop() {
        lock_guard<mutex> lg(m);
        if (S.empty()) throw runtime_error("Stack Empty");
        auto ptr = make_shared<T>(std::move(S.top()));
        S.pop();
        return ptr;
    }

    bool empty() const {
        lock_guard<mutex> lg(m);
        return S.empty();
    }

private:
    mutable mutex m;
    stack<T> S;
};

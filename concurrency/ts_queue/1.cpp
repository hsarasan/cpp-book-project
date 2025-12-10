#include <shared_mutex>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>

template <typename T>
class ts_Q{
    public:
        ts_Q() = default;
        ts_Q(const ts_Q &) = delete;
        ts_Q& operator-(const ts_Q&) = delete;

        void push(T value){
            std::lock_guard<std::mutex> lg(m);
            Q.push(value);
            cv.notify_one();
        }

        bool try_and_pop(T& value){
            std::lock_guard<std::mutex> lg(m);
            if (Q.empty())
                return false;
            value = std::move(Q.front());
            Q.pop();
            return true;
        }

        std::shared_ptr<T> try_and_pop(){
            std::lock_guard<std::mutex> lg(m);
            if (Q.empty())
                return nullptr;
            auto x(std::make_shared<T>(Q.front()));
            Q.pop();
            return x;
        }

        void wait_and_pop(T& value){
            std::unique_lock<std::mutex> ul(m);
            cv.wait(ul, [&](){ return !Q.empty();});
            value = std::move(Q.front());
            Q.pop();
        }

        std::shared_ptr<T> wait_and_pop(){
            std::unique_lock<std::mutex> ul(m);
            cv.wait(ul, [&](){ return !Q.empty();});
            auto x(std::make_shared<T>(Q.front()));
            Q.pop();
            return x;

        }

        bool empty() const{
            std::lock_guard<std::mutex> lg(m);
            return Q.empty();
        }
    private:
        std::condition_variable cv;
        mutable std::mutex m;
        std::queue<T> Q;
};
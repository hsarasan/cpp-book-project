#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

template <typename T, size_t N>
class BoundedQ{

    public:

        BoundedQ() = default;
        BoundedQ(const BoundedQ&) = delete;
        BoundedQ & operator=(const BoundedQ&) = delete;

        void wait_and_push(const T& value){
            std::unique_lock<std::mutex> ul(m);
            not_full.wait(ul, [&](){return Q.size()<N;});
            Q.push(value);
            not_empty.notify_one();
        }
        void wait_and_push(T&& value){
            std::unique_lock<std::mutex> ul(m);
            not_full.wait(ul, [&](){return Q.size()<N;});
            Q.push(std::move(value));
            not_empty.notify_one();
        }
        void wait_and_pop(T& value){
            std::unique_lock<std::mutex> ul(m);
            not_empty.wait(ul,[&](){return !Q.empty();});
            value = std::move(Q.front());
            Q.pop();
            not_full.notify_one();
        }
        std::shared_ptr<T> wait_and_pop(){
            std::unique_lock<std::mutex> ul(m);
            not_empty.wait(ul,[&](){return !Q.empty();});
            std::shared_ptr<T> res = std::make_shared<T>(std::move(Q.front()));
            Q.pop();
            not_full.notify_one();
            return res;
        }

        bool empty() const{
            std::lock_guard<std::mutex> lg(m);
            return Q.empty();
        }

    private:
        std::condition_variable not_full;
        std::condition_variable not_empty;
        std::queue<T> Q;
        mutable std::mutex m;
};

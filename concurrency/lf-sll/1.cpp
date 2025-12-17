#include <atomic>
#include <thread>

template <typename T>
struct SLList{
    struct Node{
        T data;
        Node * next;
        Node(const T &t):data(t),next(nullptr){}
    };
    SLList() = default;
    SLList(const SLList &) = delete;
    SLList & operator=(const SLList &) = delete;
    void push_front(T &t){
        auto n = new Node(t);
        n->next = head.load(memory_order_relaxed);
        while(!head.compare_exchange_weak(n->next,n
        , std::memory_order_release
        , std::memory_order_relaxed
    )){}
    }

    void pop(){
        auto old_head = head.load(std::memory_order_relaxed);
        while (old_head && !head.compare_exchange_weak(
            old_head,
            head->next,
            memory_order_release,
            memory_order_relaxed
        )){}
    }

    private:
        std::atomic<Node*> head{nullptr};

};
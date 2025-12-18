#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <functional>
#include <shared_mutex>

using namespace std;

template <typename T>
class ConcurrentLinkedList{
    public:
    struct Node{
        T data;
        std::shared_ptr<Node> next;
        std::shared_mutex m;
        Node(const T& data):data(data),next(nullptr){}
        Node(const Node& ) = delete;
        Node & operator=( const Node&) = delete;
    };
    ConcurrentLinkedList() = default;
    ConcurrentLinkedList( const ConcurrentLinkedList&) = delete;
    ConcurrentLinkedList & operator=(const ConcurrentLinkedList&) = delete;
    void push(const T& data){
        auto n = std::make_shared<Node>(data);
        std::lock_guard lock(m);
        n->next = head;
        head = n;
    }
    bool pop(T &value){
        std::lock_guard lock(m);
        if (!head) return false;
        value = std::move(head->data);
        head = head->next;
        return true;
    }

    private:
        std::shared_ptr<Node> head{nullptr};
        std::mutex m;
};
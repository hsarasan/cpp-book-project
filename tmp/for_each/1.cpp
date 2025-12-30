#include <iostream>
#include <functional>

template <typename Iter, typename Callable, typename... Args>
void my_for_each(Iter begin, Iter end, Callable&& func, Args&&... args){
    auto current = begin;
    while (current != end){
        auto item = *current;
        std::invoke(std::forward<Callable>(func), std::forward<Args>(args)..., item);
        current++;
    }
}


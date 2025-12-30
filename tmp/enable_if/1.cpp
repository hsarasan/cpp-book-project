#include <iostream>


template <typename T>
using ResultType = decltype(std::declval<T>() + 0.1);

template <typename T>
std::enable_if_t<std::is_integral_v<T>, ResultType<T> >
add(T t){
    return t + 0.1;
} 

int main(){
    std::cout<<add(1)<<std::endl;
}
#include <iostream>
#include <string>
#include <algorithm>
#include <ranges>
#include <iterator>
#include <vector>

template <typename T>
constexpr const char* filler = " ";  

template <>
constexpr const char* filler<int> = ","; 

template <>
constexpr const char* filler<std::string> = "||"; 


template <typename Container>
void printC(const Container &c) {
    using T = typename Container::value_type;
    std::ranges::copy(c,std::ostream_iterator<T>(std::cout, filler<T>));
    std::cout << "\n";
}

int main() {
    printC(std::vector<int>{1, 2, 3, 4, 5});
    printC(std::vector<char>{'a','b','c'});
    printC(std::vector<std::string>{"one", "two", "three"});
    return 0;
}

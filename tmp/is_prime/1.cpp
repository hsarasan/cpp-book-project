#include <iostream>
#include <type_traits>

template <int N, int i>
struct is_divisible{

    static constexpr bool value = (N%i>0)? is_divisible<N,i-1>::value : true;

};

template <int N>
struct is_divisible<N,1>{

    static constexpr bool value = false;

};

template <int N>
struct is_prime{
    static constexpr bool value = !is_divisible<N,N/2>::value;
};
template <>
struct is_prime<0>{
    static constexpr bool value = false;
};
template <>
struct is_prime<1>{
    static constexpr bool value = false;
};
template <>
struct is_prime<2>{
    static constexpr bool value = true;
};

template <int N>
constexpr bool is_prime_v = is_prime<N>::value;

int main(){
    std::cout<<is_prime_v<1> <<std::endl;
    std::cout<<is_prime_v<2> <<std::endl;
    std::cout<<is_prime_v<3> <<std::endl;
    std::cout<<is_prime_v<4> <<std::endl;
    std::cout<<is_prime_v<5> <<std::endl;
    std::cout<<is_prime_v<6> <<std::endl;
    std::cout<<is_prime_v<7> <<std::endl;
    std::cout<<is_prime_v<8> <<std::endl;
    std::cout<<is_prime_v<9> <<std::endl;
    std::cout<<is_prime_v<10> <<std::endl;

    
}
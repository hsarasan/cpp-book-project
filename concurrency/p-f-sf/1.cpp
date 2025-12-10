#include <shared_mutex>
#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>


int main(){

   std::promise<int> p;
   std::future<int> f = p.get_future();
   std::shared_future<int> s = f.share();

   auto producer = [&](){
        p.set_value(100);
   };
   auto consumer = [s](int v){
        auto s1 = s;
        return s1.get() * v;
   };

   producer();
   int a1 = consumer(2);
   int a2 = consumer(4);
   int a3 = consumer(10);

   std::cout<<a1<<" "<<a2<<" "<<a3<<std::endl;

    return 0;
}
#include <iostream>
#include <jthread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;

int main(){
    vector<std::jthread> threads;
    for (int i=0; i<std::thread::hardware_concurrency(); ++i)
        threads.emplace_back([](){
            cout<<"Entering "<<this_thread::get_id()<<endl;
        });
    return 0;
}
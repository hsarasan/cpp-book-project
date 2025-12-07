#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;

int main(){
    vector<std::thread> threads;
    for (int i=0; i<std::thread::hardware_concurrency(); ++i)
        threads.emplace_back([](){
            cout<<"Entering "<<this_thread::get_id()<<endl;
        });
    for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
    return 0;
}
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void f(){
    cout<<"in thread "<<this_thread::get_id()<<endl;
    this_thread::sleep_for(std::chrono::seconds(4));
    cout<<"Leaving "<<endl;
}

class RAIIThread {
public:
    explicit RAIIThread(std::thread&& temp) : t(std::move(temp)) {}
    ~RAIIThread() {
        if (t.joinable()) t.join();
    }

    RAIIThread(const RAIIThread&) = delete;
    RAIIThread& operator=(const RAIIThread&) = delete;

    std::thread::id get_id() const { return t.get_id(); }

private:
    std::thread t;
};

int main(){
    std::thread tid(f);
    RAIIThread r(std::move(tid));
    return 0;
}

#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;


void accumulate_with_threads_without_false_sharing(const vector<int>& v) {
    int n_threads = std::thread::hardware_concurrency();
    if (n_threads == 0) n_threads = 1;

    vector<long long> partial(n_threads, 0);
    vector<thread> threads;
    threads.reserve(n_threads);

    size_t n = v.size();
    size_t chunk = n / n_threads;
    size_t rem = n % n_threads;

    size_t start = 0;

    for (int i = 0; i < n_threads; i++) {
        size_t end = start + chunk + (i < rem ? 1 : 0);

        threads.emplace_back([&, i, start, end] {
            long long sum = 0;

            // local accumulator (avoids false sharing)
            for (size_t j = start; j < end; j++)
                sum += v[j];

            partial[i] = sum;
        });

        start = end;
    }

    for (auto& t : threads)
        t.join();

    long long total = std::accumulate(partial.begin(), partial.end(), 0LL);

    cout << "Total sum = " << total << "\n";
}

void accumulate_with_threads(const vector<int>& v) {
    int n_threads = std::thread::hardware_concurrency();
    if (n_threads == 0) n_threads = 1;

    vector<long long> partial(n_threads, 0);
    vector<thread> threads;
    threads.reserve(n_threads);

    size_t n = v.size();
    size_t chunk = n / n_threads;
    size_t rem = n % n_threads;

    size_t start = 0;

    for (int i = 0; i < n_threads; i++) {
        size_t end = start + chunk + (i < rem ? 1 : 0);

        threads.emplace_back([&, i, start, end] {
            partial[i] = std::accumulate(v.begin()+start, v.begin()+end, 0LL);
        });

        start = end;
    }

    for (auto& t : threads)
        t.join();

    long long total = std::accumulate(partial.begin(), partial.end(), 0LL);

    cout << "Total sum = " << total << "\n";
}


void accumulate_standalone(const vector<int>& v){
    auto result = accumulate(v.begin(), v.end(), 0LL);
    cout<<"Result : "<<result<<endl;
}


template <typename Func, typename... Args>
auto execute_with_time(Func&& f, Args&&... args){
    timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    
    f(std::forward<Args>(args)...);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    long ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    return ns;
}

int main(){
    vector<int> v(1000000);
    iota(v.begin(), v.end(), 0);
    auto n1 = execute_with_time(accumulate_standalone, v);
    auto n2 = execute_with_time(accumulate_with_threads, v);
    auto n3 = execute_with_time(accumulate_with_threads_without_false_sharing, v);
    cout<<n1<<" "<<n2<<" "<<n3<<endl;
    cout<<"Speedup factor "<<(float)n2/n1<< " "<<(float)n3/n1<<endl;
    return 0;
}
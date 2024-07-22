#ifndef UTILS_HPP
#define UTILS_HPP

#include "iostream"
#include "chrono"

using namespace std;
using namespace chrono;

template <typename Func, typename... Args>
auto timingdecorator(Func func, Args... args) -> typename enable_if<is_void<decltype(func(args...))>::value>::type
{
    auto start = high_resolution_clock::now();
    func(args...);
    auto end = high_resolution_clock::now();
    duration<double, milli> elapsed = end - start;
    cout << "Elapsed time: " << elapsed.count() << " ms\n";
}

#endif
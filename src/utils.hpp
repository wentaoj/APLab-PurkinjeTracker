#include <chrono>
#include <iostream>

template <typename Func, typename... Args>
auto timing_decorator(const std::string &label, Func func, Args &&...args)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << label << " took " << duration.count() << " seconds." << std::endl;
    return result;
}
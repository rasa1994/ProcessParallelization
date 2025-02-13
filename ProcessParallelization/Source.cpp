// ProcessParallelization.cpp : Defines the entry point for the application.
//

import ProcessParallelization;
#include <any>
#include <concepts>
#include <array>
#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <ranges>
#include <functional>
#include <mutex>
#include <atomic>
#include <chrono>
#include <future>
#include <numeric>
#include <type_traits>


int main()
{
    ThreadPool pool{};
    std::vector<int> vec;

    std::ranges::copy(std::ranges::views::iota(1, 1'000'001), std::back_inserter(vec));

    const auto leftSum = [&vec]
    {
        auto itMid = std::begin(vec);
        std::advance(itMid, vec.size() / 2);
        return std::accumulate(std::begin(vec),itMid , 0ull);
    };

    const auto rightSum = [&vec]
    {
        auto itMid = std::begin(vec);
        std::advance(itMid, vec.size() / 2);
        return std::accumulate(itMid,std::end(vec) , 0ull);
    };

    std::vector<std::future<std::any>> futures;
    futures.push_back((pool.RegisterWork(leftSum)));
    futures.push_back((pool.RegisterWork(rightSum)));

    unsigned long long sum = 0;
    for (auto& s : futures)
        sum += std::any_cast<unsigned long long >(s.get());

    std::cout << sum << std::endl;
}

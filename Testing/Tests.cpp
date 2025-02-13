import ProcessParallelization;
#include "gtest/gtest.h"
#include <ranges>
#include <algorithm>
#include <numeric>
#include <vector>
#include <future>


TEST(Tests, Test1)
{
    ThreadPool pool{};
    std::vector<int> vec;

    std::ranges::copy(std::ranges::views::iota(1, 1'000'001), std::back_inserter(vec));

    const auto leftSum = [&vec]
    {
        auto itMid = std::begin(vec);
        std::advance(itMid, vec.size() / 2);
        return std::accumulate(std::begin(vec), itMid, 0ull);
    };

    const auto rightSum = [&vec]
    {
        auto itMid = std::begin(vec);
        std::advance(itMid, vec.size() / 2);
        return std::accumulate(itMid, std::end(vec), 0ull);
    };

    std::vector<std::future<std::any>> futures;
    futures.push_back((pool.RegisterWork(leftSum)));
    futures.push_back((pool.RegisterWork(rightSum)));

    unsigned long long sum = 0;
    for (auto& s : futures)
        sum += std::any_cast<unsigned long long>(s.get());

    EXPECT_EQ(sum, 500000500000);
}

TEST(Tests, Test2)
{
    std::vector<int> vec;
    std::ranges::copy(std::ranges::views::iota(1, 1'000'001), std::back_inserter(vec));

    std::atomic_llong sum{ 0 };
    Parallel_For(vec, [&sum](const int& a) 
    { 
        sum += a;
    });

    EXPECT_EQ(sum, 500000500000);
}

TEST(Tests, Test3)
{
    // without lock test
    std::vector<int> vec;
    std::ranges::copy(std::ranges::views::iota(1, 1'000'001), std::back_inserter(vec));

    long long sum{ 0 };
    Parallel_For(vec, [&sum](const int& a) 
    { 
        sum += a;
    });

    EXPECT_LT(sum, 500000500000);
}
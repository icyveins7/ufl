#include "upfirlerp.h"
#include <cstdlib>

#include <catch2/catch_test_macros.hpp>
// Also include benchmarking headers, i don't really know which one is necessary
#include <catch2/benchmark/catch_benchmark.hpp>

TEST_CASE("benchmark doubles, taps length 100, input len 10000, output len 20000", "[interpolate],[double]")
{
    std::vector<std::complex<double>> input(10000);
    for (auto& v : input)
        v = std::complex<double>(std::rand() / (double)RAND_MAX, std::rand() / (double)RAND_MAX);

    std::vector<double> taps(100);
    for (auto& v : taps)
    {
        v = std::rand() / (double)RAND_MAX;
    }

    ufl::UpfirLerp<double> upfirlerp;
    upfirlerp.set_up_taps(taps);

    double T = 0.01;
    std::vector<double> t(20000);
    for (int i = 0; i < t.size(); ++i)
        t.at(i) = i * T/100;

    std::vector<std::complex<double>> out(20000);

    BENCHMARK("up 10, 1 thread")
    {
        upfirlerp.set_up_rate(10).set_threads(1);

        upfirlerp.interpolate(
            input, T, t, out
        );
    };


    // This should be around half the time as the previous one,
    // since for a doubling of upsample rate,
    // the number of multiplications using the taps is halved;
    // the number of non-zero indices references is roughly halved
    BENCHMARK("up 20, 1 thread")
    {
        upfirlerp.set_up_rate(20).set_threads(1);

        upfirlerp.interpolate(
            input, T, t, out
        );
    };

    BENCHMARK("up 10, 4 threads")
    {
        upfirlerp.set_up_rate(10).set_threads(4);

        upfirlerp.interpolate(
            input, T, t, out
        );

    };


    BENCHMARK("up 20, 4 threads")
    {
        upfirlerp.set_up_rate(20).set_threads(4);

        upfirlerp.interpolate(
            input, T, t, out
        );

    };
}



TEST_CASE("benchmark doubles, taps length 100, input len 100000, output len 200000", "[interpolate],[double]")
{
    std::vector<std::complex<double>> input(100000);
    for (auto& v : input)
        v = std::complex<double>(std::rand() / (double)RAND_MAX, std::rand() / (double)RAND_MAX);

    std::vector<double> taps(100);
    for (auto& v : taps)
    {
        v = std::rand() / (double)RAND_MAX;
    }

    ufl::UpfirLerp<double> upfirlerp;
    upfirlerp.set_up_taps(taps);

    double T = 0.01;
    std::vector<double> t(200000);
    for (int i = 0; i < t.size(); ++i)
        t.at(i) = i * T/100;

    std::vector<std::complex<double>> out(200000);

    BENCHMARK("up 10, 1 thread")
    {
        upfirlerp.set_up_rate(10).set_threads(1);

        upfirlerp.interpolate(
            input, T, t, out
        );
    };

    BENCHMARK("up 20, 1 thread")
    {
        upfirlerp.set_up_rate(20).set_threads(1);

        upfirlerp.interpolate(
            input, T, t, out
        );
    };

    BENCHMARK("up 10, 4 threads")
    {
        upfirlerp.set_up_rate(10).set_threads(4);

        upfirlerp.interpolate(
            input, T, t, out
        );

    };

    BENCHMARK("up 20, 4 threads")
    {
        upfirlerp.set_up_rate(20).set_threads(4);

        upfirlerp.interpolate(
            input, T, t, out
        );

    };
}

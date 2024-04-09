#include "upfirlerp.h"
#include <vector>
#include <complex>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("verification against numpy+scipy", "[upfirlerp],[python]")
{
    // This aims to mirror test.py
    // Defined from test.py
    std::vector<double> taps = {
        0.003560172702628888,0.03808372279569934,0.1610318523331153,0.2973242521685565,0.2973242521685565,0.1610318523331153,0.03808372279569934,0.003560172702628888
    };

    std::vector<std::complex<double>> input = {
        {1.0, 0.0},
        {0.0, 1.0},
        {0.0, -1.0},
        {-1.0, 0.0},
        {0.0, -1.0}
    };

    // Define expected result as seen from the script
    std::vector<std::complex<double>> expected = {
        {0.0, 0.0},
        {0.01736959273985707, 0.0},
        {0.18829033230020353, 0.03117901277708525},
        {-0.24280729223438002, -0.01736959273985707},
        {0.0, 0.0}
    };

    double T = 0.01;
    std::vector<double> t = {
        -0.0001, 0.001, 0.012, 0.041, 0.05
    };


    SECTION("double")
    {
        ufl::UpfirLerp<double> upfirlerp;

        upfirlerp.set_threads(1).set_up_rate(4).set_up_taps(taps);

        std::vector<std::complex<double>> output;

        upfirlerp.interpolate(
            input,
            T,
            t,
            output
        );

        for (int i = 0; i < output.size(); ++i)
        {
            REQUIRE_THAT(
                output[i].real(),
                Catch::Matchers::WithinRel(expected[i].real(), 1e-10)
            );
            REQUIRE_THAT(
                output[i].imag(),
                Catch::Matchers::WithinRel(expected[i].imag(), 1e-10)
            );
        }
    }

    SECTION("double, 4 threads")
    {
        ufl::UpfirLerp<double> upfirlerp;

        upfirlerp.set_threads(4).set_up_rate(4).set_up_taps(taps);

        std::vector<std::complex<double>> output;

        upfirlerp.interpolate(
            input,
            T,
            t,
            output
        );

        for (int i = 0; i < output.size(); ++i)
        {
            REQUIRE_THAT(
                output[i].real(),
                Catch::Matchers::WithinRel(expected[i].real(), 1e-10)
            );
            REQUIRE_THAT(
                output[i].imag(),
                Catch::Matchers::WithinRel(expected[i].imag(), 1e-10)
            );
        }
    }

    SECTION("float")
    {
        // we cast our set-up vectors to float
        std::vector<float> taps_f(taps.size());
        for (int i = 0; i < taps.size(); ++i)
        {
            taps_f[i] = static_cast<float>(taps[i]);
        }

        std::vector<std::complex<float>> input_f(input.size());
        for (int i = 0; i < input.size(); ++i)
            input_f[i] = static_cast<std::complex<float>>(input[i]);

        ufl::UpfirLerp<float> upfirlerp;

        upfirlerp.set_threads(1).set_up_rate(4).set_up_taps(taps_f);

        std::vector<std::complex<float>> output;

        upfirlerp.interpolate(
            input_f,
            T,
            t,
            output
        );

        for (int i = 0; i < output.size(); ++i)
        {
            // single precision needs higher thresholds..
            REQUIRE_THAT(
                output[i].real(),
                Catch::Matchers::WithinRel(expected[i].real(), 1e-7)
            );
            REQUIRE_THAT(
                output[i].imag(),
                Catch::Matchers::WithinRel(expected[i].imag(), 1e-7)
            );
        }
    }


    SECTION("float, 4 threads")
    {
        // we cast our set-up vectors to float
        std::vector<float> taps_f(taps.size());
        for (int i = 0; i < taps.size(); ++i)
        {
            taps_f[i] = static_cast<float>(taps[i]);
        }

        std::vector<std::complex<float>> input_f(input.size());
        for (int i = 0; i < input.size(); ++i)
            input_f[i] = static_cast<std::complex<float>>(input[i]);

        ufl::UpfirLerp<float> upfirlerp;

        upfirlerp.set_threads(4).set_up_rate(4).set_up_taps(taps_f);

        std::vector<std::complex<float>> output;

        upfirlerp.interpolate(
            input_f,
            T,
            t,
            output
        );

        for (int i = 0; i < output.size(); ++i)
        {
            // single precision needs higher thresholds..
            REQUIRE_THAT(
                output[i].real(),
                Catch::Matchers::WithinRel(expected[i].real(), 1e-7)
            );
            REQUIRE_THAT(
                output[i].imag(),
                Catch::Matchers::WithinRel(expected[i].imag(), 1e-7)
            );
        }
    }
};

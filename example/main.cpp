#include "upfirlerp.h"
#include <vector>
#include <complex>
#include <cstdlib>
#include <iostream>

int main()
{
    // Create the object
    ufl::UpfirLerp<float> upfirlerp;

    // Define upsample rate
    int up = 10;

    // Define taps
    std::vector<float> taps = {0.1, 1.0, 0.1};

    // Configure
    upfirlerp.set_up_rate(up).set_up_taps(taps);

    // Generate some data
    std::vector<std::complex<float>> input(10);
    for (auto& v : input)
    {
        v = std::complex<float>(std::rand() / (float)RAND_MAX, std::rand() / (float)RAND_MAX);
        std::cout << v << std::endl;
    }

    // Interpolate
    std::vector<std::complex<float>> output;
    std::vector<double> t = {0.01, 0.03, 0.045, 0.06, 0.062, 0.071};
    upfirlerp.interpolate(
        input, 0.1, t, output
    );

    // Print
    for (int i = 0; i < output.size(); ++i)
    {
        printf("%d : %f, %f \n", i, output[i].real(), output[i].imag());
    }



    return 0;
}

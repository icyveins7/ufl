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
    int up = 2;

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
    const double T = 0.01;
    printf("Input data is from [0, %f], upsampled should be from [0, %f]\n",
           T * (input.size()-1), (T/up) * (up * input.size() - 1));

    // Interpolate
    std::vector<std::complex<float>> output;
    std::vector<double> t = {0.004, 0.03, 0.045, 0.06, 0.062, 0.095, 0.10};
    upfirlerp.interpolate(
        input, 0.01, t, output
    );

    // Print
    for (int i = 0; i < output.size(); ++i)
    {
        printf("%d : %f, %f \n", i, output[i].real(), output[i].imag());
    }



    return 0;
}

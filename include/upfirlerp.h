#pragma once

#include <vector>
#include <complex>
#include <thread>

namespace ufl
{

template <typename T>
class UpfirLerp
{
public:

    // Main runtime method.
    void interpolate(
        const std::vector<std::complex<T>> &in,
        const double in_T,
        const std::vector<double> &t,
        std::vector<std::complex<T>> &out
    ){
        out.resize(t.size());

        // Split the work over multiple threads
        std::vector<std::thread> threads(m_threads);

        for (int tidx = 0; tidx < m_threads; tidx++)
        {
            threads.at(tidx) = std::thread(
                &UpfirLerp::interpolate_work,
                this,
                tidx,
                std::ref(in),
                in_T,
                std::ref(t),
                std::ref(out)
            );
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
    }

    // Configures the upsampling filter taps.
    UpfirLerp& set_up_taps(
        const std::vector<T> &taps
    ){
        // Reverse them so we can perform multiplies in increasing order
        m_rev_taps = taps;
        std::reverse(m_rev_taps.begin(), m_rev_taps.end());

        return *this;
    }

    // Configures the upsampling rate.
    UpfirLerp& set_up_rate(int up)
    {
        m_up = up;
        return *this;
    }

    // Configure number of threads to use
    UpfirLerp& set_threads(int threads)
    {
        m_threads = threads;
        return *this;
    }


private:
    int m_threads = 1;
    int m_up = 0;

    std::vector<T> m_rev_taps;


    void interpolate_work(
        int tidx,
        const std::vector<std::complex<T>> &in,
        const double in_T,
        const std::vector<double> &t,
        std::vector<std::complex<T>> &out
    ){
        printf("Thread %d: output is size %zd\n", tidx, out.size());

        // Precompute the upsampled period
        const double interrim_T = in_T / static_cast<double>(m_up);

        for(int i=tidx; i<t.size(); i+=m_threads)
        {
            // Compute the interrim sample number
            int j = static_cast<int>(t.at(i) / interrim_T);
            // Ensure this sample is valid
            if (j < 0 || j >= in.size() * m_up)
            {
                // TODO: decide if want to write nans
                continue;
            }

            // Linearly interpolate between this and the next sample
            std::complex<T> xj1 = {0, 0.2}; // TODO: dot product
            std::complex<T> xj2 = {0.1, -0.4}; // TODO: dot product
            // Compute the time values at just the 1st sample
            double tj1 = interrim_T * j;

            // Then interpolate
            // TODO: determine if downcasting to float is ok?
            out.at(i) = xj1 + (xj2 - xj1) * static_cast<T>((t.at(i) - tj1) / interrim_T);
        }
    }

};

}

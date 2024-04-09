#pragma once

#include <vector>
#include <complex>
#include <thread>

#ifndef NDEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

namespace ufl
{


template <typename T, typename UflClass>
class BaseUpfirLerp
{
public:

    void interpolate_array(
        const std::complex<T>* const in,
        const size_t in_length,
        const double in_T,
        const double* const t,
        const size_t out_length,
        std::complex<T>* out
    ){
        // Split the work over multiple threads
        std::vector<std::thread> threads(m_threads);

        for (int tidx = 0; tidx < m_threads; tidx++)
        {
            threads.at(tidx) = std::thread(
                &BaseUpfirLerp<T, UflClass>::interpolate_work,
                this,
                tidx,
                in,
                in_length,
                in_T,
                t,
                out_length,
                out
            );
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

    }

    // Main runtime method.
    void interpolate(
        const std::vector<std::complex<T>> &in,
        const double in_T,
        const std::vector<double> &t,
        std::vector<std::complex<T>> &out
    ){
        out.resize(t.size());

        // Just call the array form
        interpolate_array(
            in.data(),
            in.size(),
            in_T,
            t.data(),
            t.size(),
            out.data()
        );
    }


    // Configures the upsampling filter taps.

    // array-style
    UflClass& set_up_taps(
        const T* const taps,
        size_t len
    ){
        m_rev_taps.resize(len);
        m_rev_taps.assign(taps, taps + len);
        std::reverse(m_rev_taps.begin(), m_rev_taps.end());

        return static_cast<UflClass&>(*this);
    }

    // std::vector-style
    UflClass& set_up_taps(
        const std::vector<T> &taps
    ){
        // Reverse them so we can perform multiplies in increasing order
        m_rev_taps = taps;
        std::reverse(m_rev_taps.begin(), m_rev_taps.end());

        return static_cast<UflClass&>(*this);
    }

    // Configures the upsampling rate.
    UflClass& set_up_rate(int up)
    {
        m_up = up;
        return static_cast<UflClass&>(*this);
    }
    int get_up_rate() const
    {
        return m_up;
    }

    // Configure number of threads to use
    UflClass& set_threads(int threads)
    {
        m_threads = threads;
        return static_cast<UflClass&>(*this);
    }
    int get_threads() const
    {
        return m_threads;
    }


protected:
    int m_threads = 1;
    int m_up = 1;

    std::vector<T> m_rev_taps;


    void interpolate_work(
        int tidx,
        const std::complex<T>* const in,
        const size_t in_length,
        const double in_T,
        const double* const t,
        const size_t out_length,
        std::complex<T>* out
    ){
        DEBUG_PRINT("Thread %d: output is size %zd\n", tidx, out_length);

        // Precompute the upsampled period
        const double interrim_T = in_T / static_cast<double>(m_up);

        // Define thread workspace
        int tistart = tidx * out_length / m_threads;
        int tistop = (tidx + 1) * out_length / m_threads;
        tistop = tistop > out_length ? out_length : tistop; // ensure last thread doesn't go out of bounds

        for(int i=tistart; i<tistop; i++)
        {
            // We exclude interpolation for any sample that is outside the upsampled range
            if (t[i] < 0 || t[i] > interrim_T * (in_length * m_up - 1))
            {
                DEBUG_PRINT("t[%d] = %f is outside the valid upsampled range [0, %f]\n",
                       i, t[i], interrim_T * (in_length * m_up - 1));

                continue;
            }
            // Compute the interrim sample number
            double jd = t[i] / interrim_T;
            int j = static_cast<int>(jd);

            DEBUG_PRINT("t[%d]=%f -> %f[%d]\n", i, t[i], jd, j);

            // Linearly interpolate between this and the next sample
            std::complex<T> xj1 = calculate_interrim_sample(j, in, in_length);
            std::complex<T> xj2 = calculate_interrim_sample(j+1, in, in_length);
            // Compute the time values at just the 1st sample
            double tj1 = interrim_T * j;

            // Then interpolate
            // TODO: determine if downcasting to float is ok?
            out[i] = xj1 + (xj2 - xj1) * static_cast<T>((t[i] - tj1) / interrim_T);
        }
    }

    // Helper method
    std::complex<T> calculate_interrim_sample(
        const int j,
        const std::complex<T>* const in,
        const size_t in_length
    ){
        // We need to perform the dot product from [j-m_rev_taps.size()+1 , j]
        int start = j - m_rev_taps.size() + 1; // keep this so we can reference the taps vector index
        // Enforce starting at 0
        int zstart = start < 0 ? 0 : start;
        // Now enforce starting at a non-zero index (0, 1*m_up, 2*m_up,...)
        if (zstart % m_up != 0 && j - zstart >= m_up)
            zstart += m_up - (zstart % m_up);

        // Enforce ending to be size of the upsampled input
        int zend = j >= in_length * m_up ? in_length * m_up - 1 : j;

        if (zstart <= j)
            DEBUG_PRINT("==Interrim %d: dot prod from %d(%d) to %zd(%d)\n", j, start, zstart, start + m_rev_taps.size() - 1, zend);
        else
            DEBUG_PRINT("==Interrim %d: no valid dot prod\n", j);

        // Finally iterate forwards with the dot product
        std::complex<T> xj = {0, 0};

        for (int i = zstart; i <= zend; i += m_up)
        {
            DEBUG_PRINT("==Accessing interrim index %d -> index %d from input\n", i, i/m_up);
            DEBUG_PRINT("==Multiplying by taps[%d]\n", i - start);
            xj += in[i / m_up] * static_cast<T>(m_rev_taps.at(i - start));
        }

        DEBUG_PRINT("==Dot prod for interrim %d complete\n\n", j);


        return xj;
    }
}; // End of class definition

// CRTP definition of actual class
// This is the actual class that should be used.
template <typename T>
class UpfirLerp : public BaseUpfirLerp<T, UpfirLerp<T>>
{

};

}

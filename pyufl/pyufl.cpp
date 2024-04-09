#include "upfirlerp.h"
#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>

namespace nb = nanobind;
using namespace nb::literals;

template <typename T>
class Pyufl : public ufl::UpfirLerp<T>
{
public:
    // Define the wrapper to numpy array
    void interpolate_numpy(
        const nb::ndarray<std::complex<T>, nb::ndim<1>> &input,
        const double in_T,
        const nb::ndarray<double, nb::ndim<1>> &t,
        nb::ndarray<std::complex<T>, nb::ndim<1>> &output
    ){

        // Check that t length and output length are the same
        if (t.shape(0) != output.shape(0))
            throw std::runtime_error("t and output must have the same length");

        // Directly call the interpolate function
        this->interpolate_array(
            reinterpret_cast<const std::complex<T>*>(input.data()),
            input.shape(0),
            in_T,
            reinterpret_cast<const double*>(t.data()),
            t.shape(0),
            reinterpret_cast<std::complex<T>*>(output.data())
        );
    }

    // setters and getters, explicitly
    // TODO: find a way to get CRTP polymorphic chaining restored?
    int get_up_rate() const {return m_up;}
    void set_up_rate(int up) { 
        m_up = up;
        printf("m_up is now %d\n", m_up);
    }

    int get_threads() const { return m_threads; }
    void set_threads(int threads) { m_threads = threads; }

    void set_up_taps(const nb::ndarray<T, nb::ndim<1>> &taps)
    {
        ufl::UpfirLerp<T>::set_up_taps(
            reinterpret_cast<const T*>(taps.data()), taps.shape(0));
    }


};

NB_MODULE(pyufl, m) {
    // We specify noconvert() so it doesn't change 32f to 64f
    nb::class_<Pyufl<double>>(m, "PyuflDouble")
        .def(nb::init<>())
        .def("set_up_rate", &Pyufl<double>::set_up_rate, "up"_a)
        .def("get_up_rate", &Pyufl<double>::get_up_rate)
        .def("set_threads", &Pyufl<double>::set_threads, "threads"_a)
        .def("get_threads", &Pyufl<double>::get_threads)
        .def("set_up_taps", &Pyufl<double>::set_up_taps, "taps"_a.noconvert())
        .def("interpolate_numpy", &Pyufl<double>::interpolate_numpy,
             "input"_a.noconvert(),
             "in_T"_a,
             "t"_a.noconvert(),
             "output"_a.noconvert()
        );


    // nb::class_<Pyufl<float>>(m, "PyuflFloat")
    //     .def(nb::init<>())
    //     .def("set_up_rate", &Pyufl<float>::set_up_rate, "up"_a)
    //     .def("get_up_rate", &Pyufl<float>::get_up_rate)
    //     .def("set_threads", &Pyufl<float>::set_threads, "threads"_a)
    //     .def("get_threads", &Pyufl<float>::get_threads)
    //     .def("set_up_taps", &Pyufl<float>::set_up_taps, "taps"_a)
    //     .def("interpolate_numpy", &Pyufl<float>::interpolate_numpy,
    //          "input"_a,
    //          "in_T"_a,
    //          "t"_a,
    //          "output"_a
    //     );
}

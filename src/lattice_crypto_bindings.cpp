#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>  // If you are using Eigen with pybind11
#include "lattice_crypto.h"   // Always include the header file, not the .cpp file

namespace py = pybind11;

PYBIND11_MODULE(lattice_crypto, m) {
    // Binding the RingLWECrypto class to Python
    py::class_<RingLWECrypto>(m, "RingLWECrypto")
        .def(py::init<int, int>())  // Constructor binding
        .def("encrypt", &RingLWECrypto::encrypt)  // Binding the encrypt method
        .def("decrypt", &RingLWECrypto::decrypt); // Binding the decrypt method
}

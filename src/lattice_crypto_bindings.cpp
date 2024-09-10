#include <pybind11/pybind11.h>
#include "lattice_crypto.h"   // Include the header file

namespace py = pybind11;

// Use the lattice_crypto namespace to avoid prefixing the class everywhere
using namespace lattice_crypto;

PYBIND11_MODULE(lattice_crypto, m) {
    // Binding the RingLWECrypto class to Python
    py::class_<RingLWECrypto>(m, "RingLWECrypto")
        .def(py::init<int, int>())  // Constructor binding
        .def("encrypt", &RingLWECrypto::encrypt)  // Binding the encrypt method
        .def("decrypt", &RingLWECrypto::decrypt); // Binding the decrypt method
}

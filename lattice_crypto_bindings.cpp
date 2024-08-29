#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include "lattice_crypto.cpp"  // Include the C++ file directly

namespace py = pybind11;

PYBIND11_MODULE(lattice_crypto, m) {
    py::class_<lattice_crypto::RingLWECrypto>(m, "RingLWECrypto")
        .def(py::init<int, int>())
        .def("encrypt", &lattice_crypto::RingLWECrypto::encrypt)
        .def("decrypt", &lattice_crypto::RingLWECrypto::decrypt);
}

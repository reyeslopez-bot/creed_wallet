#include "lattice_crypto.h"  // Include the header file for declarations
#include <unsupported/Eigen/FFT>  // Include FFT from Eigen
#include <random>  // For random number generation
#include <complex>  // For handling complex numbers
#include <iostream>  // For input-output operations

namespace lattice_crypto {

// Implementation of generate_random_matrix
Eigen::MatrixXi KeyGenerator::generate_random_matrix(int rows, int cols) {
    Eigen::MatrixXi mat(rows, cols);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            mat(i, j) = dist(gen);
        }
    }
    return mat;
}

// Implementation of polynomial_multiply
Eigen::MatrixXi KeyGenerator::polynomial_multiply(const Eigen::MatrixXi& a, const Eigen::MatrixXi& b, int q) {
    if (a.cols() != b.rows()) {
        throw std::runtime_error("Matrix dimensions are not compatible for multiplication.");
    }

    int resultSize = 1;
    while (resultSize < a.cols() + b.rows() - 1) {
        resultSize *= 2;
    }

    std::vector<std::complex<double>> a_complex(resultSize, 0), b_complex(resultSize, 0);
    for (int i = 0; i < a.cols(); ++i) {
        a_complex[i] = std::complex<double>(a(0, i), 0);
    }
    for (int i = 0; i < b.rows(); ++i) {
        b_complex[i] = std::complex<double>(b(i, 0), 0);
    }

    Eigen::FFT<double> fft;
    fft.fwd(a_complex, a_complex);
    fft.fwd(b_complex, b_complex);

    std::vector<std::complex<double>> result_complex(resultSize);
    for (int i = 0; i < resultSize; ++i) {
        result_complex[i] = a_complex[i] * b_complex[i];
    }

    fft.inv(result_complex, result_complex);

    Eigen::MatrixXi result_int(a.rows(), b.cols());
    for (int i = 0; i < a.rows(); ++i) {
        for (int j = 0; j < b.cols(); ++j) {
            result_int(i, j) = static_cast<int>(std::round(result_complex[i + j].real() / resultSize)) % q;
        }
    }
    return result_int;
}

// Implementation of generate_keys
std::pair<Eigen::MatrixXi, Eigen::MatrixXi> KeyGenerator::generate_keys(int rows, int cols, int q) {
    Eigen::MatrixXi secret_key = generate_random_matrix(rows, cols);
    Eigen::MatrixXi public_key = polynomial_multiply(secret_key, generate_random_matrix(cols, cols), q);
    return {secret_key, public_key};
}

// Constructor for RingLWECrypto
RingLWECrypto::RingLWECrypto(int poly_degree, int modulus)
    : poly_degree(poly_degree), q(modulus), key_gen(std::make_unique<KeyGenerator>()) {
    secret_key = key_gen->generate_random_matrix(poly_degree, poly_degree);
    public_key = key_gen->generate_keys(poly_degree, poly_degree, q);
}

// Implementation of encrypt
std::pair<Eigen::MatrixXi, Eigen::MatrixXi> RingLWECrypto::encrypt(const std::string& plaintext) {
    Eigen::MatrixXi plaintext_matrix(1, poly_degree);
    for (size_t i = 0; i < plaintext.size(); ++i) {  // Changed int to size_t
        plaintext_matrix(0, i) = static_cast<int>(plaintext[i]);
    }

    if (plaintext.size() < static_cast<size_t>(poly_degree)) {  // Cast poly_degree to size_t
        for (size_t i = plaintext.size(); i < static_cast<size_t>(poly_degree); ++i) {
            plaintext_matrix(0, i) = 0;
        }
    }

    Eigen::MatrixXi error_matrix = key_gen->generate_random_matrix(1, poly_degree);
    Eigen::MatrixXi c1 = key_gen->polynomial_multiply(public_key.first, plaintext_matrix.transpose(), q).transpose();
    Eigen::MatrixXi c2 = plaintext_matrix + key_gen->polynomial_multiply(public_key.second, error_matrix.transpose(), q).transpose();

    return {c1, c2};
}

// Implementation of decrypt
std::string RingLWECrypto::decrypt(const std::pair<Eigen::MatrixXi, Eigen::MatrixXi>& ciphertext) {
    Eigen::MatrixXi c1 = ciphertext.first;
    Eigen::MatrixXi c2 = ciphertext.second;

    Eigen::MatrixXi m = key_gen->polynomial_multiply(c1, secret_key, q);
    m = c2 - m;

    std::string plaintext;
    for (int i = 0; i < m.cols(); ++i) {
        plaintext += static_cast<char>(m(0, i));
    }

    return plaintext;
}

}  // namespace lattice_crypto

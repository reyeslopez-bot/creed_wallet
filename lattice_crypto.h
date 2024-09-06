#ifndef LATTICE_CRYPTO_H
#define LATTICE_CRYPTO_H

#include <Eigen/Dense>
#include <memory>
#include <string>
#include <utility>

namespace lattice_crypto {

// KeyGenerator handles the generation of keys and related cryptographic operations
class KeyGenerator {
public:
    // Generates a random matrix of given dimensions using OpenSSL's secure random number generator
    Eigen::MatrixXi generate_random_matrix(int rows, int cols);

    // Performs polynomial multiplication using Number Theoretic Transform (NTT)
    Eigen::MatrixXi polynomial_multiply(const Eigen::MatrixXi& a, const Eigen::MatrixXi& b, int q);

    // Generates both secret and public keys
    std::pair<Eigen::MatrixXi, Eigen::MatrixXi> generate_keys(int rows, int cols, int q);

    // Generates binomially distributed error values
    Eigen::MatrixXi generate_binomial_error(int rows, int cols);  // This is required as used in the cpp file
};

// RingLWECrypto handles encryption and decryption using Ring Learning with Errors (Ring-LWE) cryptography
class RingLWECrypto {
public:
    // Constructor with default parameters for polynomial degree and modulus
    RingLWECrypto(int poly_degree = 512, int modulus = 4096);

    // Encrypts the given plaintext and returns a pair of ciphertexts (c1, c2)
    std::pair<Eigen::MatrixXi, Eigen::MatrixXi> encrypt(const std::string& plaintext);

    // Decrypts the given ciphertext pair and returns the original plaintext
    std::string decrypt(const std::pair<Eigen::MatrixXi, Eigen::MatrixXi>& ciphertext);

private:
    int poly_degree;  // Degree of the polynomial used in cryptographic operations
    int q;  // Modulus value
    std::unique_ptr<KeyGenerator> key_gen;  // Key generation utility
    Eigen::MatrixXi secret_key;  // Secret key matrix
    std::pair<Eigen::MatrixXi, Eigen::MatrixXi> public_key;  // Public key pair
};

}  // namespace lattice_crypto

#endif  // LATTICE_CRYPTO_H

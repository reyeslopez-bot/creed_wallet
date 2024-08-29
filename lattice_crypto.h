#ifndef LATTICE_CRYPTO_H
#define LATTICE_CRYPTO_H

#include <Eigen/Dense>
#include <memory>
#include <string>
#include <utility>

namespace lattice_crypto {

class KeyGenerator {
public:
    Eigen::MatrixXi generate_random_matrix(int rows, int cols);
    Eigen::MatrixXi polynomial_multiply(const Eigen::MatrixXi& a, const Eigen::MatrixXi& b, int q);
    std::pair<Eigen::MatrixXi, Eigen::MatrixXi> generate_keys(int rows, int cols, int q);
};

class RingLWECrypto {
public:
    RingLWECrypto(int poly_degree = 512, int modulus = 4096);
    std::pair<Eigen::MatrixXi, Eigen::MatrixXi> encrypt(const std::string& plaintext);
    std::string decrypt(const std::pair<Eigen::MatrixXi, Eigen::MatrixXi>& ciphertext);

private:
    int poly_degree;
    int q;
    std::unique_ptr<KeyGenerator> key_gen;
    Eigen::MatrixXi secret_key;
    std::pair<Eigen::MatrixXi, Eigen::MatrixXi> public_key;
};

}  // namespace lattice_crypto

#endif  // LATTICE_CRYPTO_H

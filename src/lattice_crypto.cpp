#include "lattice_crypto.h"  // Include the header file for declarations
#include <random>  // For random number generation
#include <iostream>  // For input-output operations
#include <fstream>  // For logging output to a file
#include <openssl/rand.h>  // OpenSSL for cryptographic random number generation
#include <iomanip>  // For std::setw and std::setfill
#include <vector>
#include <cmath>
#include <stdexcept>

#if __has_include(<filesystem>)
  #include <filesystem>
  namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
  #include <experimental/filesystem>
  namespace fs = std::experimental::filesystem;
#else
  #error "No filesystem support"
#endif

// Global log file variable
std::ofstream log_file;

void init_logging() {
    // Ensure log directory exists
    try {
        if (!fs::create_directories("logs")) {
            std::cerr << "Log directory already exists or failed to create." << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);  // Exit if the log directory can't be created
    }

    // Start logging to a file inside the logs directory
    log_file.open("logs/crypto_log.txt", std::ios::out | std::ios::app);  // Open in append mode

    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        exit(EXIT_FAILURE);  // Exit if the log file can't be opened
    }

    log_file << "Logging started." << std::endl;
    log_file.flush();  // Flush immediately after writing
}

namespace lattice_crypto {

// Modular exponentiation function with base, exponent, and modulo
int mod_exp(int base, int exp, int mod) {
    log_file << "Modular exponentiation started: base=" << base << ", exp=" << exp << ", mod=" << mod << std::endl;
    log_file.flush();  // Flush the log after each log entry
    int result = 1;
    base %= mod;  // Always reduce base modulo
    while (exp > 0) {
        if (exp & 1) result = (1LL * result * base) % mod;
        base = (1LL * base * base) % mod;
        exp >>= 1;
    }
    log_file << "Modular exponentiation result: " << result << std::endl;
    log_file.flush();
    return result;
}

// Helper function to reverse bits of a number up to 'log_n' bits
int reverse_bits(int num, int log_n) {
    log_file << "Reversing bits for number: " << num << ", log_n: " << log_n << std::endl;
    int reversed = 0;
    for (int i = 0; i < log_n; ++i) {
        if (num & (1 << i)) reversed |= 1 << (log_n - 1 - i);
    }
    log_file << "Reversed bits result: " << reversed << std::endl;
    return reversed;
}

// Number Theoretic Transform (NTT) implementation
std::vector<int> ntt(const std::vector<int>& a, int n, int root, int mod) {
    log_file << "Starting NTT for vector of size " << a.size() << " with n: " << n << ", root: " << root << ", mod: " << mod << std::endl;
    std::vector<int> A(n);
    int log_n = std::log2(n);
    for (int i = 0; i < n; ++i) {
        A[i] = a[reverse_bits(i, log_n)];
    }
    for (int length = 2; length <= n; length <<= 1) {
        int w_length = mod_exp(root, (mod - 1) / length, mod);
        for (int i = 0; i < n; i += length) {
            int w = 1;
            for (int j = 0; j < length / 2; ++j) {
                int u = A[i + j];
                int v = (1LL * A[i + j + length / 2] * w) % mod;
                A[i + j] = (u + v) % mod;
                A[i + j + length / 2] = (u - v + mod) % mod;
                w = (1LL * w * w_length) % mod;
            }
        }
    }
    log_file << "NTT completed." << std::endl;
    return A;
}

// Inverse NTT
std::vector<int> intt(const std::vector<int>& a, int n, int inv_root, int mod) {
    log_file << "Starting inverse NTT..." << std::endl;
    std::vector<int> A = ntt(a, n, inv_root, mod);
    int inv_n = mod_exp(n, mod - 2, mod);  // Fermat's Little Theorem
    for (int &x : A) {
        x = (1LL * x * inv_n) % mod;
    }
    log_file << "Inverse NTT completed." << std::endl;
    return A;
}

// Secure random matrix generation using OpenSSL's cryptographic RNG
Eigen::MatrixXi KeyGenerator::generate_random_matrix(int rows, int cols) {
    log_file << "Generating random matrix with dimensions: " << rows << "x" << cols << std::endl;
    Eigen::MatrixXi mat(rows, cols);
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            unsigned char rand_byte;
            if (RAND_bytes(&rand_byte, sizeof(rand_byte)) != 1) {
                log_file << "Error generating secure random bytes." << std::endl;
                throw std::runtime_error("Error generating secure random bytes.");
            }
            mat(i, j) = rand_byte % 2;  // Generate either 0 or 1 for this example
        }
    }
    
    log_file << "Random matrix generated." << std::endl;
    return mat;
}

// Binomial distribution-based error generation
Eigen::MatrixXi KeyGenerator::generate_binomial_error(int rows, int cols) {
    log_file << "Generating binomial error matrix with dimensions: " << rows << "x" << cols << std::endl;
    Eigen::MatrixXi mat(rows, cols);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::binomial_distribution<int> binomial(10, 0.5);  // Parameters: trials=10, probability=0.5

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            mat(i, j) = binomial(gen) - 5;  // Center the distribution around 0
        }
    }
    
    log_file << "Binomial error matrix generated." << std::endl;
    return mat;
}

// Convert matrix to hex
std::string matrix_to_hex(const Eigen::MatrixXi& matrix) {
    std::stringstream hex_stream;
    for (int i = 0; i < matrix.rows(); ++i) {
        for (int j = 0; j < matrix.cols(); ++j) {
            unsigned char value = static_cast<unsigned char>(matrix(i, j));
            hex_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value);
        }
    }
    log_file << "Matrix converted to hex: " << hex_stream.str() << std::endl;
    return hex_stream.str();
}

// NTT-based polynomial multiplication
Eigen::MatrixXi KeyGenerator::polynomial_multiply(const Eigen::MatrixXi& a, const Eigen::MatrixXi& b, int q) {
    log_file << "Polynomial multiplication started..." << std::endl;
    if (a.cols() != b.rows()) {
        log_file << "Matrix dimensions are not compatible for multiplication." << std::endl;
        throw std::runtime_error("Matrix dimensions are not compatible for multiplication.");
    }
    int resultSize = 1;
    while (resultSize < a.cols() + b.rows() - 1) {
        resultSize <<= 1;  // Ensure resultSize is a power of 2
    }

    std::vector<int> a_values(resultSize, 0);
    std::vector<int> b_values(resultSize, 0);

    for (int i = 0; i < a.cols(); ++i) {
        a_values[i] = a(0, i) % q;
    }
    for (int i = 0; i < b.rows(); ++i) {
        b_values[i] = b(i, 0) % q;
    }

    int root = 3;  // Primitive root
    int inv_root = mod_exp(root, q - 2, q);  // Modular inverse of the root

    std::vector<int> a_ntt = ntt(a_values, resultSize, root, q);
    std::vector<int> b_ntt = ntt(b_values, resultSize, root, q);

    std::vector<int> result_ntt(resultSize, 0);
    for (int i = 0; i < resultSize; ++i) {
        result_ntt[i] = (1LL * a_ntt[i] * b_ntt[i]) % q;
    }

    std::vector<int> result = intt(result_ntt, resultSize, inv_root, q);

    Eigen::MatrixXi result_int(1, resultSize);
    for (int i = 0; i < resultSize; ++i) {
        result_int(0, i) = result[i] % q;
        if (result_int(0, i) < 0) {
            result_int(0, i) += q;  // Ensure values are positive
        }
    }
    log_file << "Polynomial multiplication completed." << std::endl;
    return result_int;
}

// Modify the dimensions of the public key to match poly_degree (512)
std::pair<Eigen::MatrixXi, Eigen::MatrixXi> KeyGenerator::generate_keys(int rows, int cols, int q) {
    log_file << "Key generation started..." << std::endl;

    // Generate secret key with dimensions rows x cols (both set to poly_degree)
    Eigen::MatrixXi secret_key = generate_random_matrix(rows, cols);

    // Adjust public_key dimensions to match poly_degree
    Eigen::MatrixXi public_key_first = polynomial_multiply(secret_key, generate_random_matrix(rows, cols), q);
    Eigen::MatrixXi public_key_second = polynomial_multiply(secret_key, generate_random_matrix(rows, cols), q);

    log_file << "Secret Key (hex): \n" << matrix_to_hex(secret_key) << std::endl;
    log_file << "Public Key (first part, hex): \n" << matrix_to_hex(public_key_first) << std::endl;
    log_file << "Public Key (second part, hex): \n" << matrix_to_hex(public_key_second) << std::endl;

    log_file << "Key generation completed." << std::endl;

    return {public_key_first, public_key_second};
}

// Constructor for RingLWECrypto
RingLWECrypto::RingLWECrypto(int poly_degree, int modulus)
    : poly_degree(poly_degree), q(modulus), key_gen(std::unique_ptr<KeyGenerator>(new KeyGenerator())) {
    log_file << "Initializing RingLWE Crypto with polynomial degree: " << poly_degree << ", modulus: " << modulus << std::endl;
    secret_key = key_gen->generate_random_matrix(poly_degree, poly_degree);
    std::pair<Eigen::MatrixXi, Eigen::MatrixXi> public_key_pair = key_gen->generate_keys(poly_degree, poly_degree, q);

    public_key.first = public_key_pair.first;
    public_key.second = public_key_pair.second;

    log_file << "Secret Key (hex): \n" << matrix_to_hex(secret_key) << std::endl;
    log_file << "Public Key (first part, hex): \n" << matrix_to_hex(public_key.first) << std::endl;
    log_file << "Public Key (second part, hex): \n" << matrix_to_hex(public_key.second) << std::endl;
}

std::pair<Eigen::MatrixXi, Eigen::MatrixXi> RingLWECrypto::encrypt(const std::string& plaintext) {
    log_file << "Starting encryption for plaintext: " << plaintext << std::endl;

    // Create the plaintext matrix with size based on poly_degree
    Eigen::MatrixXi plaintext_matrix(1, poly_degree);  // poly_degree = 512
    for (size_t i = 0; i < plaintext.size(); ++i) {
        plaintext_matrix(0, i) = static_cast<int>(plaintext[i]);
    }

    // Fill remaining part of the matrix with zeros if plaintext size < poly_degree
    if (plaintext.size() < static_cast<size_t>(poly_degree)) {
        for (size_t i = plaintext.size(); i < static_cast<size_t>(poly_degree); ++i) {
            plaintext_matrix(0, i) = 0;
        }
    }

    // Generate the error matrix
    Eigen::MatrixXi error_matrix = key_gen->generate_binomial_error(1, poly_degree);

    // Log the dimensions for debugging
    log_file << "public_key.first dimensions: " << public_key.first.rows() << " x " << public_key.first.cols() << std::endl;
    log_file << "plaintext_matrix dimensions: " << plaintext_matrix.rows() << " x " << plaintext_matrix.cols() << std::endl;
    log_file << "plaintext_matrix.transpose() dimensions: " << plaintext_matrix.transpose().rows() << " x " << plaintext_matrix.transpose().cols() << std::endl;

    // Check dimensions before performing multiplication
    if (public_key.first.cols() == plaintext_matrix.transpose().rows()) {
        Eigen::MatrixXi c1 = key_gen->polynomial_multiply(public_key.first, plaintext_matrix.transpose(), q).transpose();
        Eigen::MatrixXi c2 = plaintext_matrix + key_gen->polynomial_multiply(public_key.second, error_matrix.transpose(), q).transpose();

        log_file << "Plaintext matrix (hex): " << matrix_to_hex(plaintext_matrix) << std::endl;
        log_file << "Ciphertext c1 (hex): " << matrix_to_hex(c1) << std::endl;
        log_file << "Ciphertext c2 (hex): " << matrix_to_hex(c2) << std::endl;

        return {c1, c2};
    } else {
        log_file << "Error: Matrix dimensions are not compatible for multiplication." << std::endl;
        log_file << "public_key.first.cols(): " << public_key.first.cols() << ", plaintext_matrix.transpose().rows(): " << plaintext_matrix.transpose().rows() << std::endl;
        throw std::runtime_error("Matrix dimensions are not compatible for multiplication.");
    }
}
// Decryption
std::string RingLWECrypto::decrypt(const std::pair<Eigen::MatrixXi, Eigen::MatrixXi>& ciphertext) {
    log_file << "Starting decryption..." << std::endl;
    Eigen::MatrixXi c1 = ciphertext.first;
    Eigen::MatrixXi c2 = ciphertext.second;

    Eigen::MatrixXi m = key_gen->polynomial_multiply(c1, secret_key, q);
    m = c2 - m;

    std::stringstream hex_stream;
    bool found_padding = false;
    for (int i = 0; i < m.cols(); ++i) {
        unsigned char decrypted_byte = static_cast<unsigned char>(m(0, i));
        if (decrypted_byte == 0 || decrypted_byte == 0xff) {
            found_padding = true;
            break;
        }
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(decrypted_byte);
    }

    if (found_padding) {
        log_file << "Padding found and stripped from the decrypted message." << std::endl;
    }

    log_file << "Decrypted hex: " << hex_stream.str() << std::endl;

    return hex_stream.str();
}

}  // namespace lattice_crypto

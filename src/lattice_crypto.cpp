#include "lattice_crypto.h"  // Include the header file for declarations
#include <random>  // For random number generation
#include <iostream>  // For input-output operations
#include <fstream>  // For logging output to a file
#include <openssl/rand.h>  // OpenSSL for cryptographic random number generation
#include <iomanip>  // For std::setw and std::setfill
#include <vector> // For std::vector
#include <cmath> // For std::log2
#include <stdexcept> // For std::runtime_error

#if __has_include(<filesystem>) // Check if filesystem is available
  #include <filesystem> // For C++17 filesystem support
  namespace fs = std::filesystem; // Alias for filesystem namespace
#elif __has_include(<experimental/filesystem>) // Check if experimental filesystem is available
  #include <experimental/filesystem> // For C++17 experimental filesystem support
  namespace fs = std::experimental::filesystem; // Alias for experimental filesystem namespace
#else // No filesystem support
  #error "No filesystem support" // Error message
#endif // End of filesystem check

// Global log file variable
std::ofstream log_file; // Log file stream

void init_logging() { // Function to initialize logging
    // Ensure log directory exists
    try { // Try block to handle exceptions
        if (!fs::create_directories("logs")) { // Create logs directory
            std::cerr << "Log directory already exists or failed to create." << std::endl; // Error message
        }
    } catch (const fs::filesystem_error& e) { // Catch filesystem errors
        std::cerr << "Filesystem error: " << e.what() << std::endl; // Error message
        exit(EXIT_FAILURE);  // Exit if the log directory can't be created
    }

    // Start logging to a file inside the logs directory
    log_file.open("logs/crypto_log.txt", std::ios::out | std::ios::app);  // Open in append mode

    if (!log_file.is_open()) { // Check if the log file is open
        std::cerr << "Failed to open log file." << std::endl; // Error message
        exit(EXIT_FAILURE);  // Exit if the log file can't be opened
    }

    log_file << "Logging started." << std::endl; // Log the start of logging
    log_file.flush();  // Flush immediately after writing
}

namespace lattice_crypto { // Start of lattice_crypto namespace

// Modular exponentiation function with base, exponent, and modulo
int mod_exp(int base, int exp, int mod) { // Function to calculate modular exponentiation
    log_file << "Modular exponentiation started: base=" << base << ", exp=" << exp << ", mod=" << mod << std::endl; // Log the start of modular exponentiation  
    log_file.flush();  // Flush the log after each log entry
    int result = 1; // Initialize result to 1
    base %= mod;  // Always reduce base modulo
    while (exp > 0) { // Loop until exponent is greater than 0
        if (exp & 1) result = (1LL * result * base) % mod; // If exponent is odd, multiply result by base and take modulo
        base = (1LL * base * base) % mod; // Square the base and take modulo
        exp >>= 1; // Right shift the exponent
    }
    log_file << "Modular exponentiation result: " << result << std::endl; // Log the result of modular exponentiation
    log_file.flush(); // Flush the log after each log entry
    return result; // Return the result
}

// Helper function to reverse bits of a number up to 'log_n' bits
int reverse_bits(int num, int log_n) { // Function to reverse bits of a number
    log_file << "Reversing bits for number: " << num << ", log_n: " << log_n << std::endl; // Log the start of bit reversal
    int reversed = 0; // Initialize reversed number to 0
    for (int i = 0; i < log_n; ++i) { // Loop through each bit
        if (num & (1 << i)) reversed |= 1 << (log_n - 1 - i); // Reverse the bit
    }
    log_file << "Reversed bits result: " << reversed << std::endl; // Log the result of bit reversal
    return reversed; // Return the reversed number
}

// Number Theoretic Transform (NTT) implementation for polynomial multiplication
std::vector<int> ntt(const std::vector<int>& a, int n, int root, int mod) { // Function to perform Number Theoretic Transform
    log_file << "Starting NTT for vector of size " << a.size() << " with n: " << n << ", root: " << root << ", mod: " << mod << std::endl; // Log the start of NTT
    std::vector<int> A(n); // Initialize vector A with size n
    int log_n = std::log2(n); // Calculate log base 2 of n
    for (int i = 0; i < n; ++i) { // Loop through each element of A
        A[i] = a[reverse_bits(i, log_n)]; // Reverse the bits and assign the value
    }
    for (int length = 2; length <= n; length <<= 1) { // Loop through each length
        int w_length = mod_exp(root, (mod - 1) / length, mod); // Calculate w_length
        for (int i = 0; i < n; i += length) { // Loop through each element of A
            int w = 1; // Initialize w to 1
            for (int j = 0; j < length / 2; ++j) { // Loop through each element of A
                int u = A[i + j]; // Assign value to u
                int v = (1LL * A[i + j + length / 2] * w) % mod; // Calculate v
                A[i + j] = (u + v) % mod; // Assign value to A[i + j]
                A[i + j + length / 2] = (u - v + mod) % mod; // Assign value to A[i + j + length / 2]
                w = (1LL * w * w_length) % mod; // Update w
            }
        }
    }
    log_file << "NTT completed." << std::endl; // Log the completion of NTT
    return A; // Return the transformed vector
}

// Inverse NTT
std::vector<int> intt(const std::vector<int>& a, int n, int inv_root, int mod) { // Function to perform Inverse Number Theoretic Transform
    log_file << "Starting inverse NTT..." << std::endl; // Log the start of inverse NTT
    std::vector<int> A = ntt(a, n, inv_root, mod); // Perform NTT with inverse root
    int inv_n = mod_exp(n, mod - 2, mod);  // Fermat's Little Theorem
    for (int &x : A) { // Loop through each element of A
        x = (1LL * x * inv_n) % mod; // Calculate the inverse NTT
    } // End of loop
    log_file << "Inverse NTT completed." << std::endl; // Log the completion of inverse NTT
    return A; // Return the transformed vector
}

// Secure random matrix generation using OpenSSL's cryptographic RNG
Eigen::MatrixXi KeyGenerator::generate_random_matrix(int rows, int cols) { // Function to generate random matrix
    log_file << "Generating random matrix with dimensions: " << rows << "x" << cols << std::endl; // Log the matrix generation
    Eigen::MatrixXi mat(rows, cols); // Initialize matrix with given dimensions
    
    for (int i = 0; i < rows; ++i) { // Loop through each row
        for (int j = 0; j < cols; ++j) { // Loop through each column
            unsigned char rand_byte; // Initialize random byte
            if (RAND_bytes(&rand_byte, sizeof(rand_byte)) != 1) { // Generate random byte
                log_file << "Error generating secure random bytes." << std::endl; // Log error
                throw std::runtime_error("Error generating secure random bytes."); // Throw runtime error
            }
            mat(i, j) = rand_byte % 2;  // Generate either 0 or 1 for this example
        }
    }
    
    log_file << "Random matrix generated." << std::endl; // Log the completion of matrix generation
    return mat; // Return the generated matrix
}

// Binomial distribution-based error generation
Eigen::MatrixXi KeyGenerator::generate_binomial_error(int rows, int cols) { // Function to generate binomial error matrix
    log_file << "Generating binomial error matrix with dimensions: " << rows << "x" << cols << std::endl; // Log the error matrix generation
    Eigen::MatrixXi mat(rows, cols); // Initialize matrix with given dimensions
    std::random_device rd; // Random device for seeding
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::binomial_distribution<int> binomial(10, 0.5);  // Parameters: trials=10, probability=0.5

    for (int i = 0; i < rows; ++i) { // Loop through each row
        for (int j = 0; j < cols; ++j) { // Loop through each column
            mat(i, j) = binomial(gen) - 5;  // Center the distribution around 0
        } // End of column loop
    }
    
    log_file << "Binomial error matrix generated." << std::endl; // Log the completion of error matrix generation
    return mat; // Return the generated error matrix
}

// Convert matrix to hex
std::string matrix_to_hex(const Eigen::MatrixXi& matrix) { // Function to convert matrix to hex
    std::stringstream hex_stream; // Initialize hex stream
    for (int i = 0; i < matrix.rows(); ++i) { // Loop through each row
        for (int j = 0; j < matrix.cols(); ++j) { // Loop through each column
            unsigned char value = static_cast<unsigned char>(matrix(i, j)); // Get the value
            hex_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value); // Convert to hex and append
        } // End of column loop
    } // End of row loop
    log_file << "Matrix converted to hex: " << hex_stream.str() << std::endl; // Log the hex conversion
    return hex_stream.str(); // Return the hex string
} // End of function

// NTT-based polynomial multiplication
Eigen::MatrixXi KeyGenerator::polynomial_multiply(const Eigen::MatrixXi& a, const Eigen::MatrixXi& b, int q) { // Function to perform polynomial multiplication
    log_file << "Polynomial multiplication started..." << std::endl; // Log the start of polynomial multiplication
    if (a.cols() != b.rows()) { // Check if dimensions are compatible
        log_file << "Matrix dimensions are not compatible for multiplication." << std::endl; // Log the error
        throw std::runtime_error("Matrix dimensions are not compatible for multiplication."); // Throw runtime error
    } // End of check
    int resultSize = 1; // Initialize result size to 1
    while (resultSize < a.cols() + b.rows() - 1) { // Loop until result size is large enough
        resultSize <<= 1;  // Ensure resultSize is a power of 2
    }

    std::vector<int> a_values(resultSize, 0); // Initialize vector for a values
    std::vector<int> b_values(resultSize, 0); // Initialize vector for b values

    for (int i = 0; i < a.cols(); ++i) { // Loop through each column of a
        a_values[i] = a(0, i) % q; // Assign value to a_values
    } // End of loop
    for (int i = 0; i < b.rows(); ++i) { // Loop through each row of b
        b_values[i] = b(i, 0) % q; // Assign value to b_values
    } // End of loop

    int root = 3;  // Primitive root
    int inv_root = mod_exp(root, q - 2, q);  // Modular inverse of the root

    std::vector<int> a_ntt = ntt(a_values, resultSize, root, q); // Perform NTT on a
    std::vector<int> b_ntt = ntt(b_values, resultSize, root, q); // Perform NTT on b

    std::vector<int> result_ntt(resultSize, 0); // Initialize vector for result
    for (int i = 0; i < resultSize; ++i) { // Loop through each element
        result_ntt[i] = (1LL * a_ntt[i] * b_ntt[i]) % q; // Calculate the result
    }

    std::vector<int> result = intt(result_ntt, resultSize, inv_root, q); // Perform inverse NTT on the result

    Eigen::MatrixXi result_int(1, resultSize); // Initialize matrix for result
    for (int i = 0; i < resultSize; ++i) { // Loop through each element
        result_int(0, i) = result[i] % q; // Assign value to result_int
        if (result_int(0, i) < 0) { // Check if value is negative
            result_int(0, i) += q;  // Ensure values are positive
        }
    }
    log_file << "Polynomial multiplication completed." << std::endl; // Log the completion of polynomial multiplication
    return result_int; // Return the result matrix
}

// Modify the dimensions of the public key to match poly_degree (512)
std::pair<Eigen::MatrixXi, Eigen::MatrixXi> KeyGenerator::generate_keys(int rows, int cols, int q) { // Function to generate keys
    log_file << "Key generation started..." << std::endl; // Log the start of key generation

    // Generate secret key with dimensions rows x cols (both set to poly_degree) and random values
    Eigen::MatrixXi secret_key = generate_random_matrix(rows, cols); // Generate random matrix for secret key

    // Adjust public_key dimensions to match poly_degree
    Eigen::MatrixXi public_key_first = polynomial_multiply(secret_key, generate_random_matrix(rows, cols), q); // Generate first part of public key
    Eigen::MatrixXi public_key_second = polynomial_multiply(secret_key, generate_random_matrix(rows, cols), q); // Generate second part of public key

    log_file << "Secret Key (hex): \n" << matrix_to_hex(secret_key) << std::endl; // Log the secret key
    log_file << "Public Key (first part, hex): \n" << matrix_to_hex(public_key_first) << std::endl;     // Log the first part of public key
    log_file << "Public Key (second part, hex): \n" << matrix_to_hex(public_key_second) << std::endl;  // Log the second part of public key

    log_file << "Key generation completed." << std::endl; // Log the completion of key generation

    return {public_key_first, public_key_second}; // Return the public key pair
}

// Constructor for RingLWECrypto
RingLWECrypto::RingLWECrypto(int poly_degree, int modulus) // Constructor with default parameters
    : poly_degree(poly_degree), q(modulus), key_gen(std::unique_ptr<KeyGenerator>(new KeyGenerator())) { // Initialize member variables
    log_file << "Initializing RingLWE Crypto with polynomial degree: " << poly_degree << ", modulus: " << modulus << std::endl; // Log the initialization
    secret_key = key_gen->generate_random_matrix(poly_degree, poly_degree); // Generate secret key
    std::pair<Eigen::MatrixXi, Eigen::MatrixXi> public_key_pair = key_gen->generate_keys(poly_degree, poly_degree, q); // Generate public key pair

    public_key.first = public_key_pair.first; // Assign first part of public key
    public_key.second = public_key_pair.second; // Assign second part of public key

    log_file << "Secret Key (hex): \n" << matrix_to_hex(secret_key) << std::endl; // Log the secret key
    log_file << "Public Key (first part, hex): \n" << matrix_to_hex(public_key.first) << std::endl; // Log the first part of public key
    log_file << "Public Key (second part, hex): \n" << matrix_to_hex(public_key.second) << std::endl; // Log the second part of public key
}

std::pair<Eigen::MatrixXi, Eigen::MatrixXi> RingLWECrypto::encrypt(const std::string& plaintext) { // Function to encrypt plaintext
    log_file << "Starting encryption for plaintext: " << plaintext << std::endl; // Log the start of encryption

    // Create the plaintext matrix with size based on poly_degree
    Eigen::MatrixXi plaintext_matrix(1, poly_degree);  // poly_degree = 512
    for (size_t i = 0; i < plaintext.size(); ++i) { // Loop through each character of plaintext
        plaintext_matrix(0, i) = static_cast<int>(plaintext[i]); // Assign the ASCII value to the matrix
    }

    // Fill remaining part of the matrix with zeros if plaintext size < poly_degree
    if (plaintext.size() < static_cast<size_t>(poly_degree)) { // Check if plaintext size is less than poly_degree
        for (size_t i = plaintext.size(); i < static_cast<size_t>(poly_degree); ++i) { // Loop through remaining elements
            plaintext_matrix(0, i) = 0; // Fill with zeros
        }
    }

    // Generate the error matrix
    Eigen::MatrixXi error_matrix = key_gen->generate_binomial_error(1, poly_degree); // Generate error matrix

    // Log the dimensions for debugging
    log_file << "public_key.first dimensions: " << public_key.first.rows() << " x " << public_key.first.cols() << std::endl; // Log the dimensions of public_key.first
    log_file << "plaintext_matrix dimensions: " << plaintext_matrix.rows() << " x " << plaintext_matrix.cols() << std::endl; // Log the dimensions of plaintext_matrix
    log_file << "plaintext_matrix.transpose() dimensions: " << plaintext_matrix.transpose().rows() << " x " << plaintext_matrix.transpose().cols() << std::endl; // Log the dimensions of plaintext_matrix.transpose()

    // Check dimensions before performing multiplication
    if (public_key.first.cols() == plaintext_matrix.transpose().rows()) {   // Check if dimensions are compatible
        Eigen::MatrixXi c1 = key_gen->polynomial_multiply(public_key.first, plaintext_matrix.transpose(), q).transpose(); // Perform polynomial multiplication
        Eigen::MatrixXi c2 = plaintext_matrix + key_gen->polynomial_multiply(public_key.second, error_matrix.transpose(), q).transpose(); // Perform polynomial multiplication

        log_file << "Plaintext matrix (hex): " << matrix_to_hex(plaintext_matrix) << std::endl; // Log the plaintext matrix
        log_file << "Ciphertext c1 (hex): " << matrix_to_hex(c1) << std::endl; // Log the first part of ciphertext
        log_file << "Ciphertext c2 (hex): " << matrix_to_hex(c2) << std::endl; // Log the second part of ciphertext

        return {c1, c2}; // Return the ciphertext pair
    } else { // Dimensions are not compatible
        log_file << "Error: Matrix dimensions are not compatible for multiplication." << std::endl; // Log the error
        log_file << "public_key.first.cols(): " << public_key.first.cols() << ", plaintext_matrix.transpose().rows(): " << plaintext_matrix.transpose().rows() << std::endl;  // Log the dimensions
        throw std::runtime_error("Matrix dimensions are not compatible for multiplication."); // Throw runtime error
    }
}
// Decryption
std::string RingLWECrypto::decrypt(const std::pair<Eigen::MatrixXi, Eigen::MatrixXi>& ciphertext) { // Function to decrypt ciphertext
    log_file << "Starting decryption..." << std::endl; // Log the start of decryption
    Eigen::MatrixXi c1 = ciphertext.first; // Assign first part of ciphertext
    Eigen::MatrixXi c2 = ciphertext.second; // Assign second part of ciphertext

    Eigen::MatrixXi m = key_gen->polynomial_multiply(c1, secret_key, q); // Perform polynomial multiplication
    m = c2 - m; // Subtract the result from c2

    std::stringstream hex_stream; // Initialize hex stream
    bool found_padding = false; // Initialize padding flag
    for (int i = 0; i < m.cols(); ++i) { // Loop through each column
        unsigned char decrypted_byte = static_cast<unsigned char>(m(0, i)); // Get the decrypted byte
        if (decrypted_byte == 0 || decrypted_byte == 0xff) { // Check for padding
            found_padding = true; // Set padding flag
            break; // Exit the loop
        }
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(decrypted_byte); // Convert to hex and append
    }

    if (found_padding) { // Check if padding was found
        log_file << "Padding found and stripped from the decrypted message." << std::endl; // Log the padding
    }

    log_file << "Decrypted hex: " << hex_stream.str() << std::endl; // Log the decrypted hex

    return hex_stream.str(); // Return the decrypted hex string
}

}  // namespace lattice_crypto

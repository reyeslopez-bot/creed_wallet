#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "lattice_crypto.h"  // Include the lattice crypto header for RingLWECrypto

namespace fs = std::filesystem;

// Initialize logging mechanism
std::ofstream log_file;

void init_logging() {
    // Ensure the logs directory exists
    try {
        if (!fs::create_directories("logs")) {
            std::cerr << "Log directory already exists or failed to create." << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);  // Exit if the log directory can't be created
    }

    // Open the log file for writing
    log_file.open("logs/crypto_log.txt", std::ios::out | std::ios::app);  // Open in append mode
    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
        exit(EXIT_FAILURE);  // Exit if the log file can't be opened
    }

    log_file << "Logging initialized." << std::endl;
    log_file.flush();  // Flush the log file to ensure the entry is written
}

// Function to convert a string into its hex representation
std::string to_hex(const std::string& input) {
    std::stringstream hex_stream;
    for (unsigned char c : input) {
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return hex_stream.str();
}

int main() {
    // Initialize logging
    init_logging();

    // Initialize the RingLWECrypto system with polynomial degree and modulus
    int poly_degree = 512;  // Example polynomial degree
    int modulus = 2048;     // Example modulus (q)

    // Instantiate the RingLWECrypto object
    lattice_crypto::RingLWECrypto ringLWE(poly_degree, modulus);

    // Example plaintext message
    std::string plaintext = "Hello, Quantum World!";

    try {
        // Encrypt the plaintext
        auto ciphertext = ringLWE.encrypt(plaintext);

        // Decrypt the ciphertext
        std::string decrypted_text = ringLWE.decrypt(ciphertext);

        // Convert the original plaintext to hex for comparison
        std::string plaintext_hex = to_hex(plaintext);

        // Check if the decrypted hex matches the original hex-encoded plaintext
        if (decrypted_text == plaintext_hex) {
            std::cout << "Encryption/Decryption successful! Decrypted text matches the original." << std::endl;
        } else {
            std::cout << "Error: Decrypted text does not match the original." << std::endl;
            std::cout << "Original: " << plaintext_hex << std::endl;
            std::cout << "Decrypted: " << decrypted_text << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "An error occurred during encryption/decryption: " << e.what() << std::endl;
    }

    // Close the log file when done
    if (log_file.is_open()) {
        log_file.close();
    }

    return 0;
}

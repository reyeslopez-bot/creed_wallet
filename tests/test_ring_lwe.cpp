#include <iostream>
#include "lattice_crypto.h"  // Include the lattice crypto header for RingLWECrypto
#include <iomanip>  // For hex formatting

// Function to convert a string into its hex representation
std::string to_hex(const std::string& input) {
    std::stringstream hex_stream;
    for (unsigned char c : input) {
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return hex_stream.str();
}

int main() {
    // Initialize the RingLWECrypto system with polynomial degree and modulus
    int poly_degree = 512;  // Example polynomial degree
    int modulus = 2048;     // Example modulus (q)

    // Instantiate the RingLWECrypto object
    lattice_crypto::RingLWECrypto ringLWE(poly_degree, modulus);

    // Example plaintext message
    std::string plaintext = "Hello, Quantum World!";

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

    return 0;
}

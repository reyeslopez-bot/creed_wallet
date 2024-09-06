from lattice_crypto import RingLWECrypto  # Correct import

# Initialize the crypto system with your desired parameters
crypto = RingLWECrypto(512, 4096)

# Encrypt a sample message
message = "Hello, World!"
encrypted_data = crypto.encrypt(message)
print(f"Encrypted: {encrypted_data}")

# Decrypt the message
decrypted_data = crypto.decrypt(encrypted_data)
print(f"Decrypted: {decrypted_data}")

# Ensure the decrypted message matches the original
assert decrypted_data == message, "Decryption failed: message does not match!"
print("Decryption successful!")

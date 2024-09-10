# We need to add the path to the source code to the system path
# so that we can import the RingLWECrypto class from lattice_crypto.py.
# This is done in wallet_operations.py.
# test_crypto.py
import sys
import os

# Adjust the system path to include the source directory
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../src')))

# Import the RingLWECrypto class from the lattice_crypto module
from lattice_crypto import RingLWECrypto  # Correct import

# Initialize the crypto system with your desired parameters
crypto = RingLWECrypto(512, 4096)

# Encrypt a sample message
message = "Hello, World!"
message_bytes = message.encode('utf-8')  # Convert string to bytes
encrypted_data = crypto.encrypt(message_bytes)
print(f"Encrypted: {encrypted_data}")

# Decrypt the message
decrypted_data_bytes = crypto.decrypt(encrypted_data)
decrypted_data = decrypted_data_bytes.decode('utf-8')  # Convert bytes back to string
print(f"Decrypted: {decrypted_data}")

# Ensure the decrypted message matches the original
assert decrypted_data == message, "Decryption failed: message does not match!"
print("Decryption successful!")

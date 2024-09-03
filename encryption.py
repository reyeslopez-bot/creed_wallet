# encryption.py
import os
import logging
from cryptography.fernet import Fernet

# Configure logging
logging.basicConfig(
    filename='encryption.log',
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

class Encrypt:
    @staticmethod
    def generate_key():
        """Generate a new Fernet key and save it to a file."""
        key = Fernet.generate_key()
        with open('secret.key', 'wb') as key_file:
            key_file.write(key)
        logging.info("New encryption key generated and saved to secret.key")
        return key

    @staticmethod
    def load_key():
        """Load the Fernet key from a file or generate a new one if it doesn't exist."""
        if os.path.exists('secret.key'):
            with open('secret.key', 'rb') as key_file:
                key = key_file.read()
            logging.info("Encryption key loaded from secret.key")
            return key
        else:
            logging.warning("secret.key not found. Generating a new key.")
            return Encrypt.generate_key()

    @staticmethod
    def encrypt_message(key, message):
        """Encrypt a message using the provided Fernet key."""
        if isinstance(message, str):
            message = message.encode('utf-8')
        elif not isinstance(message, bytes):
            raise TypeError("Message must be a string or bytes")
        
        f = Fernet(key)
        encrypted_message = f.encrypt(message)
        logging.info("Message encrypted successfully.")
        return encrypted_message

    @staticmethod
    def decrypt_message(key, encrypted_message):
        """Decrypt a message using the provided Fernet key."""
        f = Fernet(key)
        try:
            decrypted_message = f.decrypt(encrypted_message).decode()
            logging.info("Message decrypted successfully.")
            return decrypted_message
        except Exception as e:
            logging.error(f"Failed to decrypt message: {e}")
            return None

from web3 import Web3
from eth_account import Account
import logging
from variables import INFURA_ENDPOINT
from encryption import Encrypt
from lattice_crypto import RingLWECrypto  # Import the RingLWECrypto class

def create_wallet():
    """Create a new Ethereum wallet and encrypt its private key."""
    try:
        # Initialize Web3 connection to Infura
        logging.info(f"Attempting to connect to Infura with endpoint: {INFURA_ENDPOINT}")
        w3 = Web3(Web3.HTTPProvider(INFURA_ENDPOINT))
        if not w3.is_connected():
            logging.error("Failed to connect to Infura's Ethereum mainnet.")
            raise ConnectionError("Failed to connect to Infura's Ethereum mainnet.")
        logging.info("Successfully connected to Infura's Ethereum mainnet.")
        print("Successfully connected to Infura's Ethereum mainnet.")

        # Generate a new Ethereum account
        acct = Account.create()
        logging.info(f"New Ethereum account created. Address: {acct.address}")
        print(f"New Ethereum account created. Address: {acct.address}")

        # Load or generate encryption key
        encryption_key = Encrypt.load_key()
        logging.info("Encryption key loaded successfully.")

        # Encrypt the private key using Fernet
        encrypted_private_key = Encrypt.encrypt_message(encryption_key, acct.privateKey.hex())
        logging.info("Private key encrypted successfully with Fernet.")

        # Additional encryption using lattice-based cryptography
        lattice_crypt = RingLWECrypto(512, 4096)
        encrypted_lattice_private_key = lattice_crypt.encrypt(encrypted_private_key.decode())
        logging.info("Private key additionally encrypted using lattice-based cryptography.")

        return acct, encrypted_lattice_private_key

    except ConnectionError as ce:
        logging.error(f"Connection error: {ce}")
        raise

    except Exception as e:
        logging.error(f"An unexpected error occurred during wallet creation: {e}")
        raise

from web3 import Web3
from eth_account import Account
import logging
from variables import INFURA_ENDPOINT
from encryption import Encrypt
from lattice_crypto import RingLWECrypto  # Import the RingLWECrypto class

# Configure logging
logging.basicConfig(
    filename='wallet_create.log',
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)

def create_wallet():
    """Create a new Ethereum wallet and encrypt its private key."""
    try:
        # Initialize Web3 connection to Infura
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

        # Print available attributes for debugging
        print(dir(acct))

        # Load or generate encryption key
        encryption_key = Encrypt.load_key()

        # Attempt to access the private key (try both possible attribute names)
        try:
            encrypted_private_key = Encrypt.encrypt_message(encryption_key, acct.private_key.hex())
        except AttributeError:
            encrypted_private_key = Encrypt.encrypt_message(encryption_key, acct.key.hex())

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
        logging.error(f"An error occurred during wallet creation: {e}")
        raise

import sys
import os
import logging
import base64
from eth_account import Account

# Adjust path to make sure 'src/' can be found for 'lattice_crypto'
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../src')))

# Now you can import 'lattice_crypto'
from lattice_crypto import RingLWECrypto
from encryption import Encrypt  # Assuming encryption is in the same src folder

def create_wallet():
    from variables import INFURA_ENDPOINT
    from web3 import Web3
    
    try:
        w3 = Web3(Web3.HTTPProvider(INFURA_ENDPOINT))
        if not w3.is_connected():
            logging.error("Failed to connect to Infura's Ethereum mainnet.")
            raise ConnectionError("Failed to connect to Infura's Ethereum mainnet.")
        logging.info("Successfully connected to Infura's Ethereum mainnet.")
        print("Successfully connected to Infura's Ethereum mainnet.")

        acct = Account.create()
        logging.info(f"New Ethereum account created. Address: {acct.address}")
        print(f"New Ethereum account created. Address: {acct.address}")

        # Return the account object without exposing the private key directly
        return acct
    except Exception as e:
        logging.error(f"An error occurred during wallet creation: {e}")
        raise

def encrypt_private_key(acct):
    encryption_key = Encrypt.load_key()
    private_key_bytes = bytes(acct._private_key)

    encrypted_private_key = Encrypt.encrypt_message(encryption_key, private_key_bytes)
    encrypted_private_key_b64 = base64.b64encode(encrypted_private_key).decode('utf-8')
    
    lattice_crypt = RingLWECrypto(512, 4096)
    encrypted_lattice = lattice_crypt.encrypt(encrypted_private_key)
    encrypted_lattice_b64 = base64.b64encode(encrypted_lattice).decode('utf-8')
    
    return encrypted_private_key_b64, encrypted_lattice_b64

def verify_lattice_encryption(encrypted_private_key_b64, encrypted_lattice_b64):
    lattice_crypto = RingLWECrypto(512, 4096)
    encrypted_lattice = base64.b64decode(encrypted_lattice_b64)
    decrypted_lattice = lattice_crypto.decrypt(encrypted_lattice)
    decrypted_lattice_b64 = base64.b64encode(decrypted_lattice).decode('utf-8')

    if decrypted_lattice_b64 != encrypted_private_key_b64:
        logging.error("Lattice-based decryption failed.")
        return False
    else:
        logging.info("Lattice-based decryption succeeded.")
        return True

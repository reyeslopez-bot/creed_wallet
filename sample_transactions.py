import os
import logging
from web3 import Web3
from eth_account import Account
from web3.exceptions import TransactionNotFound
from encryption import encrypt_data, decrypt_data  # Ensure these functions are defined
from variables import acct, INFURA_ENDPOINT
from lattice_crypto import encrypt_lattice, decrypt_lattice  # Ensure these functions are defined
from wallet_operations import main as wallet_create
from logging_setup import setup_logging

# Initialize logging
setup_logging()  # Call your custom logging setup first if it does more than basicConfig
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Initialize Web3 connection
w3 = Web3(Web3.HTTPProvider(INFURA_ENDPOINT))

def connect_to_provider():
    if w3.is_connected():
        logger.info("Connected to Infura")
    else:
        logger.error("Failed to connect to Infura")
        raise ConnectionError("Unable to connect to Infura")

def get_balance(account):
    try:
        balance = w3.eth.get_balance(account.address)
        logger.info(f'Balance: {w3.from_wei(balance, "ether")} ETH')
        return balance
    except Exception as e:
        logger.error(f"Error retrieving balance: {e}")
        raise

def create_transaction(account, to_address, value_ether):
    try:
        nonce = w3.eth.get_transaction_count(account.address)
        transaction = {
            'to': to_address,
            'value': w3.to_wei(value_ether, 'ether'),
            'gas': 21000,  # Standard gas limit for a simple transfer
            'gasPrice': w3.to_wei('50', 'gwei'),
            'nonce': nonce,
            'chainId': 1  # Mainnet chain ID
        }
        logger.info("Transaction created successfully.")
        return transaction
    except Exception as e:
        logger.error(f"Error creating transaction: {e}")
        raise

def encrypt_transaction(transaction):
    try:
        transaction_str = str(transaction)
        encrypted_transaction = encrypt_lattice(transaction_str)
        logger.info("Transaction data encrypted successfully.")
        return encrypted_transaction
    except Exception as e:
        logger.error(f"Error encrypting transaction: {e}")
        raise

def sign_transaction(account, encrypted_transaction):
    try:
        signed_txn = w3.eth.account.sign_transaction(encrypted_transaction, account._private_key)
        logger.info("Transaction signed successfully.")
        return signed_txn
    except Exception as e:
        logger.error(f"Error signing transaction: {e}")
        raise

def send_transaction(signed_txn):
    try:
        txn_hash = w3.eth.send_raw_transaction(signed_txn.raw_transaction)
        logger.info(f'Transaction hash: {txn_hash.hex()}')
        return txn_hash
    except Exception as e:
        logger.error(f"Error sending transaction: {e}")
        raise

def wait_for_receipt(txn_hash):
    try:
        receipt = w3.eth.wait_for_transaction_receipt(txn_hash, timeout=120)
        logger.info(f'Transaction receipt: {receipt}')
        return receipt
    except TransactionNotFound:
        logger.warning("Transaction not found in the block after waiting.")
    except Exception as e:
        logger.error(f"Error waiting for transaction receipt: {e}")
        raise

def main():
    connect_to_provider()
    get_balance(acct)

    receiver_address = '0xA930Ecf9B55333d2631eE787c4E6f25153dB70bA'  # Example address
    transaction = create_transaction(acct, receiver_address, 0.01)
    
    encrypted_transaction = encrypt_transaction(transaction)
    
    # Optional: Decrypt to verify integrity before signing (for test purposes)
    try:
        decrypted_transaction = decrypt_lattice(encrypted_transaction)
        assert str(transaction) == decrypted_transaction, "Data integrity check failed!"
        logger.info("Data integrity verified.")
    except Exception as e:
        logger.error(f"Data integrity verification failed: {e}")
        raise

    signed_txn = sign_transaction(acct, encrypted_transaction)
    txn_hash = send_transaction(signed_txn)
    wait_for_receipt(txn_hash)

    wallet_create()
    logger.info("Transaction completed successfully.")

if __name__ == "__main__":
    main()

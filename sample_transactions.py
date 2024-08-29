# sample_transactions.py
# Import the required libraries
from web3 import Web3
from eth_account import Account
from web3.exceptions import TransactionNotFound
import os
import logging
import wallet_create
from web3.exceptions import TransactionNotFound
from variables import acct, INFURA_ENDPOINT

w3 = Web3(Web3.HTTPProvider(INFURA_ENDPOINT))

if w3.is_connected():
    print("Connected to Infura")
else:
    print("Failed to connect to Infura")

# Check account balance
try:
    balance = w3.eth.get_balance(acct.address)
    print(f'Balance: {w3.from_wei(balance, "ether")} ETH')
except Exception as e:
    print(f"Error retrieving balance: {e}")

# Prepare a transaction (Remember to replace '0x123...' with an actual Ethereum address)
receiver_address = '0xA930Ecf9B55333d2631eE787c4E6f25153dB70bA'  # Example address
transaction = {
    'to': receiver_address,
    'value': w3.to_wei(0.01, 'ether'),
    'gas': 21000,  # Standard gas limit for a simple transfer
    'gasPrice': w3.to_wei('50', 'gwei'),
    'nonce': w3.eth.get_transaction_count(acct.address),
    'chainId': 1  # Mainnet chain ID
}
signed_txn = w3.eth.account.sign_transaction(transaction, acct._private_key)
txn_hash = w3.eth.send_raw_transaction(signed_txn.raw_transaction)
receipt = w3.eth.wait_for_transaction_receipt(txn_hash, timeout=120)

# Sign the transaction
try:
    signed_txn = w3.eth.account.sign_transaction(transaction, acct._private_key)
except Exception as e:
    print(f"Error signing transaction: {e}")
    raise

# Send the transaction
try:
    txn_hash = w3.eth.send_raw_transaction(signed_txn.raw_transaction)
    print(f'Transaction hash: {txn_hash.hex()}')
except Exception as e:
    print(f"Error sending transaction: {e}")
    raise

# Optionally, wait for transaction receipt
try:
    receipt = w3.eth.wait_for_transaction_receipt(txn_hash, timeout=120)
    print(f'Transaction receipt: {receipt}')
except TransactionNotFound:
    print("Transaction not found in the block after waiting.")
except Exception as e:
    print(f"Error waiting for transaction receipt: {e}")

wallet_create.main()
print("Transaction completed successfully.")


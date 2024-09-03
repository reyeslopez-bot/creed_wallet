# main.py
import logging
from wallet_operations import create_wallet, encrypt_private_key, verify_lattice_encryption
from build_operations import build_project
from logging_setup import setup_logging
from arguments import parse_arguments

def main():
    args = parse_arguments()
    setup_logging()
    
    if not build_project(args.build_dir):
        logging.error("Build process failed. Exiting main process.")
        return

    try:
        # Create the wallet and get the encrypted private key
        acct, private_key_bytes = create_wallet()
        logging.info(f"Wallet address: {acct.address}")
        print(f"Wallet address: {acct.address}")

        # Encrypt the private key
        encrypted_private_key_b64, encrypted_lattice_b64 = encrypt_private_key(acct)

        # Verify the lattice-based encryption
        if verify_lattice_encryption(encrypted_private_key_b64, encrypted_lattice_b64):
            logging.info("Wallet created and keys encrypted successfully.")
            print("Wallet created and keys encrypted successfully.")
        else:
            logging.error("Failed to verify encryption.")
            print("Failed to verify encryption.")
    except Exception as e:
        logging.error(f"Failed to create wallet: {e}")
        print(f"Failed to create wallet: {e}")

import logging
import time
import subprocess
import os
from logging_setup import setup_logging
from encryption import Encrypt
import variables
from lattice_crypto import RingLWECrypto
from wallet_create import create_wallet

# Constants for rate limiting
LAST_WALLET_CREATION_TIME = 0
WALLET_CREATION_INTERVAL = 60 * 5  # 5 minutes in seconds

def run_command(command, cwd=None):
    """Run a shell command and log its output."""
    try:
        cwd_display = cwd or os.getcwd()
        logging.info(f"Running command: {' '.join(command)} in {cwd_display}")
        result = subprocess.run(command, cwd=cwd, check=True, text=True, capture_output=True)
        logging.info(f"Command output:\n{result.stdout}")
        if result.stderr:
            logging.warning(f"Command stderr:\n{result.stderr}")
    except subprocess.CalledProcessError as e:
        logging.error(f"Command '{' '.join(command)}' failed with return code {e.returncode}")
        logging.error(f"Command output:\n{e.stdout}")
        logging.error(f"Command error output:\n{e.stderr}")
        raise e
    except Exception as e:
        logging.exception(f"Unexpected error while running command: {' '.join(command)}")
        raise e

def clean_build_directory(build_dir='build'):
    """Clean up the build directory if it exists."""
    if os.path.exists(build_dir):
        logging.info(f"Cleaning previous build directory: {build_dir}")
        run_command(['rm', '-rf', build_dir])

def create_build_directory(build_dir='build'):
    """Create a new build directory."""
    try:
        logging.info(f"Creating new build directory: {build_dir}")
        os.mkdir(build_dir)
    except OSError as e:
        logging.error(f"Failed to create build directory {build_dir}: {e}")
        raise e

def check_makefile(build_dir='build'):
    """Check if the Makefile exists in the build directory."""
    makefile_path = os.path.join(build_dir, 'Makefile')
    if not os.path.exists(makefile_path):
        logging.error(f"Makefile not found in {build_dir}. Build cannot proceed.")
        raise FileNotFoundError(f"Makefile not found in {build_dir}")

def build_project():
    """Build the C++ project using CMake and Make."""
    logging.info("Starting the build process...")
    try:
        clean_build_directory()
        create_build_directory()

        logging.info("Running CMake...")
        # Ensure CMake is instructed to generate the Makefile in the 'build' directory
        run_command(['cmake', f'-DCMAKE_PREFIX_PATH={os.popen("brew --prefix pybind11").read().strip()}', '-S', '.', '-B', 'build'], cwd='.')

        # Check if Makefile was generated
        check_makefile()

        logging.info("Running Make...")
        run_command(['make'], cwd='build')

        logging.info("Build completed successfully.")
    except Exception as e:
        logging.error(f"Build process failed: {e}")
        print("Build process failed. Please check the logs for details.")
        raise e

def create_wallet_with_rate_limit():
    """Create a new wallet if the rate limit allows."""
    global LAST_WALLET_CREATION_TIME
    current_time = time.time()

    if current_time - LAST_WALLET_CREATION_TIME < WALLET_CREATION_INTERVAL:
        logging.info("Wallet creation rate limit hit. Returning existing wallet address.")
        print(f"Wallet creation is limited. Current address: {variables.acct.address}")
        return variables.acct

    # Proceed to create a new wallet
    acct, encrypted_private_key = create_wallet()
    LAST_WALLET_CREATION_TIME = current_time
    return acct

def main():
    """Main function to build the project and create a wallet."""
    setup_logging()  # Initialize logging
    logging.info("Starting main function...")

    try:
        build_project()  # Build the project
    except Exception as e:
        logging.error(f"Build process failed and main process aborted: {e}")
        return  # Abort the rest of the process if build fails

    try:
        # Create wallet with rate limiting
        acct = create_wallet_with_rate_limit()
        logging.info(f"Wallet address: {acct.address}")
        print(f"Wallet address: {acct.address}")

        # Load or generate the encryption key for Fernet
        encryption_key = Encrypt.load_key()

        # Encrypt the private key using Fernet encryption
        private_key = acct._private_key.hex()
        encrypted_private_key = Encrypt.encrypt_message(encryption_key, private_key)

        # Store the encrypted private key
        variables.encrypted_private_key = encrypted_private_key
        logging.info(f"Encrypted private key: {encrypted_private_key}")

        # Utilize lattice-based encryption for further security
        lattice_crypt = RingLWECrypto(512, 4096)
        encrypted_lattice = lattice_crypt.encrypt(encrypted_private_key)
        logging.info(f"Lattice Encrypted Key: {encrypted_lattice}")

        # Decrypt the lattice-encrypted key to verify
        decrypted_lattice = lattice_crypt.decrypt(encrypted_lattice)
        if decrypted_lattice != encrypted_private_key:
            logging.error("Lattice-based decryption failed.")
        else:
            logging.info("Lattice-based decryption succeeded.")
            print("Wallet created and keys encrypted successfully.")

    except Exception as e:
        logging.error(f"Failed to create wallet: {e}")
        print(f"Failed to create wallet: {e}")

if __name__ == "__main__":
    main()

import base64
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
        log_command_output(result)
    except subprocess.CalledProcessError as e:
        logging.error(f"Command '{' '.join(command)}' failed with return code {e.returncode}")
        logging.error(f"Command output:\n{e.stdout}")
        logging.error(f"Command error output:\n{e.stderr}")
        raise e
    except Exception as e:
        logging.exception(f"Unexpected error while running command: {' '.join(command)}")
        raise e

def log_command_output(result):
    """Log the output of a subprocess command."""
    logging.info(f"Command output:\n{result.stdout}")
    if result.stderr:
        logging.warning(f"Command stderr:\n{result.stderr}")

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
        run_command(['cmake', f'-DCMAKE_PREFIX_PATH={os.popen("brew --prefix pybind11").read().strip()}', '-S', '.', '-B', 'build'], cwd='.')

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

    acct, encrypted_private_key = create_wallet()
    LAST_WALLET_CREATION_TIME = current_time
    return acct

def encrypt_private_key(acct):
    """Encrypt the private key using Fernet and lattice-based encryption."""
    encryption_key = Encrypt.load_key()
    
    # Convert the HexBytes private key to a byte sequence
    private_key_bytes = bytes(acct._private_key)
    
    # Encrypt the private key using Fernet
    encrypted_private_key = Encrypt.encrypt_message(encryption_key, private_key_bytes)
    
    # Encode to Base64 for safe storage and transmission
    encrypted_private_key_b64 = base64.b64encode(encrypted_private_key).decode('utf-8')
    variables.encrypted_private_key = encrypted_private_key_b64
    logging.info(f"Encrypted private key (Base64): {encrypted_private_key_b64}")

    # Utilize lattice-based encryption for further security
    lattice_crypt = RingLWECrypto(512, 4096)
    
    # Encrypt the already base64 encoded private key (do not encode to utf-8 again)
    encrypted_lattice = lattice_crypt.encrypt(encrypted_private_key)
    encrypted_lattice_b64 = base64.b64encode(encrypted_lattice).decode('utf-8')
    logging.info(f"Lattice Encrypted Key (Base64): {encrypted_lattice_b64}")

    return encrypted_private_key_b64, encrypted_lattice_b64

def verify_lattice_encryption(encrypted_private_key_b64, encrypted_lattice_b64):
    """Verify that the lattice-based encryption can be decrypted correctly."""
    lattice_crypt = RingLWECrypto(512, 4096)

    # Decode Base64 lattice-encrypted key
    encrypted_lattice = base64.b64decode(encrypted_lattice_b64)
    decrypted_lattice = lattice_crypt.decrypt(encrypted_lattice)

    # Convert decrypted bytes back to Base64 encoded string for comparison
    decrypted_lattice_b64 = base64.b64encode(decrypted_lattice).decode('utf-8')

    if decrypted_lattice_b64 != encrypted_private_key_b64:
        logging.error("Lattice-based decryption failed.")
        return False
    else:
        logging.info("Lattice-based decryption succeeded.")
        return True

def main():
    """Main function to build the project and create a wallet."""
    setup_logging()
    logging.info("Starting main function...")

    try:
        build_project()
    except Exception as e:
        logging.error(f"Build process failed and main process aborted: {e}")
        return

    try:
        acct = create_wallet_with_rate_limit()
        logging.info(f"Wallet address: {acct.address}")
        print(f"Wallet address: {acct.address}")

        encrypted_private_key, encrypted_lattice = encrypt_private_key(acct)

        if verify_lattice_encryption(encrypted_private_key, encrypted_lattice):
            print("Wallet created and keys encrypted successfully.")
        else:
            print("Failed to verify encryption.")
    except Exception as e:
        logging.error(f"Failed to create wallet: {e}")
        print(f"Failed to create wallet: {e}")

if __name__ == "__main__":
    main()

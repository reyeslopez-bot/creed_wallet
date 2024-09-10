from encryption import Encrypt

# Infura endpoint for connecting to Ethereum mainnet
INFURA_ENDPOINT = "https://mainnet.infura.io/v3/c41054ecfd8f45eb8b9952fad208f67a"

# Load or generate the encryption key
encryption_key = Encrypt.load_key()

# Placeholder for account details (will be set by wallet_create.py)
acct = None

# Placeholder for the encrypted private key (will be set by wallet_create.py)
encrypted_private_key = None

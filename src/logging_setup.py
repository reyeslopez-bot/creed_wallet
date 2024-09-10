import logging

def setup_logging():
    logging.basicConfig(
        filename='creed_wallet.log',  # Centralized log file
        level=logging.INFO,            # Set to INFO or DEBUG as needed
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    logging.info("Logging initialized.")

# arguments.py
import argparse

def parse_arguments():
    parser = argparse.ArgumentParser(description="Build and Wallet Creation Script")
    parser.add_argument('-d', '--build-dir', default='build', help='Specify the build directory.')
    parser.add_argument('-i', '--interval', type=int, default=300, help='Specify wallet creation interval in seconds.')
    args = parser.parse_args()

    if args.interval < 0:
        raise ValueError("Interval must be a non-negative integer.")
    
    return args

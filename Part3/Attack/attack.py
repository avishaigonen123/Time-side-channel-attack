from hashlib import sha256
from ecdsa import SigningKey, SECP256k1  # Assuming ECDSA over SECP256k1 curve
from fpylll import IntegerMatrix, LLL, BKZ  # fpylll for lattice reduction
from time import time
from elliptic_curve import EllipticCurve, Point
import os
import re
from datetime import datetime

# Parameters
N = 1000  # Number of signatures to collect
d = 10   # Number of fastest signatures to consider
li = 3   # Number of most significant bits assumed to be zero in the nonce

# Elliptic curve parameters
# TODO: set real values to this parametersp = 5003
p = 5003
a = 2
b = 3

curve = EllipticCurve(a, b, p)

# get mose recent data file
def get_most_recent_file(directory):
    # Regular expression to match the date and time in the filename
    pattern = re.compile(r'.+_(\d{4}-\d{2}-\d{2}_\d{2}-\d{2}-\d{2})\.txt')

    most_recent_file = None
    most_recent_time = None

    for filename in os.listdir(directory):
        match = pattern.match(filename)
        if match:
            timestamp_str = match.group(1)
            timestamp = datetime.strptime(timestamp_str, '%Y-%m-%d_%H-%M-%S')

            if most_recent_time is None or timestamp > most_recent_time:
                most_recent_time = timestamp
                most_recent_file = filename

    return most_recent_file


# Step 1: Collect N signatures and measure their signing durations
def collect_signatures(N):
    signatures = []
    for _ in range(N):
        sk = SigningKey.generate(curve=curve)
        # open file and read content
        path = 'data\\' + get_most_recent_file('data')
        with open(path, 'r') as file:
            for line in file:
                r, s, time = line.split(" ")
        signature = (int(r),int(s))
        duration = int(time)
        signatures.append((signature, duration))
    return signatures

signatures = collect_signatures(N)

# Step 2: Sort signatures by duration and select the fastest d signatures
sorted_signatures = sorted(signatures, key=lambda x: x[1])
fastest_signatures = sorted_signatures[:d]

# Step 3: Construct the lattice for lattice-based attack
def construct_lattice(signatures):
    B = IntegerMatrix(d, d + 1)
    for i in range(d):
        signature, duration = signatures[i]
        r, s = signature.r, signature.s
        # Construct the equation from the signature
        # Example: Constructing the matrix B
        B[i, 0] = s
        B[i, 1] = -r
        B[i, 2] = curve.order()
        # Add more rows as needed based on the attack description
    return B

B = construct_lattice(fastest_signatures)

# Step 4: Perform LLL reduction on the lattice matrix B
LLL.reduction(B)

# Step 5: Check for the private key recovery
def check_private_key(B, public_key):
    for i in range(B.nrows):
        candidate_priv_key = B[i, -1]
        # Generate corresponding public key
        candidate_pub_key = candidate_priv_key * curve.generator
        if candidate_pub_key == public_key:
            return candidate_priv_key
    return None

# Assuming you have the public key available
public_key = SigningKey.generate(curve=curve).verifying_key

recovered_private_key = check_private_key(B, public_key)

if recovered_private_key:
    print(f"Private key recovered: {recovered_private_key}")
else:
    print("Private key recovery unsuccessful.")


from hashlib import sha256
from fpylll import IntegerMatrix, LLL
from time import time
from elliptic_curve import EllipticCurve, Point  # Assuming you have this module implemented
import os
import re
from datetime import datetime

# Parameters
N = 1000  # Number of signatures to collect
d = 10   # Number of fastest signatures to consider

# Elliptic curve parameters (example values, replace with actual parameters)
p = 5003
a = 2
b = 3

curve = EllipticCurve(a, b, p)

# Get most recent data file
def get_most_recent_file(directory):
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

# Collect N signatures and measure their signing durations
def collect_signatures(N):
    signatures = []
    public_key = None

    for _ in range(N):
        path = 'data/' + get_most_recent_file('data')
        with open(path, 'r') as file:
            for line in file:
                if line.startswith("public key: "):
                    line = line.removeprefix("public key: ")
                    x, y = line.split()
                    public_key = Point(int(x), int(y))
                else:
                    r, s, elapsed = line.split()
                    signature = (int(r), int(s))
                    duration = int(elapsed)
                    signatures.append((signature, duration))
                    
    return signatures, public_key

signatures, public_key = collect_signatures(N)

# Sort signatures by duration and select the fastest d signatures
sorted_signatures = sorted(signatures, key=lambda x: x[1])
fastest_signatures = sorted_signatures[:d]

def geom_bound(index, num_of_signatures):
        """Estimate the number of leading zero bits at signature with `index`."""
        i = 1
        while (num_of_signatures) / (2 ** i) >= index + 1:
            i += 1
        i -= 1
        if i <= 1:
            return 0
        return i

def modular_inverse(x, n):
    """Calculate the modular inverse of x modulo n."""
    return pow(x, -1, n)

def build_svp_lattice(signatures, curve):
    """Build the basis matrix for the SVP lattice using `signatures`."""
    dim = len(signatures)
    B = IntegerMatrix(dim + 2, dim + 2)  # the SVP matrix
    n = curve.calc_order()  # curve order

    for i in range(dim):
        li = 3  # Assume number of MSB bits with zero
        si = signatures[i][0]  # si
        ri = signatures[i][1]  # ri
        ti = (modular_inverse(si, n) * ri) % n  # ti
        ui = (modular_inverse(si, n) * signatures[i].h) % n  # ui

        B[i, i] = (2 ** li) * n  # (2^li)*n
        B[dim, i] = (2 ** li) * ti  # (2^li) * ti
        B[dim + 1, i] = (2 ** li) * ui + n  # (2^li) * ui + n

    B[dim, dim] = 1
    B[dim + 1, dim + 1] = n
    return B

# Assuming `fastest_signatures` is a list of `Signature` namedtuples and `curve` is defined
B = build_svp_lattice(fastest_signatures, curve)

# Perform LLL reduction on the lattice matrix B
LLL.reduction(B)

# Check the lattice matrix B (optional)
for row in B:
    print(row)

# Check for the private key recovery
def check_private_key(B, public_key):
    for i in range(B.nrows):
        candidate_priv_key = B[i, -1]
        # Generate corresponding public key
        candidate_pub_key = candidate_priv_key * curve.generator
        if candidate_pub_key == public_key:
            return candidate_priv_key
    return None

recovered_private_key = check_private_key(B, public_key)

if recovered_private_key:
    print(f"Private key recovered: {recovered_private_key}")
else:
    print("Private key recovery unsuccessful.")

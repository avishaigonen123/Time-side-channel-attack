from fpylll import IntegerMatrix, LLL 
from time import time
from elliptic_curve import EllipticCurve, Point  
import os
import re
from datetime import datetime

# TODO: change this number if nessacery
d = 200   # Number of fastest signatures to consider

# Elliptic curve parameters (example values, replace with actual parameters)
p = 991
a = 16
b = 20
G = Point(589, 52)


curve = EllipticCurve(a, b, p, G)

hash_to_change = 1234

# Collect N signatures and measure their signing durations
# TODO: fix this function to, to check whether it is working
# TODO: delete this function and write again, this is not a good name
def collect_signatures():
    signatures = []
    public_key = None

    path = "/home/agonen/Time-side-channel-attack/Part3/Attack/data/new_test_2_1.txt" 
    
    with open(path, 'r') as file:
        for line in file:
            if line.startswith("public key: "):
                line = line.removeprefix("public key: ")
                x, y = line.split(" ")
                public_key = Point(int(x), int(y))
            elif line.startswith("r "):
                public_key = Point(596, 141) # real public key
                continue 
            else:
                r, s, elapsed = line.split(" ")
                signature = (int(r), int(s))
                duration = int(elapsed)
                signatures.append((signature, duration))
                    
    return signatures, public_key

# TODO: consider whether to use it or not, becuase maybe i can only say li = 3
def geom_bound(index, num_of_signatures):
        """Estimate the number of leading zero bits at signature with `index`."""
        i = 1
        while (num_of_signatures) / (2 ** i) >= index + 1:
            i += 1
        i -= 1
        if i <= 1:
            return 0
        return i


# TODO: understand what is going on, check if it working
def build_svp_lattice(signatures, curve):
    """Build the basis matrix for the SVP lattice using `signatures`."""
    dim = len(signatures)
    B = IntegerMatrix(dim + 2, dim + 2)  # the SVP matrix
    n = curve.calc_order()  # curve order

    for i in range(dim):
        li = geom_bound(i, dim) + 1  # Assume number of MSB bits with zero
        ri = signatures[i][0][0]  # ri
        si = signatures[i][0][1]  # si
        ti = (curve.modularInverse(si, n) * ri) % n  # ti
        ui = -((curve.modularInverse(si, n) * hash_to_change) % n)  # ui

        B[i, i] = (2 ** li) * n  # (2^li)*n
        B[dim, i] = (2 ** li) * ti  # (2^li) * ti
        B[dim + 1, i] = (2 ** li) * ui + n  # (2^li) * ui + n 

    B[dim, dim] = 1
    B[dim + 1, dim + 1] = n
    return B

# Check for the private key recovery
def check_private_key(B, public_key):
    n = curve.calc_order()
    for i in range(B.nrows):
        candidate_priv_key = B[i, -2] % n
        # Generate corresponding public key
        candidate_pub_key = curve.scalarMultiplication(curve.G, candidate_priv_key) 
        if candidate_pub_key == public_key:
            return candidate_priv_key
    return None

# the attack itself.
# TODO: give more convient name, divide to files
def attack():
    RawSignatures, public_key = collect_signatures()

    signatures = []    
    found = {}
    for sig in RawSignatures:
        if not sig[0] in found:
            found[sig[0]] = sig
            signatures += [sig]

    # Sort signatures by duration and select the fastest d signatures
    sorted_signatures = sorted(signatures, key=lambda x: x[1])
    fastest_signatures = sorted_signatures[:d]

    # Assuming `fastest_signatures` is a list of `Signature` namedtuples and `curve` is defined
    B = build_svp_lattice(fastest_signatures, curve)

    # Perform LLL reduction on the lattice matrix B
    LLL.reduction(B)

    recovered_private_key = check_private_key(B, public_key)

    if recovered_private_key:
        print(f"Private key recovered: {recovered_private_key}")
    else:
        print("Private key recovery unsuccessful.")


attack()
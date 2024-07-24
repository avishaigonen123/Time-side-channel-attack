from fpylll import IntegerMatrix, LLL 
from time import time
from elliptic_curve_imp import EllipticCurve, Point  
import os
import re
from datetime import datetime

d = 10   # Number of fastest signatures to consider

# Elliptic curve parameters (example values, replace with actual parameters)
p = 991
a = 16
b = 20
G = Point(589, 52)

curve = EllipticCurve(a, b, p, G)

hash_of_test_message = 1234

# Collect N signatures and measure their signing durations
def collect_signatures(path):
    signatures = []
    public_key = None

    # path = "/home/agonen/Time-side-channel-attack/Part3/Attack/data/16_20_991_990.txt" 
    
    with open(path, 'r') as file:
        line = file.readline()
        if line.startswith("public_key: "):
            x, y = line.split(" ")[1:]
            print("public key: " + x + " " + y)
            public_key = Point(int(x), int(y))
        else:
            public_key = Point(596, 141) # real public key
            # print("public key: defalte")
        for line in file.readlines()[1:]:
            r, s, elapsed = line.split(" ")
            signature = (int(r), int(s))
            duration = int(elapsed)
            signatures.append((signature,duration))
                    
    return signatures, public_key

# consider whether to use it or not, becuase maybe i can only say li = 3
def geom_bound(index, num_of_signatures):
        """Estimate the number of leading zero bits at signature with `index`."""
        i = 1
        while (num_of_signatures) / (2 ** i) >= index + 1:
            i += 1
        i -= 1
        if i <= 1:
            return 0
        return i


# build svp lattice based on our d equtions, do reduction to another solvable problem
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
        ui = -((curve.modularInverse(si, n) * hash_of_test_message) % n)  # ui

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
def attack(path):
    RawSignatures, public_key = collect_signatures(path)

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
    # magic stuff here 
    LLL.reduction(B)

    recovered_private_key = check_private_key(B, public_key)

    if recovered_private_key:
        return f"Private key recovered: {recovered_private_key}"
    else:
        return False
    #     print(f"Private key recovered: {recovered_private_key}")
    # else:
    #     print("Private key recovery unsuccessful.")

path = input("give me path to your data file: ")

# we will try to do the attack for a lot of d's
while True:
    res = attack(path)
    if res:
        print(f"d = {d}")
        print(res)
        break
    if not d %100:
        print(f"d = {d}")
    if d > 500:
        print("Attack failed")
        break
    d+=5
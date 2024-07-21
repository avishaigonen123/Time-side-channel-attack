from elliptic_curve_imp import EllipticCurve, Point 
from sympy import isprime

def next_prime(n):
    if n <= 1:
        return 2
    prime = n
    found = False
    while not found:
        prime += 1
        if isprime(prime):
            found = True
    return prime


def findSEC():
    a = 16
    b = 20
    p = 4294967291
    found = False
    tryis = ('a', 'b')
    nextTry = tryis[0]
    while not found:
        if nextTry == 'a':
            a += 1
        elif nextTry == 'b':
            b += 1
        nextTry = (tryis.index(nextTry) + 1) % len(tryis)
        inp = ""
        while True:
            inp = input("Enter the G!\n")
            if inp == "n":
                break
            elif inp == "E":
                exit(0)
            Garr = inp.split(" ")
            if len(Garr) != 2:
                print("format: x y")
                continue
            G = Point(int(Garr[0]), int(Garr[1]))
            if (G.y**2 - (G.x**3 + a*G.x + b)) % p != 0:
                print(f"G is not on curve.\nG : {G}")
                continue
            else:
                print("G is on curve!")
            curve = EllipticCurve(a,b,p, G)
            if isprime(curve.calc_order()):
                print("found it!")
                print(f"a={a} b={b} p={p} g={G}")
            else:
                print ("not prime..\nx y - try a diffrent G\nn - next curve\nE - exit the program")
            
findSEC()
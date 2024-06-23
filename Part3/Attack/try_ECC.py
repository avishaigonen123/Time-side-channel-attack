import random
import math

class Point:
    def __init__(self, x=None, y=None):
        if x is None and y is None:
            self.x = random.randint(1, p)
            x = self.x
            self.y = int(math.sqrt(pow(x, 3) + a*x + b)) % p
            if self.y == 0:
                self.y = random.randint(1, p)
            # self.y = random.randint(1, p)
        else:
            self.x = x
            self.y = y

    def __str__(self):
        return f"({self.x}, {self.y})"
    
    def __add__(self, other):
        # Check if points are equal
        if self.x == other.x and self.y == other.y:
            return self.double()  # Point doubling
        # Check if either point is the identity (0, 0)
        elif self.x == 0 and self.y == 0:
            return other
        elif other.x == 0 and other.y == 0:
            return self
        # Check if points are inverses
        elif self.x == other.x and self.y == -other.y:
            return Point(0, 0)  # Identity point (0, 0)
        else:
            # Compute the sum using the elliptic curve addition formula
            if self.x == other.x:
                s = (3 * self.x ** 2 + self.curve_a) * pow(2 * self.y, -1, self.p) % self.p
            else:
                s = (other.y - self.y) * pow(other.x - self.x, -1, self.p) % self.p
            x3 = (s ** 2 - self.x - other.x) % self.p
            y3 = (s * (self.x - x3) - self.y) % self.p
            return Point(x3, y3)
    
    def double(self):
        # Point doubling formula
        lam = (3 * self.x ** 2 + self.curve_a) * pow(2 * self.y, -1, self.p) % self.p
        x3 = (lam ** 2 - 2 * self.x) % self.p
        y3 = (lam * (self.x - x3) - self.y) % self.p
        return Point(x3, y3)

# Elliptic curve parameters
p = 10007
a = 2
b = 2
G = (5, 1)  # Base point (generator)


# Generate a random point on the elliptic curve
Z = set()
Y = set()
points = [Point() for _ in range(12000)]

for P in points:
    xP, yP = P.x, P.y
    s = (3 * pow(xP, 2) + a) * pow(2*yP, -1)
    xR0 = pow(s, 2) - 2*xP
    yR0 = s * (xP - xR0) - yP 
    # R0 = 2 * P
    if yR0 < p and xR0 < p: # not modulo
        xQ, yQ = xR0, yR0
        xP, yP = P.x, P.y
        s = (yP - yQ) * pow(xP - xQ, -1)
        xR0 = pow(s, 2) - (xP + xQ)
        yR0 = s * (xP - xR0) - yP
        if yR0 < p and xR0 < p:
            Y.add(Point(xR0, yR0))
        else:
            Z.add(Point(xR0, yR0))

print(len(Z))
print(len(Y))
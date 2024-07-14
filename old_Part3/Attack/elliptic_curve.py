
# class to present a point
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

    def __str__(self):
        if self.x == -1 and self.y == -1:
            return "Point at infinity"
        return f"({self.x}, {self.y})"
    def __hash__(self):
        return hash((self.x, self.y))

# class to present Elliptic Curve
class EllipticCurve:
    def __init__(self, a, b, p):
        self.p = p
        self.b = b
        self.a = a
        self.InfPoint = Point(-1, -1)

    def addPoint(self, point1, point2):
        if point1.x == point2.x and point1.y == point2.y:
            return self.doublingPoint(point1)
        if point1.x == point2.x: # vertical
            return self.InfPoint
        if point1 == self.InfPoint: 
            return point2
        if point2 == self.InfPoint:
            return point1

        numerator = point1.y - point2.y
        denominator = point1.x - point2.x
        if denominator < 0:
            numerator *= -1
            denominator *= -1
        s = self.module(numerator, self.p) * self.modularInverse(denominator, self.p) % self.p

        R_x = s * s - (point1.x + point2.x)
        R_x = self.module(R_x, self.p)
        R_y = s * (point1.x - R_x) - point1.y
        R_y = self.module(R_y, self.p)
        return Point(R_x, R_y)

    def doublingPoint(self, point):
        if point == self.InfPoint:
            return self.InfPoint
        if point.y == 0:
            return self.InfPoint

        numerator = 3 * point.x * point.x + self.a
        denominator = 2 * point.y
        s = self.module(numerator, self.p) * self.modularInverse(denominator, self.p) % self.p

        R_x = s * s - 2 * point.x
        R_x = self.module(R_x, self.p)
        R_y = s * (point.x - R_x) - point.y
        R_y = self.module(R_y, self.p)
        return Point(R_x, R_y)

    def scalarMultiplication(self, P, k):
        R0 = P
        l = self.key_length(k)
        for j in range(l - 2, -1, -1):
            R0 = self.doublingPoint(R0)
            if (k >> j) & 1:
                R0 = self.addPoint(R0, P)
        return R0

    def EllipticCurveCalcPoint(self, P, PrivKey):
        return self.scalarMultiplication(P, PrivKey)

    def gcdExtended(self, a, b):
        if a == 0:
            return b, 0, 1
        gcd, x1, y1 = self.gcdExtended(b % a, a)
        x = y1 - (b // a) * x1
        y = x1
        return gcd, x, y

    def modularInverse(self, a, p):
        gcd, x, y = self.gcdExtended(a, p)
        if gcd != 1:
            return -1  # Modular inverse doesn't exist
        else:
            return (x % p + p) % p

    def key_length(self, k):
        for i in range(31, -1, -1):
            if (k >> i) & 1: # reach the first non-zero bit
                return i + 1
        return 0

    def module(self, a, b):
        r = a % b
        if r < 0: 
            r += b
        return r

# Example usage
if __name__ == "__main__":
    curve = EllipticCurve(a=2, b=3, p=97)
    P = Point(3, 6)
    R0 = P
    R0 = curve.addPoint(R0, P)
    R0 = curve.addPoint(R0, P)
    R0 = curve.addPoint(R0, P)
    R0 = curve.addPoint(R0, P)
    print(R0)

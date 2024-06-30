import random
import math

# TODO: set real values to this DELTA 
DELTA = 0.1 # this is the difference between the two groups

# TODO: set key size
KEY_SIZE = 8

# Elliptic curve parameters
# TODO: set real values to this parameters
p = 97
a = 2
b = 3

class Point:
    # TODO: implement the functions that they will work
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return f"({self.x}, {self.y})"
    
    '''def __add__(self, other):
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
        return Point(x3, y3)'''


'''
    Point EllipticCurve::addPoint(const Point& point1, const Point& point2)
{
    if (point1.x == point2.x) // next point will be infinity point
        return InfPoint;    
    if (point1 == InfPoint) // return the second point
        return point2;
    if (point2 == InfPoint) // return the second point
        return point1;
    if (point1.x == point2.x && point1.y == point2.y) // the same point
        return doublingPoint(point1);  
    Point R;
    int32_t numerator = point1.y - point2.y;
    int32_t denominator = point1.x - point2.x;
    int32_t s = modulo(numerator, p) * modularInverse(denominator, p);

    R.x = s * s - (point1.x + point2.x);
    R.x = modulo(R.x, p);
    R.y = s * (point1.x - R.x) - point1.y;
    R.y = modulo(R.y, p);
    return R;
}
Point EllipticCurve::doublingPoint(const Point& point){
    if (point == InfPoint) // InfPoint+InfPoint = InfPoint
        return InfPoint;
    if (point.y == 0) // InfPoint because this is a vertical line
        return InfPoint;
    Point R;
    int32_t numerator = 3 * point.x * point.x + a;
    int32_t denominator = 2 * point.y;
    int32_t s = modulo(numerator, p) * modularInverse(denominator, p);

    R.x = s * s - 2 * point.x;
    R.x = modulo(R.x, p);
    R.y = s * (point.x - R.x) - point.y;
    R.y = modulo(R.y, p);
    return R;
}


'''

def not_modulo_stage_doubling(P):
    # TODO: implement not_modulo_stage_doubling
    xP, yP = P.x, P.y
    s = (3 * pow(xP, 2) + a) * pow(2*yP, -1)
    xR0 = pow(s, 2) - 2*xP
    yR0 = s * (xP - xR0) - yP 
    # R0 = 2 * P
    return yR0 < p and xR0 < p # not modulo

def not_modulo_stage_adding(P):
    # TODO: implement not_modulo_stage_adding
    xQ, yQ = xR0, yR0
    xP, yP = P.x, P.y
    s = (yP - yQ) * pow(xP - xQ, -1)
    xR0 = pow(s, 2) - (xP + xQ)
    yR0 = s * (xP - xR0) - yP
    return yR0 < p and xR0 < p    

def promote_points(points_to_times, key_bit):
    # TODO: implement promote_points
    # based on the key_bit i got, i should promote the points
    # at the end, return the new dict back to the caller
    pass

def cluster_points(points_to_times):
    # TODO: implement cluster_points
    # based on the points_to_times dict, i should cluster the points
    # at the end, return the new dict back to the caller
    modulo_after_1_stage = {} # dictionary of points that did modulo after 1 stage
    not_modulo_at_all = {} # dictionary of points that were not modulo at all

    for P in points_to_times.keys():
        if not_modulo_stage_doubling(P):
            if not_modulo_stage_adding(P):
                not_modulo_at_all[P] = points_to_times[P]
            else:
                modulo_after_1_stage[P] = points_to_times[P]
    
    return modulo_after_1_stage, not_modulo_at_all

def calculate_averages(cluster):
    average_cluster = 0
    for time in cluster.values():
        average_cluster += time
    average_cluster /= len(cluster)

    return average_cluster




def main():
    # open file and read content
    path = r'C:\Users\avish\OneDrive\Documents\PlatformIO\Projects\Time side channel attack\Part3\Attack\data.txt'

    points_to_times = {} # dictionary of points to times

    with open(path, 'r') as file:
        data = file.read()
    
    for line in data.split('\n'):
        x, y, time = line.split(' ')
        points_to_times[Point(int(x),int(y))] = float(time)
    
    modulo_after_1_stage = {} # dictionary of points that did modulo after 1 stage
    not_modulo_at_all = {} # dictionary of points that were not modulo at all

    key_bits = []


    while not len(key_bits) == KEY_SIZE:
       # first, cluster the points
        modulo_after_1_stage, not_modulo_at_all = cluster_points(points_to_times) 
        '''now, i have my dictionaries, all left is to analyze the difference
        assume our current bit is 1, so the difference between the average of each group should be significant.
        If not, it means that our current bit is 0.
        '''

        average_not_modulo_at_all = calculate_averages(not_modulo_at_all)
        average_modulo_after_1_stage = calculate_averages(modulo_after_1_stage)

        if average_modulo_after_1_stage - average_not_modulo_at_all > DELTA:
            key_bits += [1]
        else:
            key_bits += [0]

        promote_points(points_to_times, key_bits[:-1]) # promote points, according to the last bit found
    
    print(f"found key! {hex(key_bits)}")

if __name__ == "__main__":
    main()
import random
import math

# TODO: set real values to this DELTA 
DELTA = 0.01 # this is the difference between the two groups

# TODO: set key size
KEY_SIZE = 20

# Elliptic curve parameters
# TODO: set real values to this parameters
p = 10007
a = 2
b = 2

class Point:
    # TODO: implement the functions that they will work
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

def calculate_averages(not_modulo_at_all, modulo_after_1_stage):
    average_not_modulo_at_all = 0
    for time in not_modulo_at_all.values():
        average_not_modulo_at_all += time
    average_not_modulo_at_all /= len(not_modulo_at_all)

    average_modulo_after_1_stage = 0
    for time in modulo_after_1_stage.values():
        average_modulo_after_1_stage += time
    average_modulo_after_1_stage /= len(modulo_after_1_stage)

    return average_not_modulo_at_all, average_modulo_after_1_stage




def main():
    # open file and read content
    path = r'C:\Users\avish\OneDrive\Documents\PlatformIO\Projects\Time side channel attack\Part3\Attack\data.txt'

    points_to_times = {} # dictionary of points to times

    with open(path, 'r') as file:
        data = file.read()
    
    for line in data.split('\n'):
        x, y, time = line.split(' ')
        points_to_times[Point(x,y)] = time
    
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


        average_not_modulo_at_all, average_modulo_after_1_stage = calculate_averages(not_modulo_at_all, modulo_after_1_stage)

        if average_modulo_after_1_stage - average_not_modulo_at_all > DELTA:
            key_bits += [1]
        else:
            key_bits += [0]

        promote_points(points_to_times, key_bits[::-1]) # promote points, according to the last bit found


if __name__ == "__main__":
    main()
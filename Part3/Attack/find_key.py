import random
import math
from elliptic_curve import EllipticCurve, Point
import os
import re
from datetime import datetime
 # this is the difference between the two groups
DELTA = 0.28
KEY_SIZE = 8 # key size in bits
CONST_STD_DIV = 150

# Elliptic curve parameters
a = 2
b = 3
p = 193939

curve = EllipticCurve(a, b, p)
ifFirstBit = True

# function that returns the most recent data file
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

# function that returns true if in the special modulo, the value is negative.
def is_negative_in_special_modulo(point, ogP):
    R0 = curve.doublingPoint(point) 
    return R0.y < ogP.y # "numerator = point.y - R0.y", it enters the if only when this is negative'

# function that promote all points to next iteration, using the given key_bit
def promote_points(points_to_times, key_bit):
    newPointsToTimes = {}
    if key_bit == 1:
        for P in points_to_times.keys():
            R0 = curve.doublingPoint(P)
            R0 = curve.addPoint(R0, points_to_times[P][1])
            newPointsToTimes[R0] = points_to_times[P]
    else:
        for P in points_to_times.keys():
            R0 = curve.doublingPoint(P)                    # double 
            newPointsToTimes[R0] = points_to_times[P]
    
    return newPointsToTimes

# function that cluster the points to 2 groups, based on give predicate, and return this two clusters. 
def cluster_points(points_to_times, predicate):
    cluster1 = {} # dictionary of points that are satisfied by the given predicate
    cluster2 = {} # dictionary of points that aren't satisfied by the given predicate

    for P in points_to_times.keys():
        val = points_to_times[P]
        if predicate(P, val[1]):
            cluster1[P] = val
        else:
            cluster2[P] = val
    
    return cluster1, cluster2

# function that calculates average time for given cluster
def calculate_averages(cluster):
    # sum up total time for given cluster
    cluster_total_time = 0
    for timePoint in cluster.values():
        cluster_total_time += timePoint[0]

    # calculate average time for given cluster
    average_cluster_time = cluster_total_time / len(cluster)
    return average_cluster_time

def removedeviations(cluster):
    avr = calculate_averages(cluster)
    newClaster = {}
    for elem in cluster.keys():
        if cluster[elem][0] <= avr + CONST_STD_DIV:
            newClaster[elem] = cluster[elem]
    return newClaster

# function that gets points to times dictionary, and returns the key
def crack_key(points_to_times):

    negative_in_special_modulo = {} # dictionary of points that did modulo after 1 stage
    not_negative_in_special_modulo = {} # dictionary of points that were not modulo at all
    key_bits = [1] # here we'll store the key bits
    #points_to_times = promote_points(points_to_times, 0)
    index = 1
    realKey = [1,0,1,1,1,0,1,0]
    # find the key bits, bit by bit
    while not len(key_bits) == KEY_SIZE:
       # first, cluster the points to 2 clusters
        negative_in_special_modulo, not_negative_in_special_modulo = cluster_points(points_to_times, is_negative_in_special_modulo) 

        # calculate average time for each cluster
        average_not_negative_in_special_modulo = calculate_averages(not_negative_in_special_modulo)
        average_negative_in_special_modulo = calculate_averages(negative_in_special_modulo)

        # if the difference between the averages is significant, the operation was done and the bit is 1
        d = abs(average_negative_in_special_modulo - average_not_negative_in_special_modulo)
        print(d)
        if d > DELTA:
            key_bits += [1]
        else: # otherwise, the bit is 0
            key_bits += [0]
        
        # key_bits += [realKey[index]]
        # index += 1

        # promote points, according to the last bit found
        points_to_times = promote_points(points_to_times, key_bits[-1])

    # return the key 
    return key_bits

# main function
def main():
    # open file and read content
    path = 'data\\' + get_most_recent_file('data')
    print("Reading data from: " + path)
    points_to_times = {} # dictionary of points to times

    with open(path, 'r') as file:
        data = file.read()
    

    for line in data.split('\n')[1:-1]:
        x, y, time = line.split(' ')
        points_to_times[Point(int(x),int(y))] = (float(time), Point(int(x),int(y)))
    
    points_to_times = removedeviations(points_to_times)
    # crack the key
    key_bits = crack_key(points_to_times)

    # print the found key as hexadecimal number
    key = 0
    for i in range(KEY_SIZE):
        key = (key << 1) + key_bits[i]
    print(f"found key {hex(key)}")


if __name__ == "__main__":
    main()
    '''key = 0x12
    P = Point(12,94)
    res1 = curve.EllipticCurveCalcPoint(P, key)
    cluster = {Point(P.x,P.y):(1,P)}
    #res2 = promote_points(cluster, 1)
    #print(list(res2.keys())[0])
    #res2 = promote_points(cluster, 0, P)
    #res2 = promote_points(res2, 1, P)
    #res2 = promote_points(res2, 1, P)

    res2 = promote_points(cluster, 0)
    res2 = promote_points(res2, 0)
    res2 = promote_points(res2, 1)
    res2 = promote_points(res2, 0)

    print(res1)
    print(list(res2.keys())[0])'''

    
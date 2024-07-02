import random
import math
from curve import EllipticCurve, Point
import os
import re
from datetime import datetime

# function that returns the most recent data file
def get_most_recent_file(directory):
    # Regular expression to match the date and time in the filename
    pattern = re.compile(r'data_(\d{4}-\d{2}-\d{2}_\d{2}-\d{2}-\d{2})\.txt')

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


# TODO: set real values to this DELTA 
DELTA = 1.2 # this is the difference between the two groups

# TODO: set key size
KEY_SIZE = 8

# Elliptic curve parameters
# TODO: set real values to this parameters
p = 97
a = 2
b = 3

curve = EllipticCurve(a, b, p)

def not_modulo_stage_doubling(P):
    return True # not negative always

def not_modulo_stage_adding(P):
    # TODO: implement not_modulo_stage_adding
    R0 = curve.doublingPoint(P)
    return R0.y < P.y

def promote_points(points_to_times, key_bit):
    # TODO: implement promote_points
    # based on the key_bit i got, i should promote the points
    # at the end, return the new dict back to the caller
    newPointsToTimes = {}
    if key_bit == 1:
        for P in points_to_times.keys():
            newP = curve.addPoint(curve.doublingPoint(P), P)
            newPointsToTimes[newP] = points_to_times[P]
    else:
        for P in points_to_times.keys():
            newP = curve.doublingPoint(P)
            newPointsToTimes[newP] = points_to_times[P]
    
    return newPointsToTimes


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
    path = 'data\\' + get_most_recent_file('data')

    points_to_times = {} # dictionary of points to times

    with open(path, 'r') as file:
        data = file.read()
    
    for line in data.split('\n')[1:-1]:
        x, y, time = line.split(' ')
        points_to_times[Point(int(x),int(y))] = float(time)
    size = len(points_to_times)
    modulo_after_1_stage = {} # dictionary of points that did modulo after 1 stage
    not_modulo_at_all = {} # dictionary of points that were not modulo at all

    key_bits = []


    while not len(key_bits) == KEY_SIZE:
       # first, cluster the points
        modulo_after_1_stage, not_modulo_at_all = cluster_points(points_to_times) 
        
        # now, i have my dictionaries, all left is to analyze the difference 
        # assume our current bit is 1, so the difference between the average of each group should be significant.
        # If not, it means that our current bit is 0.

        average_not_modulo_at_all = calculate_averages(not_modulo_at_all)
        average_modulo_after_1_stage = calculate_averages(modulo_after_1_stage)

        if abs(average_modulo_after_1_stage - average_not_modulo_at_all) > DELTA:
            key_bits += [1]
        else:
            key_bits += [0]

        points_to_times = promote_points(points_to_times, key_bits[-1]) # promote points, according to the last bit found
    key = 0
    for i in range(KEY_SIZE):
        key = (key << 1) + key_bits[i]
    print(f"found key {hex(key)}")

if __name__ == "__main__":
    main()
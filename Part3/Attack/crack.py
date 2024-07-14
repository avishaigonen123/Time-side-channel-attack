from elliptic_curve import EllipticCurve, Point
import os
import re
from datetime import datetime

# Elliptic curve parameters
# TODO: set real values to this parameters
p = 5003
a = 2
b = 3

curve = EllipticCurve(a, b, p)

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


def main():
    # open file and read content
    path = 'data\\' + get_most_recent_file('data')


    with open(path, 'r') as file:
        for line in file:
            message, r, s, time = line.split(", ")


if __name__ == "__main__":
    main()
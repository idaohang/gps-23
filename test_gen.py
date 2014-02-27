#!/usr/bin/python2
from math import *
from struct import *

import sys

TIME_ZERO = 1073741829
LAT_ZERO = 32.0
LON_ZERO = 42.0

# 100 km increase
DELTA_LAT = 0.902
DELTA_LON = 1.0585 

M_PI = 3.14159265
DEG_TO_RAD = (180.0 / M_PI)
EARTH_RADIUS = 6371032 
COORD_MULT = 10000000
M_IN_KM = 1000
S_IN_HR = 3600

# def calc_distance(lat_a, lon_a, lat_b, lon_b):
#     result = 0;

#     lat_a /= (COORD_MULT * DEG_TO_RAD)
#     lon_a /= (COORD_MULT * DEG_TO_RAD)
#     lat_b /= (COORD_MULT * DEG_TO_RAD)
#     lon_b /= (COORD_MULT * DEG_TO_RAD)

#     result = sin(lat_a) * sin(lat_b) + cos(lat_a) * cos(lat_b) * cos(lon_a - lon_b)

#     return acos(result) * EARTH_RADIUS / M_IN_KM

if len(sys.argv) != 3:
    print """Usage: {0} points_count test_name\n""".format(sys.argv[0])
    exit(1)

points_count = int(sys.argv[1])
time = TIME_ZERO
test_file = open(sys.argv[2], 'wb')
test_file.write(pack('<LQQQ', 1234, 2, 5678, points_count))

lat = LAT_ZERO
lon = LON_ZERO

for i in xrange(points_count):
    # lat = LAT_ZERO + i*DELTA_LAT/100
    lon = LON_ZERO + i*DELTA_LON 
    time += S_IN_HR 
    print round(lat, 2), '\t', round(lon, 2), '\t', time 
    test_file.write(pack('<LLBLL', lon*COORD_MULT, lat*COORD_MULT, 100, time, time))

test_file.write(pack('<QQ', 9101112, points_count))

for i in xrange(points_count / 2):
    lat = LAT_ZERO + i*DELTA_LAT * 0.9
    # lon = LON_ZERO + i*DELTA_LON 
    time += S_IN_HR 
    print round(lat, 2), '\t', round(lon, 2), '\t', time 
    test_file.write(pack('<LLBLL', lon*COORD_MULT, lat*COORD_MULT, 90, time, time))

for i in xrange(points_count - points_count / 2):
    lat = LAT_ZERO + i*DELTA_LAT * 1.1
    # lon = LON_ZERO + i*DELTA_LON 
    time += S_IN_HR 
    print round(lat, 2), '\t', round(lon, 2), '\t', time 
    test_file.write(pack('<LLBLL', lon*COORD_MULT, lat*COORD_MULT, 110, time, time))

test_file.close()
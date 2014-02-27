#ifndef GPS_H
#define GPS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "bst.h"

typedef struct {
	int sub_provider_id;
	unsigned long device_count; 
} subProviderBlockHeader;

typedef struct {
	long device_id;
	unsigned long point_count;
} deviceBlockHeader;

typedef struct {
	int lon, lat;
	unsigned char speed;
	int gps_time, sys_time;
} pointBlock;

typedef struct {
	unsigned int avg_speed;
	unsigned int real_avg_speed;
	unsigned int avg_speed_motion;
	unsigned int time_static;
	unsigned int dispersion;
	unsigned int real_dispersion;
} deviceData;

unsigned long calc_dispersion(node *root);

double calc_distance(double lat_a, double lon_a, double lat_b, double lon_b);

deviceData *handle_device(FILE *data, unsigned long point_count);


#endif
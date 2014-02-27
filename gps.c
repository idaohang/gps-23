#include "gps.h"

#define M_PI 3.14159265
#define DEG_TO_RAD (180.0 / M_PI)
#define EARTH_RADIUS 6371032 
#define COORD_MULT 10000000
#define M_IN_KM 1000
#define S_IN_HR 3600

int
main(int argc, char **argv) 
{
	if (argc != 2) {
		printf("Usage: %s data.bin\n", argv[0]);
		return 1;
	}

	FILE *data = fopen(argv[1], "rb");

	if (!data) {
		printf("Unable to open file %s\n", argv[1]);
		return 1;
	}

	/* Reading records directly can be dangerous due to unreliability 
	   and machine-dependency of -fpack-struct gcc option and
	   __attribute__((packed)) extension. In case of any issues 
	   reading record fields through single-char buffer should be implemented 
	   (see http://www.delorie.com/djgpp/v2faq/faq22_11.html for details)
	*/

	subProviderBlockHeader prov_header;
	deviceBlockHeader dev_header;
	
	deviceData *dev_data;

	while (!feof(data)) {
		if (!fread(&prov_header, sizeof(prov_header), 1, data)) {
			fclose(data);
			return 0;
		}
		printf("Reading subprovider #%d\n", prov_header.sub_provider_id);
		printf("%ld devices are mentioned\n", prov_header.device_count);

		for (int i = 0; i < prov_header.device_count; i++) {
			fread(&dev_header, sizeof(dev_header), 1, data);
			printf("Reading device #%ld\n", dev_header.device_id);
			printf("%ld points are mentioned\n", dev_header.point_count);

			dev_data = handle_device(data, dev_header.point_count);

			printf("\n");
			printf("Average speed: %d\nAverage speed in motion: %d\n", 
				dev_data->avg_speed, dev_data->avg_speed_motion);
			printf("Dispersion: %d\nReal average speed: %d\n", 
				dev_data->dispersion, dev_data->real_avg_speed);
			printf("Real speed dispersion: %d\nStatic time: %d\n", 
				dev_data->real_dispersion, dev_data->time_static);
			printf("\n");
			free(dev_data);
		}
	}


	fclose(data);
	return 0;
}

unsigned long 
calc_dispersion(node *root)
{
	dispersionHelper result = {0, 0, 0};

	walk(root, &result);

	if (result.sum_vals) {
		return result.expect_sq / result.sum_vals - 
			(result.expect / result.sum_vals) * 
						(result.expect / result.sum_vals);
	} else {
		// device didn't move at all
		return 0;
	}
}

double
calc_distance(double lat_a, double lon_a, double lat_b, double lon_b)
{
	double result = 0;

	lat_a = (double) lat_a / (COORD_MULT * DEG_TO_RAD);
	lon_a = (double) lon_a / (COORD_MULT * DEG_TO_RAD);
	lat_b = (double) lat_b / (COORD_MULT * DEG_TO_RAD);
	lon_b = (double) lon_b / (COORD_MULT * DEG_TO_RAD);

	result = sin(lat_a) * sin(lat_b) + cos(lat_a) * 
									cos(lat_b) * cos(lon_a - lon_b);

	return acos(result) * EARTH_RADIUS / M_IN_KM;
}

deviceData *
handle_device(FILE *data, unsigned long point_count)
{
	deviceData *result = malloc(sizeof(deviceData));
	node *speed = NULL;
	node *speed_static = NULL;
	node *curr_node = NULL;
	node *real_speed = NULL;
	pointBlock curr_block = {0}, prev_block = {0};
	double way = 0;
	double real_way = 0;
	unsigned int time_static = 0, full_time = 0;
	int curr_real_speed = 0;
	double delta_way = 0;

	fread(&prev_block, sizeof(pointBlock), 1, data);

	full_time = prev_block.gps_time;

	insert(&speed, prev_block.speed, 1);

	for (int i = 1; i < point_count; i++) {
		fread(&curr_block, sizeof(pointBlock), 1, data);

		if (curr_block.gps_time - prev_block.gps_time == 0) {
			printf("Data error: two consequent points have equal timestamps");
			printf(", ignoring the second:");
			printf("%d %d\n", curr_block.gps_time, prev_block.gps_time);
			continue;

		} else if (curr_block.gps_time - prev_block.gps_time < 0) {
			printf("Data error: two consequent points are reversed in time");
			printf(", ignoring the second:");
			printf("%d %d\n", curr_block.gps_time, prev_block.gps_time);
			continue;
		}

		if (curr_block.speed < 0) {
			printf("Data error: speed is negative, ignoring: %d", 
					curr_block.speed);
			continue;
		}

		/* Trivial linear interpolation between two nearby points. If such 
		precision is not enough, more complex ways like Lagrange polynomials
		should be implemented.
		*/

		way += (curr_block.gps_time - prev_block.gps_time) * 
								(prev_block.speed + curr_block.speed) / 2;

		if (curr_block.speed == 0 && prev_block.speed == 0) {
			time_static += curr_block.gps_time - prev_block.gps_time;
		}

		if ((curr_node = find(speed, curr_block.speed))) {
			insert(&speed, curr_node->key, curr_node->value + 1);
		}
		else {
			insert(&speed, curr_block.speed, 1);
		}

		delta_way = calc_distance((double)curr_block.lat, 
						(double)curr_block.lon, (double)prev_block.lat, 
													(double)prev_block.lon);

		real_way += delta_way;

		curr_real_speed = delta_way * S_IN_HR / ((curr_block.gps_time - 
									prev_block.gps_time));

		/* Binary search tree is implemented to store pairs of speed values and
		   how many times each speed value is met.
		*/

		if ((curr_node = find(real_speed, curr_real_speed))) {
			insert(&real_speed, curr_node->key, curr_node->value + 1);
		}
		else {
			insert(&real_speed, curr_real_speed, 1);
		}

		prev_block = curr_block;
	}

	full_time = curr_block.gps_time - full_time;	
	result->time_static = time_static;
	
	if (!full_time || !(full_time - time_static)) {
		result->avg_speed_motion = 0;
		result->real_avg_speed = 0;
	}
	else {
		result->avg_speed = way / (full_time);
		result->avg_speed_motion = way / ((full_time - time_static));
		result->real_avg_speed = real_way * S_IN_HR / (full_time - time_static);
	}
	result->dispersion = calc_dispersion(speed);
	result->real_dispersion = calc_dispersion(real_speed);

	
	free_tree(speed);
	free_tree(real_speed);
	free_tree(speed_static);

	return result;
}
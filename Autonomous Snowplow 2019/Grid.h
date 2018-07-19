#pragma once
#define _USE_MATH_DEFINES

//general includes
#include "lidar_handler.h"
#include <iostream>
#include <vector>
#include <ctime>

//types
#include "snowplow_type.h"

using namespace std;

class Grid {

public:
									Grid(lidar_handler *lidar);

private:
	unsigned long long				prv_total_scans_mapped;
	lidar_hit_map					prv_hit_map;
	lidar_data_packet *				prv_data_packet;
	lidar_handler *					prv_lidar_ref;
	double                          prv_lidar_map_x_idx;
	double                          prv_lidar_map_y_idx;

public:
	void							print_hit_map();
	void							update_hit_map();

};
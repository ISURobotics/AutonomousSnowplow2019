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

class grid_handler {

public:
									grid_handler(lidar_handler * lidar, atomic<double> * orientation,
										atomic<double> * x_position, atomic<double> * y_position);

private:
	unsigned long long				prv_total_scans_mapped;
	lidar_hit_map					prv_hit_map;
	lidar_data_packet *				prv_data_packet;
	lidar_handler *					prv_lidar_ref;
	double                          prv_lidar_map_x_idx;
	double                          prv_lidar_map_y_idx;
	atomic<double> *                prv_orientation_ref;
	atomic<double> *                prv_x_pos_ref;
	atomic<double> *                prv_y_pos_ref;
	vector<snowplow_nav_point>      loc_point_list;

public:
	bool							update_hit_map();
	void							print_hit_map();
	void                            print_obj_map();
	bool is_obj_present();

};
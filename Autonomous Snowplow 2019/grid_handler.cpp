#include "grid.h"

/*---------------------------------------
Grid constructor - builds the hit map
based off of size and resolution of field
---------------------------------------*/
grid_handler::grid_handler(lidar_handler * lidar, atomic<double> * orientation,
	atomic<double> * x_position, atomic<double> * y_position) {
	
	/*---------------------------------------
	setting references
	---------------------------------------*/
	prv_total_scans_mapped = 0;
	prv_lidar_ref          = lidar;
	prv_x_pos_ref          = x_position;
	prv_y_pos_ref          = y_position;
	prv_orientation_ref    = orientation;

	/*---------------------------------------
	building x-axis
	---------------------------------------*/
	for (int i = 0; i < FIELD_WIDTH_M / MAP_RESOLUTION_M; i++) {
		vector<int> temp;
		/*---------------------------------------
		building y-axis
		---------------------------------------*/
		for (int j = 0; j < FIELD_LENGTH_M / MAP_RESOLUTION_M; j++) {
			temp.push_back(0);
		}
		prv_hit_map.push_back(temp);
	}
}

/*---------------------------------------
Adds lidar hits to the map
---------------------------------------*/
bool grid_handler::update_hit_map() {
	if (!prv_lidar_ref->data_is_ready()) {
		cout << "Data isn't ready for grid to process. Grid update failed." << endl;
		return false;
	}
	if (*prv_x_pos_ref == NULL && *prv_y_pos_ref == NULL) {
		/*---------------------------------------
		If the current position is 0,0 then it
		probably isn't a real positional update
		since the plow would be on the post. pos
		starts at 0,0 so dont update until real 
		pos update.
		---------------------------------------*/
		cout << "Unable to update hit map, invalid position at 0.0, 0.0." << endl;
		return false;
	}
	prv_data_packet = prv_lidar_ref->get_data();

	/*---------------------------------------
	Go through and map each data point that 
	the	lidar returned. Get current
	orientation of the lidar for this set of
	points as well as position.
	---------------------------------------*/
	double orientation = *prv_orientation_ref;
	double x_pos = *prv_x_pos_ref;
	double y_pos = *prv_y_pos_ref;
	for (int i = 0; i < LIDAR_DATA_POINTS; i++) {

		/*---------------------------------------------
		Following does this for each point:
		1. get angle and distance pair from lidar data
		2. if angle is negative make it positive
		2b. adjust angle for plow orientation
		3. turn that pair into an x, y location
		4. if its outside the map dont map it
		5. count it unless count is at 999
		---------------------------------------------*/
		double angle      = get< 0 >( ( * prv_data_packet )[ i ]);
		double distance   = get< 1 >( ( * prv_data_packet )[ i ]);

		if (angle < 0) {
			angle = 360 - abs(angle);
		}

		if (orientation >= 0 && orientation < 180 ) {
			angle += orientation;
		}
		else {
			angle -= (360.0 - orientation);
		}

		double x_loc = ( cos( angle * M_PI / 180.0) * distance ) + x_pos;
		double y_loc = ( sin( angle * M_PI / 180.0) * distance ) + y_pos;
			

		//temporary for debugging
		if (i == (floor(LIDAR_DATA_POINTS / 2))) {
			//cout << "Location at 90 deg: " << x_loc << ", " << y_loc << endl;
			//cout << "Orientation and angle: " << orientation << ", " << angle << endl;
		}


		int x_index = floor(x_loc / MAP_RESOLUTION_M);
		int y_index = floor(y_loc / MAP_RESOLUTION_M);

		if (x_index < 0 || x_index > prv_hit_map.size() - 1 || y_index < 0 || y_index > prv_hit_map[0].size() - 1) {
			continue;
		}

		if (prv_hit_map[floor(x_loc / MAP_RESOLUTION_M)][floor(y_loc / MAP_RESOLUTION_M)] < 999) {
			prv_hit_map[floor(x_loc / MAP_RESOLUTION_M)][floor(y_loc / MAP_RESOLUTION_M)] += 1;
		}

	}
	prv_total_scans_mapped++;
	prv_lidar_map_x_idx = floor((x_pos) / MAP_RESOLUTION_M);
	prv_lidar_map_y_idx = floor((y_pos) / MAP_RESOLUTION_M);

	return true;
}

/*---------------------------------------
prints the lidar hit map so that 0,0 is
in the bottom left corner
---------------------------------------*/
void grid_handler::print_hit_map() {
	string map = "";
	/*-------------------------------------------------
	  ^^^^^^^^^^      Need to print the map with y
	y ||||||||||      starting at max and x starting at
	  --------->      0 since this is a vector of
	      x           vectors.
	-------------------------------------------------*/ 
	for (int y = prv_hit_map[0].size() - 1; y >= 0; y--) {
		for (int x = 0; x <  prv_hit_map.size(); x++) {

			if (prv_lidar_map_x_idx == x && prv_lidar_map_y_idx == y) {
				map.append("LLLL");
				continue;
			}

			map.append(to_string(prv_hit_map[x][y]));
			string test = to_string(prv_hit_map[x][y]);
			for (int w = 0; w < 3 - test.length(); w++) {
				map.append(" ");
			}
			map.append(" ");
		}
		map.append("\n\n");
	}
	cout << map;
}

/*---------------------------------------
prints the lidar hit map so that 0,0 is
in the bottom left corner
---------------------------------------*/
void grid_handler::print_obj_map() {
	string map = "";
	/*-------------------------------------------------
	  ^^^^^^^^^^      Need to print the map with y
	y ||||||||||      starting at max and x starting at
	  --------->      0 since this is a vector of
	      x           vectors.
	-------------------------------------------------*/
	for (int y = prv_hit_map[0].size() - 1; y >= 0; y--) {
		for (int x = 0; x < prv_hit_map.size(); x++) {

			if (prv_lidar_map_x_idx == x && prv_lidar_map_y_idx == y) {
				map.append("L");
				continue;
			}

			if (prv_hit_map[x][y] > MAP_OBJ_THRESH) {
				map.append("M");
			}
			else {
				map.append(".");
			}

			map.append(" ");
		}
		map.append("\n");
	}
	cout << map;
}

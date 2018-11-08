#pragma once
#include <ctime>
#include <cmath>
#include "snowplow_type.h"

#define PI ( 3.14159265 )

class dead_reckoning {

public:
	dead_reckoning(atomic<double> * x_pos, atomic<double> * y_pos, atomic<double> * orientation, 
		drive_data_pkt * drive_pkt, atomic<long> * timestamp);

private:
	atomic<double> * prv_x_ref;
	atomic<double> * prv_y_ref;
	atomic<double> * prv_ori_ref;
	drive_data_pkt * prv_drive_pkt_ref;
	atomic<long> * prv_timestamp_ref;

	double x_prev;
	double y_prev;
	double time_initial;
	double delta_time;
	double deca_x;
	double deca_y;

public:
	void UpdatePosition();

};
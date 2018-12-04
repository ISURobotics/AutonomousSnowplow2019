#pragma once
#define _USE_MATH_DEFINES

#include "snowplow_type.h"
#include "motor_interface.h"
#include <math.h>
#include"wayqueue.h"

class navigation_handler {
public:
									navigation_handler(atomic<double> * orientation, atomic<double> * x_pos,
										atomic<double> * y_pos, motor_interface * motor, Wayqueue * q);

private:
	motor_interface *               prv_motor_ref;
	snowplow_nav_point              prv_target;
	atomic<double> *                prv_ori_ref;
	atomic<double> *                prv_x_ref;
	atomic<double> *                prv_y_ref;
	double                          prv_goal_orientation;
	Wayqueue *                      prv_queue;

public:
	void                            update( drive_data_pkt * );

private:
	unsigned char                   get_turn_power(double dist);
	unsigned char                   get_straight_power(double dist);
	double                          get_orientation(double cur_x, double cur_y);
};
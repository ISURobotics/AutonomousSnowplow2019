#include "dead_reckoning.h"
#include <ctime>

dead_reckoning::dead_reckoning(atomic<double> * x_pos, atomic<double> * y_pos, atomic<double> * orientation,
	drive_data_pkt * drive_pkt, atomic<long> * timestamp) {
	deca_x = *x_pos;
	deca_y = *y_pos;

	prv_x_ref = x_pos;
	prv_y_ref = y_pos;
	prv_ori_ref = orientation;
	prv_drive_pkt_ref = drive_pkt;
	prv_timestamp_ref = timestamp;
}

/*
The update method will know the last time the actual position was updated
- if the driving operation is turning then don't do any position updates
- since the only other operation is driving forwards without turning then
we don't have to worry about calculating any turns
*/
void dead_reckoning::UpdatePosition() {

	if (prv_drive_pkt_ref->drive_op == RIGHT || prv_drive_pkt_ref->drive_op == LEFT ||
		prv_drive_pkt_ref->drive_op == STOP) {
		//we only want to update the position if its moving but also mark time stamp so 
		//that we know it tried to update
		(*prv_timestamp_ref) = clock();
		//mark that we will need to get the a first moving position when it starts updating
		//before we can start updating the position
		return;
	}

	//if it gets to this point then we know its driving forwards
	//use the difference in time and intensity to calculate new position

	double delta_time = ((double)clock() - ((double)*prv_timestamp_ref)) / (double)CLOCKS_PER_SEC;
	double cur_orientation = (*prv_ori_ref + 90.0), x = *prv_x_ref, y = *prv_y_ref;

	if (cur_orientation > 360) {
		cur_orientation = cur_orientation - 360.0;
	}

	double speed = (prv_drive_pkt_ref->intensity / 255.0);//m/s
	double x_inc = cos(cur_orientation * PI / 180.0) * (speed * delta_time);
	double y_inc = sin(cur_orientation * PI / 180.0) * (speed * delta_time);

	*prv_x_ref = x + x_inc;
	*prv_y_ref = y + y_inc;

	//cout << "orientation: " << cur_orientation << " x increased by: " << x_inc << " and y increased by: " << y_inc << endl;
	
	//update the timestamp for a location change
	(*prv_timestamp_ref) = clock();

}
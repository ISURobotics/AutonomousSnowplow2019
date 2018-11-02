#include "navigation_handler.h"

navigation_handler::navigation_handler(atomic<double> * orientation, atomic<double> * x_pos,
	atomic<double> * y_pos, motor_interface * motor_com) {
	prv_motor_ref = motor_com;
	prv_ori_ref = orientation;
	prv_x_ref   = x_pos;
	prv_y_ref   = y_pos;

#if( NAV_POINT_METHOD == MANUAL )
	/*---------------------------------------
	Set the first manual prv_target
	---------------------------------------*/
	cout << "Enter an x coordinate: ";
	cin >> prv_target.x;
	cout << "Enter a y coordinate: ";
	cin >> prv_target.y;

#elif( NAV_POINT_METHOD == LIST )

//need functionality to grab list and set first point

#endif

	/*---------------------------------------
	Find goal orientation
	---------------------------------------*/
	prv_goal_orientation = ( 180.0 * atan2( prv_target.y - *prv_y_ref, prv_target.x - *prv_x_ref ) ) / M_PI;
	prv_goal_orientation -= 90;
	if (prv_goal_orientation < 0) {
		prv_goal_orientation = 360 + prv_goal_orientation;
	}
	cout << "Goal orientation: " << prv_goal_orientation << endl;

}

void navigation_handler::update( drive_data_pkt * drive_pkt ) {
	/*---------------------------------------
	Make a copy to compare at the end to tell
	whether an updated drive packet needs to
	be sent to the arduino
	---------------------------------------*/
	drive_data_pkt drive_pkt_before;
	memcpy(&drive_pkt_before, drive_pkt, sizeof(drive_data_pkt));

	/*---------------------------------------------
	check to see if the plow is at the prv_target.
	get coords first so they stay the same for this
	series of calculations
	---------------------------------------------*/
	double cur_x = *prv_x_ref;
	double cur_y = *prv_y_ref;
	if( ( ( cur_x > ( prv_target.x - NAV_POINT_THRESH_M ) ) 
	 &&   ( cur_x < ( prv_target.x + NAV_POINT_THRESH_M ) ) )
	 && ( ( cur_y > ( prv_target.y - NAV_POINT_THRESH_M ) )
	 &&   ( cur_y < ( prv_target.y + NAV_POINT_THRESH_M ) ) ) ) {

#if( NAV_POINT_METHOD == MANUAL )

		/*---------------------------------------
		its at the point so stop the robot and 
		wait for the next input point
		---------------------------------------*/
		drive_pkt->drive_op = STOP;
		drive_pkt->changed = true;
		prv_motor_ref->send_pkt_to_motors();
		
		cout << "Reached target. Enter a new point" << endl;
		cout << "Enter an x coordinate: ";
		cin >> prv_target.x;
		cout << "Enter a y coordinate: ";
		cin >> prv_target.y;

#elif( NAV_POINT_METHOD == LIST )

		//need functionality to grab next target from list
		//if reached final point return stop for drive op

#endif

		/*---------------------------------------
		Recalculate orientation for next target
		---------------------------------------*/
		prv_goal_orientation = ( 180.0 * atan2( prv_target.y - cur_y, prv_target.x - cur_x ) ) / M_PI;
		if (prv_goal_orientation < 0) {
			prv_goal_orientation = 360 + prv_goal_orientation;
		}
		prv_goal_orientation -= 90;

	}
	else {

		/*---------------------------------------------
		calculate orientation boundaries. modified is
		to signal when the goal +/- tolerance contains
		thye 0 degree mark because math gets confusing
		---------------------------------------------*/
		bool modified = false;
		double lower_bound = prv_goal_orientation - ORI_THRESH_D;
		if (lower_bound < 0) {
			lower_bound += 360.0;
			modified = true;
		}
		double higher_bound = prv_goal_orientation + ORI_THRESH_D;
		if (higher_bound > 360) {
			higher_bound -= 360;
			modified = true;
		}

		/*---------------------------------------------
		Check if orientation is within its tolerance.
		If not return drive command
		---------------------------------------------*/
		double cur_ori = *prv_ori_ref;
		if( modified && ( ( cur_ori > ( lower_bound ) )
	     || ( cur_ori < ( higher_bound ) ) ) ) {
			//cout << "current orientation works." << endl;
		}
		else if (!modified && ( ( cur_ori > ( lower_bound ) )
	     && ( cur_ori < ( higher_bound ) ) ) ) {
			//cout << "current orientation works." << endl;
		}
		else {
			/*---------------------------------------------
			Orientation isn't correct at this point. 
			Depending on whether the current orientation is
			closer to the lower_bound or higher_bound the
			drive operation will be chosen
			---------------------------------------------*/
			if( modified ) {
				if( abs( lower_bound - cur_ori ) > abs( higher_bound - cur_ori ) ) {
					//cout << "Need to turn right." << endl;
					drive_pkt->drive_op  = RIGHT;
					drive_pkt->intensity = get_turn_power( abs( higher_bound - cur_ori ) );

				}
				else {
					//cout << "Need to turn left." << endl;
					drive_pkt->drive_op  = LEFT;
					drive_pkt->intensity = get_turn_power( abs( lower_bound - cur_ori ) );
				}
			}
			else {
				double l_turn_deg = 0.0, r_turn_deg = 0.0;
				if (cur_ori < prv_goal_orientation) {
					l_turn_deg = prv_goal_orientation - cur_ori;
					r_turn_deg = cur_ori + ( 360.0 - prv_goal_orientation );
				}
				else {
					l_turn_deg = prv_goal_orientation + ( 360.0 - cur_ori );
					r_turn_deg = cur_ori - prv_goal_orientation;
				}
				if (l_turn_deg < r_turn_deg) {
					//cout << "Need to turn left. Non-modified." << endl;
					drive_pkt->drive_op  = LEFT;
					drive_pkt->intensity = get_turn_power( l_turn_deg );
				}
				else {
					//cout << "Need to turn right. Non-modified." << endl;
					drive_pkt->drive_op  = RIGHT;
					drive_pkt->intensity = get_turn_power( r_turn_deg );
				}
			}
			/*---------------------------------------------
			check for a change to any value. if so send
			new packet to arduino
			---------------------------------------------*/
			if (memcmp( &drive_pkt_before, drive_pkt, sizeof( drive_data_pkt ) ) ) {
				drive_pkt->changed = true;
				cout << "packet values changed" << endl;
			}
			return;
		}
		/*--------------------------------------------------
		Orientation is correct - set drive mode and power
		--------------------------------------------------*/
		drive_pkt->drive_op  = STRAIGHT;
		drive_pkt->intensity = get_straight_power( sqrt( pow( ( prv_target.x - cur_x ), 2 )+pow( ( prv_target.y - cur_y ), 2 ) ) );

		/*---------------------------------------------
		check for a change to any value. if so send
		new packet to arduino
		---------------------------------------------*/
		if (memcmp(&drive_pkt_before, drive_pkt, sizeof(drive_data_pkt))) {
			drive_pkt->changed = true;
			cout << "packet values changed" << endl;
		}
	}
}

unsigned char navigation_handler::get_turn_power( double deg ) {
	if ( deg >= 120.0 ) { 
		return ( floor( 1.0 * SPEED_SCALAR * UCHAR_MAX ) );
	}
	else if ( deg >= 90.0 ) {
		return ( floor( 0.8 * SPEED_SCALAR * UCHAR_MAX ) );
	}
	else if ( deg >= 45.0 ) {
		return ( floor( 0.5 * SPEED_SCALAR * UCHAR_MAX ) );
	}
	else {
		return ( floor( 0.35 * SPEED_SCALAR * UCHAR_MAX ) );
	}
}

unsigned char navigation_handler::get_straight_power( double dist ) {
	if ( dist >= 2.0 ) {
		return ( floor( 1.0 * SPEED_SCALAR * UCHAR_MAX ) );
	}
	else if ( dist >= 1.0 ) {
		return ( floor( 0.6 * SPEED_SCALAR * UCHAR_MAX ) );
	}
	else if ( dist >= 0.5 ) {
		return ( floor( 0.4 * SPEED_SCALAR * UCHAR_MAX ) );
	}
	else {
		return ( floor( 0.3 * SPEED_SCALAR * UCHAR_MAX ) );
	}
}
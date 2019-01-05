#include "navigation_handler.h"
#include "wayqueue.h"

navigation_handler::navigation_handler(atomic<double> * orientation, atomic<double> * x_pos,
	atomic<double> * y_pos, motor_interface * motor_com, Wayqueue * queue, atomic<bool>* obj_pres1) {
	obj_pres = obj_pres1;
	prv_queue = queue;
	prv_motor_ref = motor_com;
	prv_ori_ref = orientation;
	prv_x_ref   = x_pos;
	prv_y_ref   = y_pos;

	//Wayqueue queue;
	//Point p1, p2, p3, p4;
	//p1.x = 1.5; p1.y = 1.3;
	//p2.x = 1.0; p2.y = 6.5;
	//p3.x = 6.5; p3.y = 6.5;
	//p4.x = 6.5; p4.y = 1.0;
	//queue.push(&p1);
	//queue.push(&p2);
	//queue.push(&p3);
	//queue.push(&p4);
	//queue.push(&p1);


#if( NAV_POINT_METHOD == MANUAL )
	/*---------------------------------------
	Set the first manual prv_target
	---------------------------------------*/
	cout << "Enter an x coordinate: ";
	cin >> prv_target.x;
	cout << "Enter a y coordinate: ";
	cin >> prv_target.y;

#elif( NAV_POINT_METHOD == LIST )

	if ((*prv_queue).getSize() > 0) {
		Point * temp = (*prv_queue).pop();
		cout << "x: " << temp->x << " y: " << temp->y << endl;
		prv_target.x = temp->x;
		prv_target.y = temp->y;
	}
	else {
		cout << "Quitting. No points in queue." << endl;
		exit(0);
	}

#endif

	/*---------------------------------------
	Find goal orientation
	---------------------------------------*/
	prv_goal_orientation = ( 180.0 * atan2( prv_target.y - ( *prv_y_ref ), prv_target.x - ( *prv_x_ref ) ) ) / M_PI;
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
	double distance_to_point = sqrt(pow((prv_target.x - cur_x), 2) + pow((prv_target.y - cur_y), 2));
	if( distance_to_point <= NAV_POINT_THRESH_M ) {

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

		//if reached final point return stop for drive op
		if ((*prv_queue).getSize() > 0) {
			Point * temp = (*prv_queue).pop();
			cout << "x: " << temp->x << " y: " << temp->y << endl;
			int x_add = 0.0;
			if (*obj_pres) { x_add = 2.0; 
			cout << "*******************************************object detected" << endl;
			}
			prv_target.x = temp->x + x_add;
			prv_target.y = temp->y;
		}
		else {
			drive_pkt->drive_op = STOP;
			drive_pkt->changed = true;
			prv_motor_ref->send_pkt_to_motors();
			cout << "Quitting..." << endl;
			exit(0);
		}

#endif

		prv_goal_orientation = get_orientation(cur_x, cur_y);

	}
	else {

		prv_goal_orientation = get_orientation(cur_x, cur_y);

		/*---------------------------------------------
		calculate orientation boundaries. modified is
		to signal when the goal +/- tolerance contains
		the 0 degree mark because math gets confusing
		---------------------------------------------*/

		double orientation_tolerance = atan( NAV_POINT_THRESH_M / distance_to_point ) * 180.0 / M_PI/2.0;
		orientation_tolerance = max(orientation_tolerance, ORI_THRESH_D);
		//cout << "orientation tolerance is: " << orientation_tolerance << endl;

		bool modified = false;
		double lower_bound = prv_goal_orientation - (orientation_tolerance);
		if (lower_bound < 0) {
			lower_bound += 360.0;
			modified = true;
		}
		double higher_bound = prv_goal_orientation + (orientation_tolerance);
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
		return ( floor( 0.65 * SPEED_SCALAR * UCHAR_MAX ) );
	}
	else {
		return ( floor( 0.40 * SPEED_SCALAR * UCHAR_MAX ) );
	}
}

unsigned char navigation_handler::get_straight_power( double dist ) {
	if ( dist >= 2.0 ) {
		return ( floor( 1.0 * SPEED_SCALAR * UCHAR_MAX ) );
	}
	else if ( dist >= 1.0 ) {
		return ( floor( 0.8 * SPEED_SCALAR * UCHAR_MAX ) );
	}
	else if ( dist >= 0.5 ) {
		return ( floor( 0.6 * SPEED_SCALAR * UCHAR_MAX ) );
	}
	else {
		return ( floor( 0.5 * SPEED_SCALAR * UCHAR_MAX ) );
	}
}

double navigation_handler::get_orientation(double cur_x, double cur_y) {

	/*---------------------------------------
	Recalculate orientation for next target
	---------------------------------------*/
	double goal_orientation = (180.0 * atan2(prv_target.y - cur_y, prv_target.x - cur_x)) / M_PI;
	if (goal_orientation < 0.0) {
		goal_orientation = 360.0 + goal_orientation;
	}

	goal_orientation -= 90.0;

	return goal_orientation;
}


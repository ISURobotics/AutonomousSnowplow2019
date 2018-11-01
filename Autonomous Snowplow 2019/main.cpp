#include <ctime>
#include <thread>

#include "snowplow_type.h"
#include "lidar_handler.h"
#include "orientation_handler.h"
#include "local_handler.h"
#include "grid.h"
#include "navigation_handler.h"
#include "motor_interface.h"
#include "stopsign_detector.h"

/*----------------------------------------------------------------
                          main variables
----------------------------------------------------------------*/
unsigned long              main_loop_iterations = 0;
unsigned long              total_failed_scans = 0;
atomic<double>                  orientation = 0.0;
atomic<double>                  x_position = NULL;
atomic<double>                  y_position = NULL;
atomic<bool>					stopsign_detected = NULL;
drive_data_pkt                  drive_pkt = { STOP, 0x00, TRUE, 0x5 };

int main() {

#if MAIN_TIMING
	auto current_time = clock();
#endif
	//start timer for main loop print statements
	auto print_timer = clock();

	/*---------------------------------------
	construct interfaces - they take in refs
	to atomic variables so that their object
	can safely modify them without having to 
	return anything. They can also read them
	whenever needed safely.
	---------------------------------------*/
	lidar_handler          Lidar;
	orientation_handler    Orientation(&orientation);
	decawave_handler       Location(&x_position, &y_position);
	grid_handler           Grid(&Lidar, &orientation, &x_position, &y_position);
	navigation_handler     Nav(&orientation, &x_position, &y_position);
	motor_interface        Motor(&drive_pkt);
	stopsign_detector	   ss_det(false, &stopsign_detected);

	/*---------------------------------------
	start orientation and location threads.
	---------------------------------------*/
	thread location_thread(&decawave_handler::run, Location);
	thread orientation_thread(&orientation_handler::run, Orientation);
	thread stopsign_thread(&stopsign_detector::run, ss_det);

	/*---------------------------------------
	main snowplow execution loop
	---------------------------------------*/
	while (1) {

		/*-----------------------------------------------------------------
		Check for stopsign. if there is one copy current command, send
		stop command, when stopsign is gone send previous command to resume
		operation
		-----------------------------------------------------------------*/
		if (stopsign_detected) {
			cout << "Stopsign detected." << endl;
			drive_data_pkt ddp_copy;
			memcpy(&ddp_copy, &drive_pkt, sizeof(drive_data_pkt));
			drive_pkt.drive_op = STOP;
			drive_pkt.changed = TRUE;
			Motor.send_pkt_to_motors();
			cout << "Send stop command." << endl;

			Sleep(500);//waiting for atleast half a second
			while (stopsign_detected) {
				//wait for stop sign to go away
			}

			memcpy(&drive_pkt, &ddp_copy, sizeof(drive_data_pkt));
			drive_pkt.changed = TRUE;
			Motor.send_pkt_to_motors();
			cout << "Resuming operation." << endl;
			//resume previous command
		}

		/*---------------------------------------
		perform scan and check for success
		---------------------------------------*/
		//if (!Lidar.perform_scan()) {
		//	cout << "Error performing scan. Trying again..." << endl;
		//	total_failed_scans += 1;
		//	continue;
		//}

		/*---------------------------------------
		analyze scan (change raw hex to angle and
		distance pairs)
		---------------------------------------*/
		//Lidar.analyze_scan();

		/*---------------------------------------
		update map of hits
		---------------------------------------*/
		//if (!Grid.update_hit_map()) {
		//	cout << "Error updating hit map. Trying again..." << endl;
		//	continue;
		//}

		/*---------------------------------------
		get drive operation from nav interface
		---------------------------------------*/
		Nav.update( &drive_pkt );

		/*---------------------------------------
		send drive operation to motors
		---------------------------------------*/
		Motor.send_pkt_to_motors();

		/*---------------------------------------
		print hit/object map - every 1.5s
		---------------------------------------*/
		if ((clock() - print_timer)/(double)CLOCKS_PER_SEC * 1000 > 1000) {
			print_timer = clock();
			cout << "=====================================================" << endl;
			cout << "power:" << ((double)drive_pkt.intensity)/255.0*100.0 << "%" << endl;
			//Grid.print_obj_map();
			cout << "Total failed scans: " << total_failed_scans << " ";
			cout << "Total scans mapped: " << main_loop_iterations << endl;

			//testing stuff
			cout << "Orientation: " << orientation << endl;
			cout << "x pos: " << x_position << endl;
			cout << "y pos: " << y_position << endl;
		}		

#if MAIN_TIMING
		//calculate run time
		double duration = (std::clock() - current_time) / (double)CLOCKS_PER_SEC * 1000.0;
		current_time = clock();
		//don't print first reading
		if (i != 0) {
			std::cout << "Main loop execution time: " << duration << " ms\n";
		}
#endif

		main_loop_iterations++;
	}

}
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
#include "SerialPort.h"
#include "temperature_interface.h"
#include "dead_reckoning.h"
#include "wayqueue.h"
#include "path.h"

/*----------------------------------------------------------------
                          main variables
----------------------------------------------------------------*/
unsigned long long              main_loop_iterations = 0;
unsigned long long              total_failed_scans = 0;
atomic<long>                    location_last_updated = NULL;
atomic<double>                  orientation = 0.0, temperature = NULL;
atomic<double>                  x_position = NULL, y_position = NULL;
atomic<bool>					stopsign_detected = NULL, location_ready = false, orientation_ready = false, obj_pres = false;
drive_data_pkt                  drive_pkt = { STOP, 0x00, TRUE, 0x5 };
Wayqueue                        main_point_queue;
bool                            found_obj = false;

int main() {

#if MAIN_TIMING
	auto current_time = clock();
#endif
	//start timer for main loop print statements
	auto print_timer = clock(), loop_timer = clock();

	/*---------------------------------------
	construct interfaces - they take in refs
	to atomic variables so that their object
	can safely modify them without having to
	return anything. They can also read them
	whenever needed safely.
	---------------------------------------*/
	lidar_handler          Lidar;
	orientation_handler    Orientation(&orientation, &orientation_ready);
	decawave_handler       Location(&x_position, &y_position, &location_ready, &location_last_updated);
	grid_handler           Grid(&Lidar, &orientation, &x_position, &y_position);
	motor_interface        Motor(&drive_pkt);
	stopsign_detector	   ss_det(false, &stopsign_detected);
	temperature_interface  temp_track(&temperature);

	/*---------------------------------------
	start orientation, location, stopsign,
	and temperature threads.
	---------------------------------------*/
	thread                 location_thread(&decawave_handler::run, Location);
	thread                 orientation_thread(&orientation_handler::run, Orientation);
	thread                 stopsign_thread(&stopsign_detector::run, ss_det);
	thread                 temperature_thread(&temperature_interface::run, temp_track);
	Sleep(10);

	/*---------------------------------------
	field coverage
	---------------------------------------*/
	Path                   coverage;

#if( FIELD == SINGLE_I )

	/*---------------------------------------
	building path for single I field
	According to the rules there won't be any
	object in the snow path so no obj avoid
	---------------------------------------*/
	/*coverage.setMapAttributes(FIELD_WIDTH_M, FIELD_LENGTH_M, 100);
	coverage.initializeMap();
	coverage.setBoxMeshAttributes(11, 1, 200, 300, 100);
	coverage.initializeBoxMesh();
	coverage.initializeBasicPath(main_point_queue);
	main_point_queue.display();*/

	Point temp_s1;
	temp_s1.x = 2;
	temp_s1.y = 13;
	main_point_queue.push(&temp_s1);
	//Point temp_s2;
	//temp_s2.x = 2;
	//temp_s2.y = 13;

#elif( FIELD == DOUBLE_I )

	//manually push points into wayqueue since our bot can't handle going back and forth

	double cov_x_pos = 1.5;

	for (double i = 3.0; i <= 12.0; i+=1.0) {
		Point temp;
		temp.x = cov_x_pos;
		temp.y = i;
		main_point_queue.push(&temp);
	}
	cov_x_pos = 2.5;
	for (double i = 12.0; i >= 3.0; i -= 1.0) {
		Point temp;
		temp.x = cov_x_pos;
		temp.y = i;
		main_point_queue.push(&temp);
	}
	Point temp;
	temp.x = 2.0;
	temp.y = 2.0;
	main_point_queue.push(&temp);

	main_point_queue.display();

#endif

	Point temp_end;
	temp_end.x = 2;
	temp_end.y = 2;
	main_point_queue.push(&temp_end);
	/*---------------------------------------
	wait for first orientation and location
	readings
	---------------------------------------*/
	cout << "Waiting for initial location and orientation readings." << endl;
	while (!orientation_ready && !location_ready) {}

	/*--------------------------------------------
	Construct the navigation interface after
	we know we have one reading from orientation
	and location because the set orientation and
	location will determine the next operation.
	--------------------------------------------*/
	navigation_handler     Nav(&orientation, &x_position, &y_position, &Motor, &main_point_queue, &obj_pres);
	dead_reckoning         dead_reck(&x_position, &y_position, &orientation, &drive_pkt, &location_last_updated);

	/*---------------------------------------
	main snowplow execution loop
	---------------------------------------*/
	while (1) {

		//a way to control the speed that this loop executes
		while ((clock() - loop_timer) / (double)CLOCKS_PER_SEC * 1000 < (1000/100)) {}
		loop_timer = clock();

		/*---------------------------------------
		use dead reckoning to update position
		---------------------------------------*/
		dead_reck.UpdatePosition();

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

			Sleep(5000);//waiting for atleast 5 seconds
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
		if (!Lidar.perform_scan()) {
			cout << "Error performing scan. Trying again..." << endl;
			total_failed_scans += 1;
			continue;
		}

		/*---------------------------------------
		analyze scan (change raw hex to angle and
		distance pairs)
		---------------------------------------*/
		Lidar.analyze_scan();

		/*---------------------------------------
		update map of hits
		---------------------------------------*/
		if (!Grid.update_hit_map()) {
			cout << "Error updating hit map. Trying again..." << endl;
			continue;
		}

		/*---------------------------------------
		check for object in front of plow
		---------------------------------------*/
#if ( FIELD == DOUBLE_I )
		if (Grid.is_obj_present() && !found_obj && (!(orientation > 15 && orientation < 165)|| !(orientation > 195 && orientation < 345))) {

			coverage.avoid_obstacle(main_point_queue);
			main_point_queue.display();
			Nav.skip_point();//pops to next point which isn't in front of the object
			//sleep(5000);
			found_obj = true;
		}
#endif
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
		if ((clock() - print_timer)/(double)CLOCKS_PER_SEC * 1000 > 500) {
			print_timer = clock();
			cout << "=====================================================" << endl;
			cout << "power:" << ((double)drive_pkt.intensity)/255.0*100.0 << "%" << endl;
			//Grid.print_obj_map();
			cout << "Total failed scans: " << total_failed_scans << " ";
			cout << "Total scans mapped: " << main_loop_iterations << endl;

			//testing stuff
			cout << "Orientation: " << orientation << endl;
			cout << "x pos: " << x_position;
			cout << ", y pos: " << y_position << endl;
			cout << "temperature: " << temperature << endl;
			cout << "drive operation: " << (int)drive_pkt.drive_op << endl;
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
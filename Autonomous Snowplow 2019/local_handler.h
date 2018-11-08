#pragma once
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctime>

#include "SerialPort.h"
#include "snowplow_type.h"
#include "data_log.h"

using namespace std;

#define PRINT_RECENT_DATA_PKT ( FALSE )
#define PRINT_PARSED_LOCATION ( FALSE )

class decawave_handler {

public:
							        decawave_handler(atomic<double> * x_ref, atomic<double> * y_ref, atomic<bool> * location_ready, atomic<long> * decawave_reading_timestamp);

private:
	atomic<bool> *                  prv_loc_ready;
	atomic<double> *                prv_x_pos_ref;
	atomic<double> *                prv_y_pos_ref;
	atomic<long> *                  prv_time_stamp_ref;

public:
	void					        run();

};
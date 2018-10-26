#pragma once
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SerialPort.h"
#include "snowplow_type.h"

using namespace std;

#define PRINT_RECENT_DATA_PKT ( FALSE )
#define PRINT_PARSED_LOCATION ( FALSE )

class decawave_handler {

public:
							        decawave_handler(atomic<double> * x_ref, atomic<double> * y_ref);

private:
	atomic<double> *                prv_x_pos_ref;
	atomic<double> *                prv_y_pos_ref;

public:
	void					        run();

};
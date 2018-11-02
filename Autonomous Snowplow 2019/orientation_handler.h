#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <functional>
#include <string>
#include <algorithm>
#include "SerialPort.h"
#include "snowplow_type.h"

class orientation_handler {

public:
	                    orientation_handler(atomic<double> * orientation, atomic<bool> * orientation_ready);

private:
	atomic<double> *    prv_ori_ref;
	atomic<bool> *      prv_ori_ready;

public:
	void                run();

};
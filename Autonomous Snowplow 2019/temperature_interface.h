#pragma once
#include "snowplow_type.h"
#include "SerialPort.h"
#include "data_log.h"

class temperature_interface {

public:
	temperature_interface(atomic<double> * temperature);
	void run();

private:
	atomic<double> * prv_temp_ref;

};
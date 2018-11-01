#pragma once

#include "snowplow_type.h"
#include <fstream>
#include <string>

#define BASE_PATH ( "C:/ISU Robotics/AutonomousSnowplow2019/QA & Testing/" )
#define TXT_EXT ( ".txt" )

class data_logger {
	string file_path;
public:
	             data_logger(string file_name);
	void         log_data(string data_to_log);
};
#include "data_log.h"

/*
This class creates a text file and then writes to it
whenever the log data method is called.
*/

data_logger::data_logger(string file_name) {
	string full_path = BASE_PATH + file_name + TXT_EXT;
	file_path = full_path;
	//create the file
	ofstream output_stream( full_path, ofstream::out);
	output_stream.close();
}

void data_logger::log_data(string data_to_log) {
	//open the file in appending mode
	ofstream output_stream(file_path, ios_base::app);
	output_stream << data_to_log << endl;
	//close the file to free system resources
	output_stream.close();
}
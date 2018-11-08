#include "temperature_interface.h"

temperature_interface::temperature_interface(atomic<double> * temperature) {
	prv_temp_ref = temperature;
}

void temperature_interface::run() {
	data_logger temp_log("temperature");
	Serial* arduino = new Serial(TEMP_COM_PORT);
	char* buf = (char*)malloc(6);
	buf[5] = '\0';
	while (1) {
		int numbytes = arduino->ReadData(buf, 5);
		if (numbytes >= 5) {
			//cout << "data read: " << buf << endl;
			double temp = stof(buf);
			temp_log.log_data(buf);
			*prv_temp_ref = temp;//set temp to current temp
		}
	}
}
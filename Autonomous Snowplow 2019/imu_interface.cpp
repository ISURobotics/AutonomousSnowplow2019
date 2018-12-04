#include "orientation_handler.h"
#include "data_log.h"

/*---------------------------------------
constructor for orientation interface.
This takes a reference to an atomic
double so its easy to modify the 
---------------------------------------*/
orientation_handler::orientation_handler(atomic<double> * orientation, atomic<bool> * orientation_ready) {
	prv_ori_ref   = orientation;
	prv_ori_ready = orientation_ready; //this flag is set true when the value start getting set to orientations
}

/*---------------------------------------
main orientation execution loop
---------------------------------------*/
void orientation_handler::run() {

	string buildingString = "";
	string mostRecent = "";
	string anotherFuckingString = "";
	const char *portName = IMU_COM_PORT;
	Serial* arduino = new Serial(portName);
	char incomingData[512] = "";
	int dataLength = 255;
	int readResult = 0;
	bool firstOpen = false;

	data_logger ori_log("orientation");

	/*--------------------------------------------------
	this works, but should probably be cleaned up
	--------------------------------------------------*/
	while (true)
	{ 
		readResult = arduino->ReadData(incomingData, dataLength);
		incomingData[readResult] = 0;
		if (firstOpen)
		{
			if (((string)incomingData).find(']') != -1)
			{
				buildingString += incomingData;
				mostRecent = buildingString;
				buildingString = "";
				firstOpen = false;
				string mysubstring = mostRecent.substr(mostRecent.find('[') + 1, mostRecent.find(']') - 1);
				anotherFuckingString = mysubstring;
				double ori = stod(anotherFuckingString);
				if (ori == 360.0) {
					ori = 0.0;
				}
				*prv_ori_ref = ori;
				ori_log.log_data(to_string(ori));
				*prv_ori_ready = true;
				continue;
			}
		}
		else
		{
			if (((string)incomingData).find('[') != -1)
			{
				firstOpen = true;
			}
		}

		buildingString += incomingData;
	}
}

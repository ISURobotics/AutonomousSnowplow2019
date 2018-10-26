#include "motor_interface.h"
#include <thread>
#include <ctime>

motor_interface::motor_interface( drive_data_pkt * drive_pkt ) {
	prv_drive_pkt_ref = drive_pkt;
	prv_serial_connection = new Serial( MOTOR_COM_PORT );

}

void motor_interface::send_pkt_to_motors() {
	/*---------------------------------------------
	check for a change to any value. if so send
	new packet to arduino
	---------------------------------------------*/
	if (prv_drive_pkt_ref->changed) {
		prv_serial_connection->WriteData((char*)&(prv_drive_pkt_ref->drive_op), 1);
		prv_serial_connection->WriteData((char*)&(prv_drive_pkt_ref->intensity), 1);
		prv_serial_connection->WriteData((char*)&(prv_drive_pkt_ref->stop_char), 1);
		/*---------------------------------------------
		mark as not changed since it sent
		---------------------------------------------*/
		prv_drive_pkt_ref->changed = false;
	}
}
#include <iostream>
#include <thread>
#include "uartCommunicator.h"

using namespace std;

class Sensor_task : public RTOS::task{
	public:
	Sensor_task(short prio, const char* name, UartCommunicator* uart):
		RTOS::task(prio, name),
		uartCom(uart)
		{}
		
	private:
		UartCommunicator* uartCom;
	
	private:
		void main(void){
			for (;;) {
				cout << "schrijf tempreq in queue" << endl;
				Requests req = { TEMPERATURE_REQ, STATUS_CMD};
				uartCom->writeCommand(req);
				sleep(2000 * 1000);
				short reply = uartCom->readReply();
				cout << dec << (reply >> 8) << hex << endl;
			}	
		}
};
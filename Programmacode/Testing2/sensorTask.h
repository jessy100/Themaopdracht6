#ifndef _SENSORTASK_H
#define _SENSORTASK_H

#include <iostream>
#include <thread>
#include "uartCommunicator.h"
#include "UpdatingSensor.h"

using namespace std;

class Sensor_task : public RTOS::task{
public:
	Sensor_task(short prio, const char* name):
		RTOS::task(prio, name)
		{}

	void addSensor(UpdatingSensor* s){
		sensors.push_back(s);
	}
		
private:
	vector<UpdatingSensor*> sensors;
	
private:
	void main(void){
		for (;;) {
			//call all updates
			for (std::vector<UpdatingSensor*>::iterator it = sensors.begin(); it != sensors.end(); ++it){
				cout << "roept SensorUpdate aan" << endl;
				(*it)->update();
				sleep(500 MS);
			}
			/*
			cout << "schrijf tempreq in queue" << endl;
			Requests req = { TEMPERATURE_REQ, STATUS_CMD};
			uartCom->writeCommand(req);
			sleep(2000 * 1000);
			short reply = uartCom->readReply();
			cout << dec << (reply >> 8) << hex << endl;
			*/
		}	
	}
};

#endif
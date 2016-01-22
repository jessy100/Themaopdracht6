/*!
*   @author     Jessy Visch, Zehna van den berg, Koen de Groot, Joost Wagensveld
*   @project    Themaopdracht 6, Wasmachine
*   @brief      Task which is respnsible for updating the Sensors.
*/

#ifndef _SENSORTASK_H
#define _SENSORTASK_H

#include <iostream>
#include <thread>
#include "uartCommunicator.h"
#include "UpdatingSensor.h"
#include "broadcaster.h"

using namespace std;

/** \class Sensor_task
	* \brief Is responsible for adding and updating sensor
	*/
class Sensor_task : public RTOS::task{
public:
	Sensor_task(short prio, const char* name, Broadcaster* b):
		RTOS::task(prio, name),
		b(b)
		{}
/** \brief addSensor adds a sensor to the sensors vector
	* \Param UpdatingSensor the instance of any sensor
*/
	void addSensor(UpdatingSensor* s){
		sensors.push_back(s);
	}
		
private:
	vector<UpdatingSensor*> sensors;
	Broadcaster* b;
	
private:
	void main(void){
		for (;;) {
			//call all updates
			for (std::vector<UpdatingSensor*>::iterator it = sensors.begin(); it != sensors.end(); ++it){
				(*it)->update();
				b->setWSData();
				//doe broadcaster aanroepen voor update
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
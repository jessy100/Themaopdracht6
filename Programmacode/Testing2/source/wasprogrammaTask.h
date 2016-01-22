/*!
*   @author     Jessy Visch, Zehna van den berg, Koen de Groot, Joost Wagensveld
*   @project    Themaopdracht 6, Wasmachine
*   @brief      Containts all logic to run a washingprogram 
*/

#ifndef _WASPROGRAMMA_TASK
#define _WASPROGRAMMA_TASK

#include <iostream>
#include <thread>
#include "uartCommunicator.h"
#include <fstream>
#include <string>
#include <queue>
#include "broadcaster.h"
#include "Sensor.h"

using namespace std;

/** \class WasProgrammaTask
	* \brief is the controller responsible for executing a washingprogram 
*/
class WasProgrammaTask : public RTOS::task{
	public:
	WasProgrammaTask(short prio, const char* name, UartCommunicator* uart, Broadcaster & b, WaterValve* waterValve, 
					WaterLevel* waterLevel, TempSensor* tempSensor, HeatingUnit* heatingUnit, Trommel* trommel, 
					SoapDispenser* soapDispenser, Pump* pump, DoorLock* doorLock):
		RTOS::task(prio, name),
		uartCom(uart),
		b(b),
		waterValve(waterValve),
		waterLevel(waterLevel),
		tempSensor(tempSensor),
		heatingUnit(heatingUnit),
		trommel(trommel),
		soapDispenser(soapDispenser),
		pump(pump),
		doorLock(doorLock)
		{}
		
	private:
		UartCommunicator* uartCom;
		int delay = 0;
		string line, filename = "was_bont.txt";
		string voorwas, hoofdwas_toeren;
		int max_water_level = 50;
		int max_temp = 40;
		WaterValve* waterValve;
		WaterLevel* waterLevel;
		TempSensor* tempSensor;
		HeatingUnit* heatingUnit;
		Trommel* trommel;
		SoapDispenser* soapDispenser;
		Pump* pump;
		DoorLock* doorLock;

		//test
		bool done = false,afpompen = false, inProgress = false, doneFilling = false, doneHeating = false,doneRotating = false, donePumping = false, doorLocked = false;
	
	private:
		Broadcaster & b;
		queue<string> *commands	;

	/** \brief loadWasProgramma is responsible for reading all the washingprogram settings from a textfile.
	* \Param file is the textfile with the needed settings
	*/
	void loadWasProgramma(string file){
		//waterValve->setWaterValve(true);
		ifstream myfile (file);
		if(myfile.is_open()){
			while(getline(myfile,line)){
				
			}
			myfile.close();
		}
	}
	
	/** \brief fillDrum checks if the drums waterlevel is lower then the max water level. If it is it will enable the waterValve.
	*/
	void fillDrum(){
		if(waterLevel->getLevel() < max_water_level && !afpompen){
			waterValve->setWaterValve(true);
		}else{
			waterValve->setWaterValve(false);
			doneFilling = true;
		}	
	}

	/** \brief EmptyDrum empties the Drum by enabling the waterPump.
	*/
	void emptyDrum(){
		if(waterLevel->getLevel() > 0){
			pump->setPump(true);
		}else{
			pump->setPump(false);
			donePumping = true;
		}
	}
	
	/** \brief checks if the current temperature is lower then the max temperature. if it is, it will enable the heating unit
	*/
	void heat(){
		if(tempSensor->getTemp() < max_temp){
			heatingUnit->setHeatingUnit(true);
		}else{
			doneHeating = true;
			heatingUnit->setHeatingUnit(false);
		}
	}
	
	/** \brief enables the soapDispenser element
	* \Param status if this parameter is false, the soap dispenser will be disabled. if true, it will be enabled.
	*/
	void setSoap(bool status){
		soapDispenser->setSoapDispenser(status);
	}
	
	/** \brief enables the motor element
	* \Param status if this parameter is false, rotate counterclockwise. if true, rotate clockwise.
	*/
	void rotate(bool status){

		if(status){
			trommel->setRPM(1600, false);
			sleep(3000 MS);
			trommel->setRPM(1600, true);	
		}else{
			trommel->setRPM(0,false);
			doneRotating = true;
		}
		
	}

	/** \brief Initialises the washing program by reading the commands queue and calling the loadWasprogramam function with the queue data.
	*/

	void init(){
		filename = commands->front();
		cout << "MESSAGE FROM WASPROGRAMAMTASK :" << filename << "\n";
		commands->pop(); 
		max_temp = atoi(commands->front().c_str());
		cout << "MESSAGE FROM WASPROGRAMAMTASK :" << max_temp << "\n";
		commands->pop(); 
		delay = atoi(commands->front().c_str());
		cout << "MESSAGE FROM WASPROGRAMAMTASK :" << delay << "\n";
		commands->pop();
		
		inProgress = true;
		b.setReadyForSending(true);

	}


	/** \brief Resets all bools and variables in order to be able to start another washingprogram
	*/
	void reset(){
		inProgress = false;
		b.setReadyForSending(false);
		doneFilling = false;
		doneHeating = false;
		doneRotating = false;
		donePumping = false;
		doorLocked = false;
		max_temp = 0;
		doorLock.setDoorLock(false);
	}

	void main(void){
		for (;;) {
			if(!inProgress){
				if(b.CheckForMessages()){
					commands = b.getMessages();
					init();
				}
				else{
					sleep(500 MS);
				}
			}
			else{
				if(delay != 0){
					sleep((delay * 1000) MS);
				}
				if(!doorLocked){
					if(doorLock.getDoorLock() == Status::eCLOSED){
						doorLock.setDoorLock(true);
						doorLocked = true;
					}
				}
				if(!doneFilling && doorLocked){
					fillDrum();
					setSoap(true);
				}
				if(doneFilling && !doneHeating){
					heat();	
					setSoap(false);
				}
				
				if(doneHeating && !doneRotating){
					rotate(true);
					sleep(10000 MS);
					rotate(false);
				}

				if(doneRotating){
					emptyDrum();
				}
				
				if(donePumping){
					reset();
				}
			}
			sleep(500 MS);
		}	
	}
};

#endif

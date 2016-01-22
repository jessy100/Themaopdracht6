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
class WasprogrammaController : public RTOS::task{
	public:
	WasprogrammaController(short prio, const char* name, UartCommunicator* uart, Broadcaster & b, WaterValve* waterValve, 
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
		bool done = false, afpompen = false, inProgress = false, doneFilling = false, doneHeating = false,doneRotating = false, donePumping = false, doorLocked = false, soapAdded = false, doneCentrifuge = false;
		int maxSpoelBeurt = 6, spoelBeurt = 0, wasRpm = 1400;
	
	private:
		Broadcaster & b;
		queue<string> *commands	;

	/** \brief loadWasProgramma is responsible for reading all the washingprogram settings from a textfile.
	* \Param file is the textfile with the needed settings
	*/
	void loadWasProgramma(string file){
		ifstream myfile ((file + ".txt"));
		string line;
		if(myfile.is_open()){
			getline(myfile,line);
			wasRpm = atoi(line.c_str());
			cout << "RPM voor centrifugeren : " << wasRpm << endl;
			getline(myfile,line);
			maxSpoelBeurt = atoi(line.c_str());
			cout << "maxSpoelBeurt : " << maxSpoelBeurt << endl;
			
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
	* \Param rpm set the speed of rotation for the drum
	* \Param turnTime set the duration how long the drum rotates to each side
	*/
	void rotate(int rpm, int turnTime){
		trommel->setRPM(rpm, false);
		sleep(turnTime MS);
		trommel->setRPM(rpm, true);
		sleep(turnTime MS);
		
		trommel->setRPM(0,false);
		doneRotating = true;
	}
	
	void setSpoelen(){
		doneFilling = false;
		doneRotating = false;
		donePumping = false;
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
		
		loadWasProgramma(filename);
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
		soapAdded = false;
		doneCentrifuge = false;
		spoelBeurt = 0;
		max_temp = 0;
		doorLock->setDoorLock(false);
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
					if(doorLock->getDoorLock() == Status::eCLOSED){
						doorLock->setDoorLock(true);
						doorLocked = true;
					}
				}
				
				if(!doneFilling && doorLocked){
					fillDrum();
				}
				
				if(!soapAdded && doorLocked){
					setSoap(true);
					soapAdded = true;
				}
				
				if(soapAdded && doneFilling){
					setSoap(false);
				}
				
				if(doneFilling && !doneHeating){
					heat();
				}
				
				if(doneHeating && !doneRotating){
					rotate(25, 6000);
				}

				if(doneRotating){
					emptyDrum();
				}
				
				if(maxSpoelBeurt > spoelBeurt && donePumping){
					setSpoelen();
					++spoelBeurt;
				}
				
				if(!doneCentrifuge && maxSpoelBeurt == spoelBeurt){
					rotate(wasRpm, 10000);
				}
				
				if(doneCentrifuge){
					reset();
				}
			}
			sleep(500 MS);
		}	
	}
};

#endif

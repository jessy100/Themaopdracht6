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

class WasProgrammaTask : public RTOS::task{
	public:
	WasProgrammaTask(short prio, const char* name, UartCommunicator* uart, Broadcaster & b, WaterValve* waterValve, 
					WaterLevel* waterLevel, TempSensor* tempSensor, HeatingUnit* heatingUnit, Trommel* trommel, 
					SoapDispenser* soapDispenser, Pump* pump):
		RTOS::task(prio, name),
		uartCom(uart),
		b(b),
		waterValve(waterValve),
		waterLevel(waterLevel),
		tempSensor(tempSensor),
		heatingUnit(heatingUnit),
		trommel(trommel),
		soapDispenser(soapDispenser),
		pump(pump)
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

		//test
		bool done = false,afpompen = false, inProgress = false, doneFilling = false, doneHeating = false,doneRotating = false, donePumping = false;
	
	private:
		Broadcaster & b;
		queue<string> *commands	;
	/* TODO
	* Get temperature, delay and wasprogramma type from queue
	* make sensor calls
	* open file with wasmachine data
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
		
	void fillDrum(){
		if(waterLevel->getLevel() < max_water_level && !afpompen){
			waterValve->setWaterValve(true);
		}else{
			waterValve->setWaterValve(false);
			doneFilling = true;
		}	
	}

	void emptyDrum(){
		if(waterLevel->getLevel() > 0){
			pump->setPump(true);
		}else{
			pump->setPump(false);
			donePumping = true;
		}
	}
	
	void heat(){
		if(tempSensor->getTemp() < max_temp){
			heatingUnit->setHeatingUnit(true);
		}else{
			doneHeating = true;
			heatingUnit->setHeatingUnit(false);
		}
	}
	
	void setSoap(bool status){
		soapDispenser->setSoapDispenser(status);
	}
	
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


	//properly reset all values in order for the next washingprogram to start correctly;
	void reset(){
		inProgress = false;
		b.setReadyForSending(false);
		doneFilling = false;
		doneHeating = false;
		doneRotating = false;
		donePumping = false;
		max_temp = 0;
	}

	void main(void){
		for (;;) {
			if(!b.CheckForMessages() && !inProgress){
				sleep(500 MS);
			}else if(!inProgress){
				commands = b.getMessages();
				init();
			}
			if(inProgress){
				if(delay != 0){
					sleep((delay * 1000) MS);
				}			
				if(!doneFilling){
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

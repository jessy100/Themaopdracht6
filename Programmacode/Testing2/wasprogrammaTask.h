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
	WasProgrammaTask(short prio, const char* name, UartCommunicator* uart, Broadcaster & b, WaterValve* waterValve):
		RTOS::task(prio, name),
		uartCom(uart),
		b(b),
		waterValve(waterValve)
		{}
		
	private:
		UartCommunicator* uartCom;
		string max_temp, delay = "0";
		string line, filename = "was_bont.txt";
		string voorwas, hoofdwas_toeren;
		WaterValve* waterValve;
	
	private:
		Broadcaster & b;
		queue<string> *commands	;
	/* TODO
	* Get temperature, delay and wasprogramma type from queue
	* make sensor calls
	* open file with wasmachine data
	*/
	void loadWasProgramma(string file){
		waterValve->setWaterValve(true);
			//read wasprogramma from file
			//file = "wasprogramma" + file;
			cout << "==========Reading file===========\n";
			ifstream myfile (file);
			if(myfile.is_open()){
				while(getline(myfile,line)){
					
				}
				myfile.close();
				


			}
			cout << "==========Done Reading file===========\n";
		}
		
	void setWaterValve(bool status){
		
		status = false;
		
	}
	
	void setHeatElement(bool status){

		status = false;
	
		
	}
	
	void addSoap(){
		//status = false;
		// add soap
	}
	
	void rotate(bool status){
		// true = start, false = stop;
	}

	void handleQueue(){
		filename = commands->front();
		cout << "MESSAGE FROM WASPROGRAMAMTASK :" << filename << "\n";
		commands->pop(); 
		max_temp = commands->front();
		cout << "MESSAGE FROM WASPROGRAMAMTASK :" << max_temp << "\n";
		commands->pop(); 
		delay = commands->front();
		cout << "MESSAGE FROM WASPROGRAMAMTASK :" << delay << "\n";
		commands->pop(); 
	}
	void main(void){
		

		loadWasProgramma(filename);
		for (;;) {
			if(b.CheckForMessages() == true){
				//ff
				commands = b.getMessages();
				handleQueue();
			}
			

			//waterLevel = sensor water level
			//if(waterLevel != max_water_level){
			//	setWaterValve(true);
			//}
			
			//temp = sensor temp
			//if(max_temp != temp){
			//	setHeatElement(true);
			//}
			sleep(1000 MS);
		}	
	}
	
		
		
		
		
		
};

#endif

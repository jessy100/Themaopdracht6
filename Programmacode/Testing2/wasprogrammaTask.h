#include <iostream>
#include <thread>
#include "uartCommunicator.h"

using namespace std;

class WasprogrammaTask : public RTOS::task{
	public:
	WasprogrammaTask(short prio, const char* name, UartCommunicator* uart):
		RTOS::task(prio, name),
		uartCom(uart)
		{}
		
	private:
		UartCommunicator* uartCom;
		int max_temp, delay;
	
	private:
	
	/* TODO
	* Get temperature, delay and wasprogramma type from queue
	* make sensor calls
	* open file with wasmachine data
	*/
		void main(void){
			
			//loadWasProgramma();
			for (;;) {
				
				//waterLevel = sensor water level
				if(waterLevel != max_water_level){
					setWaterValve(true);
				}
				
				//temp = sensor temp
				if(max_temp != temp){
					setHeatElement(true);
				}
			}	
		}
		
		void loadWasProgramma(){
			//read wasprogramma from file
		}
		
		void setWaterValve(bool status){
			// if status == true open water valve
			
			//if status == false close water valve;
			
		}
		
		void setHeatElement(bool status){
			// if status == true enable heat element
			
			//if status == false disable heat element;
			
		}
		
		void addSoap(){
			// add soap
		}
		
		void rotate(bool true){
			// true = start, false = stop;
		}
		
		
		
		
};

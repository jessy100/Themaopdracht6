#ifndef _UARTTASK_H
#define _UARTTASK_H

#include <iostream>
#include <thread>
#include "uartCommunicator.h"

using namespace std;

class Uart_task : public RTOS::task{
public:
	Uart_task(short prio, const char * name, UartCommunicator* uartCom):
		RTOS::task(prio, name),
		uartCom(uartCom)
		{}

private:
	LibSerial serial;
	UartCommunicator* uartCom;
	int reply = 0;
	
private:
	void writeSerial(){
		Requests req = uartCom->readCommand();
		serial.writeChar(req.first);
		serial.writeChar(req.second);
		cout << "klaar met schrijven" << endl;
		while(serial.peek() < 2){
			sleep(1000);
		}
		serial.read(&reply,2);
		uartCom->writeReply(reply);
	}
	
   void main (void) {
	   int err = serial.open("/dev/ttyAMA0", 9600);
	   if(err < 1){
			if(err == -1){
				std::cout << "error while opening device\n";
			}
			else if(err == -2){
				std::cout << "unable to set non-blocking mode\n";
			}
			else if(err == -3){
				std::cout << "speed (bauds) not recognized\n";
			}
			else if(err == -4){
				std::cout << "unable to set baud rate\n";
			}
			else if(err == -5){
				std::cout << "unable to set flags\n";
			}	
		}
		else{
			cout << "schrijft nu" << endl;
			serial.writeChar(MACHINE_REQ);
			serial.writeChar(START_CMD);
			cout << "klaar met schrijven" << endl;
			while(serial.peek() < 2){
				sleep(6000);
			}
			serial.read(&reply,2);
			cout << hex << reply << endl;
		}
		for (;;) {
			writeSerial();
		}
	}
};

#endif
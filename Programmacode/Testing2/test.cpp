/******************************************************************************\
 FILE:   main.cpp

 This file contains several programs to test the RTOS package.

\******************************************************************************/

#include "pRTOS.h"
#include "libserial.h"
#include "Protocol.h"
#include <iostream>

using namespace std;

class uart_task : public RTOS::task {
public:
   uart_task(short prio, const char * name):
			RTOS::task(prio, name){}

private:
	LibSerial serial;
	int n;
	char reply[2] {0,0};
	
private:
	void writeSerial(){
		serial.writeChar(DOOR_LOCK_REQ);
		serial.writeChar(LOCK_CMD);
		cout << "klaar met schrijven" << endl;
		serial.readChar(&reply[0]);
		sleep(200);
		serial.readChar(&reply[1]);
		cout << hex << reply[0] << ' ' << reply[1] << endl;
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
			cout << "minder dan 2 in recbuffer" << endl;
			sleep(100);
		}
		serial.read(reply,2);
		cout << reply[0] << ' ' << reply[1] << endl;
	}
        for (;;) {
            if (n % 10000 == 0) {
                cout << "i: " << (n / 10000) << endl;
				writeSerial();
            }
            n += 1;
            sleep(500 US);
        }
   }
};

void writeTest (void) {
	cout << "runt";
   uart_task uart(20, "UART");

   RTOS::run();
}

/******************************************************************************\
 main routine
\******************************************************************************/
int main( void )
{
   writeTest();
   return 0;
}

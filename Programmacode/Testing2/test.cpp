/******************************************************************************\
 FILE:   main.cpp

 This file contains several programs to test the RTOS package.

\******************************************************************************/

#include "pRTOS.h"
#include "libserial.h"
#include "Protocol.h"
#include <iostream>
#include <thread>
#include "PracticalSocket.h"
#include "websocket.h"
#include "broadcaster.h"

using namespace std;

class uart_task : public RTOS::task {
public:
   uart_task(short prio, const char * name):
		RTOS::task(prio, name),
		commandCh(this, "channel")
		{
			receivePool.write(false);
		}	
			
	void writeCommand(uint8_t command){
		commandCh.write(command);
	}
	
	bool waitReply(){
		bool b = receivePool.read();
		if(b){
			receivePool.write(false);
		}
		return b;
	}

private:
	LibSerial serial;
	RTOS::channel <uint8_t, 10> commandCh;
	RTOS::pool <bool> receivePool;
	int n;
	int reply = 0;
	
private:
	void writeSerial(){
		receivePool.write(false);
		wait(commandCh);
		serial.writeChar(commandCh.read());
		wait(commandCh);
		serial.writeChar(commandCh.read());
		cout << "klaar met schrijven" << endl;
		while(serial.peek() < 1){
			sleep(100);
		}
		serial.read(&reply,2);
		cout << hex << reply << endl;
		receivePool.write(true);
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
            if (n % 10000 == 0) {
                cout << "i: " << (n / 10000) << endl;
				writeSerial();
            }
            n += 1;
            sleep(500 US);
        }
   }
};

class Sensor_task : public RTOS::task{
	public:
	Sensor_task(short prio, const char* name, uart_task* uart):
		RTOS::task(prio, name),
		uart(uart)
		{}
		
	private:
		uart_task* uart;
	
	private:
		void main(void){
			for (;;) {
				cout << "schrijf tempreq in queue" << endl;
				uart->writeCommand(TEMPERATURE_REQ);
				uart->writeCommand(STATUS_CMD);
				sleep(2000 * 1000);
			}	
		}
};


Broadcaster b;

class MyListener : public WebSocketListener{
public:
	void onTextMessage( const string& s){
		b.broadcast(s);
		cout << "Recieved: " << s << endl;
		
	}

	void onClose(WebSocket* ws){
		b.remove(ws);
		delete ws;
	}
};

MyListener ml;

class Websocket_task : public thread{
public: 
	void run() {
		try {
			TCPServerSocket servSock(8080);
			cout << "server running: " << servSock.getLocalAddress().getAddress() << endl;
			for (;;) {
				cout << "before accepting websocket" << endl;
				TCPSocket *sock = servSock.accept();
				WebSocket* ws = new WebSocket(sock);
				ws->setListener(&	ml);
				b.add(ws);
				cout << "after accepting websocket" << endl;
			}
		} 
		catch (SocketException &e) {
			cerr << e.what() << endl;           // Report errors to the console
		}
	}
};
	
			
void writeTest () {
	cout << "runt";
	uart_task uart(20, "UART");
	Sensor_task sensor(30, "Sensor", &uart);
	Websocket_task ws;
	thread x(&Websocket_task::run,&ws);
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

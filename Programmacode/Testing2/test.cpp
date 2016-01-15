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

struct Requests{
	uint8_t first, second;
};

class Sensors{
public:
	Sensors(UartCommunicator* uart):
		uartCom(uart)
		{}
	
protected:
	UartCommunicator* uartCom
};

class TempSensor : public Sensors{
public:
	TempSensor(){}
	
	getTemperature(){
		Requests req = {TEMPERATURE_REQ, STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "Temp = " << uartCom->readReply() << endl;
	}
};

class WaterLevel : public Sensors{
public:
	WaterLevel(){}
	
	getWaterLevel(){
		Requests req = {WATER_LEVEL_REQ, STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "WaterLevel = " << uartCom->readReply() << endl;
	}
}

class WMStatus : public Sensors{
public:
	WMStatus(){}
	
	getWMStatus(){
		Requests req = {MACHINE_REQ, STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "WMStatus = " << uartCom->readReply() << endl;
	}
}

class UartCommunicator {
public:
	UartCommunicator():
		mailboxIn("mailbox"),
		mailboxOut("mailbox")
	{}
	
	void writeCommand(Requests req){
		mailboxIn.write(req);
	}
	
	Requests readCommand(){ 
		return mailboxIn.read();
	}
	
	void writeReply(int n){
		mailboxOut.write(n);
	}
	
	int readReply(){
		return mailboxOut.read();
	}
	
private:
	RTOS::mailbox<Requests> mailboxIn;
	RTOS::mailbox<int> mailboxOut;
};

class uart_task : public RTOS::task{
public:
	uart_task(short prio, const char * name, UartCommunicator* uartCom):
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

class Sensor_task : public RTOS::task{
	public:
	Sensor_task(short prio, const char* name, UartCommunicator* uart):
		RTOS::task(prio, name),
		uartCom(uart)
		{}
		
	private:
		UartCommunicator* uartCom;
	
	private:
		void main(void){
			for (;;) {
				cout << "schrijf tempreq in queue" << endl;
				Requests req = { TEMPERATURE_REQ, STATUS_CMD};
				uartCom->writeCommand(req);
				sleep(2000 * 1000);
				cout << uartCom->readReply() << endl;
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

/******************************************************************************\
 main routine
\******************************************************************************/
int main( void )
{
	cout << "runt";
	UartCommunicator uartCom;
	uart_task uart(20, "UART", &uartCom);
	Sensor_task sensor(30, "Sensor", &uartCom);
	Websocket_task ws;
	thread x(&Websocket_task::run,&ws);
	RTOS::run();
	return 0;
}

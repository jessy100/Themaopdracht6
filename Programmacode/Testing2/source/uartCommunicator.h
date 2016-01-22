#ifndef _UARTCOMMUNICATOR_H
#define _UARTCOMMUNICATOR_H

#include <iostream>
#include <thread>
#include "pRTOS.h"

using namespace std;

struct Requests{
	uint8_t first, second;
};

class UartCommunicator {
public:
	UartCommunicator():
		mailboxIn("mailbox"),
		mailboxOut("mailbox")
		//mailMutex("Mutex")
	{}
	
	void writeCommand(Requests req){
		mailMutex.wait();
			mailboxIn.write(req);
		mailMutex.signal();
	}
//mutex op writecommand.	
	Requests readCommand(){ 
		return mailboxIn.read();
	}
	
	void writeReply(int n){
		mailboxOut.write(n);
	}
	
	short readReply(){
		short ret = mailboxOut.read();
		return ret;
	}
	
private:
	//bool  b = false;
	RTOS::mutex mailMutex;
	RTOS::mailbox<Requests> mailboxIn;
	RTOS::mailbox<short> mailboxOut;
};

#endif
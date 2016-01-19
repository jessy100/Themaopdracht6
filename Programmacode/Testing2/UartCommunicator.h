#ifndef _UARTCOMMUNICATOR_H
#define _UARTCOMMUNICATOR_H

#include <iostream>
#include <thread>

using namespace std;

#pragma once
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
		//mailMutex.wait();
		//if(b == false){
			//b = true;
			mailboxIn.write(req);
		//}
		//mailMutex.signal();
		//return b;
	}
	
	Requests readCommand(){ 
		return mailboxIn.read();
	}
	
	void writeReply(int n){
		mailboxOut.write(n);
	}
	
	short readReply(){
		//mailMutex.wait();
		//b = false;
		short ret = mailboxOut.read();
		//mailMutex.signal();
		return ret;
	}
	
private:
	//bool  b = false;
	//RTOS::mutex mailMutex;
	RTOS::mailbox<Requests> mailboxIn;
	RTOS::mailbox<short> mailboxOut;
};

#endif
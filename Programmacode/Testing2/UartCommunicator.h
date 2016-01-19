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
	
	short readReply(){
		return mailboxOut.read();
	}
	
private:
	RTOS::mailbox<Requests> mailboxIn;
	RTOS::mailbox<short> mailboxOut;
};

#endif
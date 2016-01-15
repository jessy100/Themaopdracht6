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
	
	int readReply(){
		return mailboxOut.read();
	}
	
private:
	RTOS::mailbox<Requests> mailboxIn;
	RTOS::mailbox<int> mailboxOut;
};
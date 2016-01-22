/*!
*   @author     Jessy Visch, Zehna van den berg, Koen de Groot, Joost Wagensveld
*   @project    Themaopdracht 6, Wasmachine
*   @brief      Contains all logic to communicate with the uart
*/

#ifndef _UARTCOMMUNICATOR_H
#define _UARTCOMMUNICATOR_H

#include <iostream>
#include <thread>
#include "pRTOS.h"

using namespace std;

struct Requests{
	uint8_t first, second;
};

/** \class UartCommunicator
	* \brief is responsible for the communication between The RTOS and  the LPC's uart
*/
class UartCommunicator {
public:
	UartCommunicator():
		mailboxIn("mailbox"),
		mailboxOut("mailbox")
	{}
	
	/** \brief writeCommand is responsible for writing all commands to the mailbox
	* \Param Requests req is the request it will send.
	*/
	void writeCommand(Requests req){
		mailMutex.wait();
			mailboxIn.write(req);
		mailMutex.signal();
	}

	/** \brief readCommand is responsible for reading all commands in the mailbox.
	*/
	Requests readCommand(){ 
		return mailboxIn.read();
	}
	
	/** \brief writeReply is responsible for writing the reply mailbox
	*/
	void writeReply(int n){
		mailboxOut.write(n);
	}

	/** \brief readReply is responsible for reading the reply mailbox
	*/	
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
#include "libserial.h"
#include "pRTOS.h"

struct Request{
	char type;
	char command;
}
	
int main(){
	Libserial serial;
	int err = serial.open(/dev/ttyUSB4, 9600) //9600 is bauds klopt, eerste is device, kan fout zijn.
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
		Request req;
		req.type = 0x01;
		req.command = 0x01;
		
		serial.writeChar(req.type);
		serial.writeChar(req.command);
		
		while(serial.peek() < 2){
			wait(10);
		}
		Request reply;
		if(!serial.readChar(reply.type)){std::cout << "error while reading char\n";}
		if(!serial.readChar(reply.command)){std::cout << "error while reading char\n";}
		std::cout << reply.type << ' ' << reply.command << '\n';
	}
	return 0;
}

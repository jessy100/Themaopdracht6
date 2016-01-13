#include "pRTOS.h"
#include "protocol.h"

struct Request{
	char type;
	char command;
}

class uart_task : public RTOS::task{
	public:
		uart_task(const char * name):
			task(name, 10){}
		
	private:
		void main(){			
			for(;;){
				std::cout << "running uart task\n";
				sleep(100);
			}
	}	
};

class other_task : public RTOS::task{
	public:
		uart_task(const char * name):
			task(name, 9){}
		
	private:
		void main(){			
			for(;;){
				std::cout << "running other task\n";
				sleep(100);
			}
	}	
};
	
int main(){
	uart_task("UART");
	other_task("other");
	RTOS::run();	
	return 0;
}

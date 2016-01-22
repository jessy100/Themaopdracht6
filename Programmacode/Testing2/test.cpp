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

#include "uartCommunicator.h"
#include "sensorTask.h"
#include "websocketTask.h"
#include "uartTask.h"
#include "Sensor.h"
#include "UpdatingSensor.h"
 #include "broadcaster.h"
/******************************************************************************\
 main routine
\******************************************************************************/
int main( void )
{
	
	
	cout << "runt";
	UartCommunicator uartCom;
	TempSensor tempSensor(&uartCom);
	WaterLevel waterLevel(&uartCom);
	WMStatusSensor wmStatusSensor(&uartCom);
	Trommel trommel(&uartCom);
	DoorLock doorLock(&uartCom);
	WaterValve waterValve(&uartCom);
	HeatingUnit heatingUnit(&uartCom);
	SoapDispenser soapDispenser(&uartCom);
	Pump pump(&uartCom);
	Uart_task uart(20, "UART", &uartCom);
	
	
	Broadcaster b(&tempSensor, &waterLevel);
	Sensor_task sensor(30, "SensorHandler", &b);
	

	sensor.addSensor(&tempSensor);
	sensor.addSensor(&waterLevel);
	sensor.addSensor(&wmStatusSensor);
	sensor.addSensor(&trommel);
	
	Websocket_task ws(b);
	WasProgrammaTask wasprogramma(40, "Wasprogramma", &uartCom, b, &waterValve, &waterLevel, &tempSensor, &heatingUnit, &trommel,&soapDispenser, &pump);
	thread x(&Websocket_task::run,&ws);
	RTOS::run();
	return 0;
}

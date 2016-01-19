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
#include "uartCommunicator.h"
#include "sensorTask.h"
#include "websocketTask.h"
#include "uartTask.h"
#include "Sensor.h"
#include "UpdatingSensor.h"
/******************************************************************************\
 main routine
\******************************************************************************/
int main( void )
{
	Broadcaster b;
	
	cout << "runt";
	UartCommunicator uartCom;
	TempSensor tempSensor(&uartCom);
	WaterLevel waterLevel(&uartCom);
	WMStatusSensor wmStatusSensor(&uartCom);
	Trommel trommel(&uartCom);
	DoorLock doorLock(&uartCom);
	WaterValve waterValve(&uartCom);
	/*
	Pump pump(&uartCom);
	SoapDispenser(&uartCom);
	HeatingUnit(&uartCom);
	*/
	Uart_task uart(20, "UART", &uartCom);
	Sensor_task sensor(30, "SensorHandler");
	
	sensor.addSensor(&tempSensor);
	sensor.addSensor(&waterLevel);
	sensor.addSensor(&wmStatusSensor);
	sensor.addSensor(&trommel);
	
	Websocket_task ws(b);
	WasProgrammaTask wasprogramma(40, "Wasprogramma", &uartCom, b, &waterValve);
	thread x(&Websocket_task::run,&ws);
	RTOS::run();
	return 0;
}

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
	cout << "runt";
	UartCommunicator uartCom;
	TempSensor tempSensor(&uartCom);
	WaterLevel waterLevel(&uartCom);
	Uart_task uart(20, "UART", &uartCom);
	
	Sensor_task sensor(30, "SensorHandler");
	sensor.addSensor(&tempSensor);
	sensor.addSensor(&waterLevel);	
	
	Websocket_task ws;
	thread x(&Websocket_task::run,&ws);
	RTOS::run();
	return 0;
}

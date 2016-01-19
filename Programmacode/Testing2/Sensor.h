#ifndef _SENSOR_H
#define _SENSOR_H

#include "uartCommunicator.h"
#include "UpdatingSensor.h"

enum class Status{eERROR, eOPENED, eCLOSED, eLOCKED, eON, eOFF};
enum class WMStatus{eERROR, eHALTED, eSTOPPED, eRUNNING, eIDLE};

Status replyToStatus(uint8_t status){
	switch(status){
		case 0x01: return Status::eOPENED; 	break;
		case 0x02: return Status::eCLOSED; 	break;
		case 0x04: return Status::eLOCKED; 	break;
		case 0x08: return Status::eON; 		break;
		case 0x10: return Status::eOFF;		break;
		default: return Status::eERROR;
	}
}

WMStatus replyToWMStatus(uint8_t status){
	switch(status){
		case 0x01: return WMStatus::eHALTED; 	break;
		case 0x02: return WMStatus::eIDLE; 		break;
		case 0x04: return WMStatus::eRUNNING;	break;
		case 0x08: return WMStatus::eSTOPPED; 	break;
		default: return WMStatus::eERROR;
	}
}

class WmHardware{
protected:
	WmHardware(UartCommunicator* uart):
		uartCom(uart){}
		
	short handleRequest(uint8_t request, uint8_t command){
		Requests req = {request, command};
		uartCom->writeCommand(req);
		return (uartCom->readReply() >> 8);
	}

	UartCommunicator* uartCom;
};

class Sensor : public WmHardware, public UpdatingSensor{
public:
	
protected:
	Sensor(UartCommunicator* uart):
		WmHardware(uart)
		{}
};

class TempSensor : public Sensor{
public:
	TempSensor(UartCommunicator* uart):
		Sensor(uart){}
	
	void update(){
		getTemperature();
	}
	
private:
	int getTemperature(){
		short t = handleRequest(TEMPERATURE_REQ, STATUS_CMD);
		cout << "Temp = " << t << endl;
		return t;
	}
};

class WaterLevel : public Sensor{
public:
	WaterLevel(UartCommunicator* uart):
		Sensor(uart){}
	
	void update(){
		getWaterLevel();
	}
	
private:
	int getWaterLevel(){
		short level = handleRequest(WATER_LEVEL_REQ, STATUS_CMD);
		cout << "WaterLevel = " << level << endl;
		return level;
	}
};

class WMStatusSensor : public Sensor{
public:
	WMStatusSensor(UartCommunicator* uart):
		Sensor(uart){}
	
	void update(){
		getWMStatus();
	}
	
private:
	WMStatus getWMStatus(){
		short reply = handleRequest(MACHINE_REQ, STATUS_CMD);
		cout << "WMStatus = " << reply << endl;
		return replyToWMStatus((reply));
	}
};
/*
class Trommel : public Sensor{
public:
	Trommel(UartCommunicator& uart):
		Sensor{uart}{}
		
	void update(){
		getRPM();
	}
	
	void setRPM(unsigned int i, bool clockwise){
		if(i > 1600){ i = 1600;}
		i = i / 25;
		uint8_t rpm = ((clockwise) ? i : i | 0x80);
		
		short reply = handleRequest(SET_RPM_REQ, rpm);
		cout << "RPM = " << reply << endl;
	}
	
	short getRPM(){
		short reply = handleRequest(GET_RPM_REQ, STATUS_CMD);
		cout << "RPMSet = " << reply << endl;
		return reply;
	}
};

class DoorLock : public WmHardware{
public:
	DoorLock(UartCommunicator& uart):
		WmHardware{uart}{}
	
	Status getDoorLock(){
		short reply = handleRequest(DOOR_LOCK_REQ, STATUS_CMD);
		cout << "DoorLock = " << reply << endl;
		return replyToStatus(reply);
	}
	
	void setDoorLock(bool lock){
		short reply = handleRequest(DOOR_LOCK_REQ, ((lock) ? LOCK_CMD : UNLOCK_CMD));
		cout << "DoorLockSet = " << reply << endl;
	}
};

class WaterValve : public WmHardware{
public:
	WaterValve(UartCommunicator& uart):
		WmHardware{uart}{}
		
	Status getWaterValve(){
		short reply = handleRequest(WATER_VALVE_REQ, STATUS_CMD);
		cout << "WaterValve = " << reply << endl;
		return replyToStatus(reply);
	}
	
	void setWaterValve(bool open){
		short reply = handleRequest(WATER_VALVE_REQ, ((open) ? OPEN_CMD : CLOSE_CMD));
		cout << "WaterValveSet = " << reply << endl;
	}
};

class Pump : public WmHardware{
public:
	Pump(UartCommunicator& uart):
		WmHardware{uart}{}
		
	Status getPump(){
		short reply = handleRequest(PUMP_REQ, STATUS_CMD);
		cout << "PumpStatus = " << reply << endl;
		return replyToStatus(reply);
	}
	
	void setPump(bool on){
		short reply = handleRequest(PUMP_REQ, ((on) ? ON_CMD : OFF_CMD));
		cout << "PumpStatusSet = " << reply << endl;
	}
};

class SoapDispenser : public WmHardware{
public:
	SoapDispenser(UartCommunicator& uart):
		WmHardware{uart}{}
	
	Status getSoapDispenser(){
		short reply = handleRequest(SOAP_DISPENSER_REQ, STATUS_CMD);
		cout << "SoapDispenser = " << reply << endl;
		return replyToStatus(reply);
	}
	
	void setSoapDispenser(bool open){
		short reply = handleRequest(SOAP_DISPENSER_REQ, ((open) ? OPEN_CMD : CLOSE_CMD));
		cout << "SoapDispenserSet = " << reply << endl;
	}	
};

class HeatingUnit : public WmHardware{
public:
	HeatingUnit(UartCommunicator& uart):
		WmHardware{uart}{}
	
	Status getHeatingUnit(){
		short reply = handleRequest(HEATING_UNIT_REQ , STATUS_CMD);
		cout << "HeatingUnit = " << reply << endl;
		return replyToStatus(reply);
	}
	
	void setHeatingUnit(bool on){
		short reply = handleRequest(HEATING_UNIT_REQ , ((on) ? OPEN_CMD : CLOSE_CMD));
		cout << "HeatingUnitSet = " << reply << endl;
	}	
};
*/

#endif















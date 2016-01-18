#include "uartCommunicator"

class WmHardware{
protected:
	WmHardware(UartCommunicator* uart):
		uartCom(uart){}

	UartCommunicator* uartCom;
};

class Sensor : public WmHardware{
public:
	virtual void update(){
	}
	
protected:
	Sensor(UartCommunicator* uart):
		WmHardware{uart}
		{}
};

class TempSensor : public Sensor{
public:
	TempSensor(UartCommunicator* uart){}
	
	void update(){
		getTemperature();
	}
	
private:
	void getTemperature(){
		Requests req = {TEMPERATURE_REQ, STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "Temp = " << uartCom->readReply() << endl;
	}
};

class WaterLevel : public Sensor{
public:
	WaterLevel(UartCommunicator* uart){}
	
	void update(){
		getWaterLevel();
	}
	
private:
	void getWaterLevel(){
		Requests req = {WATER_LEVEL_REQ, STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "WaterLevel = " << uartCom->readReply() << endl;
	}
};

class WMStatus : public Sensor{
public:
	WMStatus(UartCommunicator* uart){}
	
	void update(){
		getWMStatus();
	}
	
private:
	void getWMStatus(){
		Requests req = {MACHINE_REQ, STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "WMStatus = " << uartCom->readReply() << endl;
	}
};

class Trommel : public Sensor{
public:
	Trommel(UartCommunicator* uart):
		Sensor{uart}{}
		
	void update(){
		getRPM();
	}
	
	void setRPM(unsigned int i, bool clockwise){
		if(i > 1600){ i = 1600;}
		i = i / 25;
		uint8_t rpm = ((clockwise) ? i : i | 0x80);
		
		Requests req = {SET_RPM_REQ, rpm};
		uartCom->writeCommand(req);
		cout << "RPM = " << uartCom->readReply() << endl;
	}
	
	void getRPM(){
		Requests req = {GET_RPM_REQ, STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "RPMSet = " << uartCom->readReply() << endl;
	}
};

class DoorLock : public WmHardware{
public:
	DoorLock(UartCommunicator* uart):
		WmHardware{uart}{}
	
	void getDoorLock(){
		Requests req = {DOOR_LOCK_REQ, STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "DoorLock = " << uartCom->readReply() << endl;
	}
	
	void setDoorLock(bool lock){
		Requests req = {DOOR_LOCK_REQ, ((lock) ? LOCK_CMD : UNLOCK_CMD)};
		uartCom->writeCommand(req);
		cout << "DoorLockSet = " << uartCom->readReply() << endl;
	}
};

class WaterValve : public WmHardware{
public:
	WaterValve(UartCommunicator* uart):
		WmHardware{uart}{}
		
	void getWaterValve(){
		Requests req = {WATER_VALVE_REQ, STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "WaterValve = " << uartCom->readReply() << endl;
	}
	
	void setWaterValve(bool open){
		Requests req = {WATER_VALVE_REQ, ((open) ? OPEN_CMD : CLOSE_CMD)};
		uartCom->writeCommand(req);
		cout << "WaterValveSet = " << uartCom->readReply() << endl;
	}
};

class Pump : public WmHardware{
public:
	Pump(UartCommunicator* uart):
		WmHardware{uart}{}
		
	void getPump(){
		Requests req = {PUMP_REQ, STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "PumpStatus = " << uartCom->readReply() << endl;
	}
	
	void setPump(bool on){
		Requests req = {PUMP_REQ, ((on) ? ON_CMD : OFF_CMD)};
		uartCom->writeCommand(req);
		cout << "PumpStatusSet = " << uartCom->readReply() << endl;
	}
};

class SoapDispenser : public WmHardware{
public:
	SoapDispenser(UartCommunicator* uart):
		WmHardware{uart}{}
	
	void getSoapDispenser(){
		Requests req = {SOAP_DISPENSER_REQ, STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "SoapDispenser = " << uartCom->readReply() << endl;
	}
	
	void setSoapDispenser(bool open){
		Requests req = {SOAP_DISPENSER_REQ, ((open) ? OPEN_CMD : CLOSE_CMD)};
		uartCom->writeCommand(req);
		cout << "SoapDispenserSet = " << uartCom->readReply() << endl;
	}	
};

class HeatingUnit : public WmHardware{
public:
	HeatingUnit(UartCommunicator* uart):
		WmHardware{uart}{}
	
	void getHeatingUnit(){
		Requests req = {HEATING_UNIT_REQ , STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "SoapDispenser = " << uartCom->readReply() << endl;
	}
	
	void setHeatingUnit(bool on){
		Requests req = {HEATING_UNIT_REQ , ((on) ? OPEN_CMD : CLOSE_CMD)};
		uartCom->writeCommand(req);
		cout << "SoapDispenserSet = " << uartCom->readReply() << endl;
	}	
};















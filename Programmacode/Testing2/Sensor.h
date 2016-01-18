#include "uartCommunicator"

class Sensor{
public:
	Sensor(UartCommunicator* uart):
		uartCom{uart}
		{}
		
	void update(){
	}
	
protected:
	UartCommunicator* uartCom
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
	
	void setRPM(int i){
		bool reverse = false;
		if(i > 1600){ i = 1600;}
		if(i < -1600){ i = -1600;}
		if(i < 0){reverse = true;}
		i = i / 25;
		uint8_t rpm = ((reverse) ? i | 0x80 : i);
		
		Requests req = {SET_RPM_REQ, rpm};
		uartCom->writeCommand(req);
		cout << "WMStatus = " << uartCom->readReply() << endl;
	}
	
	void getRPM(){
		Requests req = {GET_RPM_REQ, STATUS_CMD};
		uartCom->writeCommand(req);
		cout << "WMStatus = " << uartCom->readReply() << endl;
	}
};


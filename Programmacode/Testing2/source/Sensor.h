/*!
*   @author     Jessy Visch, Zehna van den berg, Koen de Groot, Joost Wagensveld
*   @project    Themaopdracht 6, Wasmachine
*   @brief      All sensors and their functions
*/


#ifndef _SENSOR_H
#define _SENSOR_H

#include "uartCommunicator.h"
#include "UpdatingSensor.h"
#include "Protocol.h"


enum class Status{eERROR, eOPENED, eCLOSED, eLOCKED, eON, eOFF};
enum class WMStatus{eERROR, eHALTED, eSTOPPED, eRUNNING, eIDLE};

/** \class Converter
	* \brief Converter is responsible for translating the codes that the uart returns.
	* \details Commands are translated from hexadecimal values to Struct options like OPENED and CLOSED
	*/
class Converter{
public:	 

	/** \brief replyToStatus is responsible for converting all sensor data's commands.
	* \details replyToStatus is responsible for converting all sensor data's commands.
	* \Param status status is the hexadecimal value the uart returns.
	*/
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

	/** \brief replyToWMStatus is responsible for converting all Wasmachine status commands.
	* \details replyToWMStatus is responsible for converting all wasmachine status commands.
	* \Param status status is the hexadecimal value the uart returns.
	*/
	WMStatus replyToWMStatus(uint8_t status){
		switch(status){
			case 0x01: return WMStatus::eHALTED; 	break;
			case 0x02: return WMStatus::eIDLE; 		break;
			case 0x04: return WMStatus::eRUNNING;	break;
			case 0x08: return WMStatus::eSTOPPED; 	break;
			default: return WMStatus::eERROR;
		}
	}
};


/** \class WmHardware
	* \brief Class wm hardware is an interface for all sensors.
	* \details This interface defines UartCommunicator for all sensors, this way we dont have to define it once per class
	*/
class WmHardware{
protected:
	WmHardware(UartCommunicator* uart):
		uartCom(uart)
		{}
		

	short handleRequest(uint8_t request, uint8_t command){
		Requests req = {request, command};
		uartCom->writeCommand(req);
		return (uartCom->readReply() >> 8);
	}

	UartCommunicator* uartCom;

 public: 
 	Converter converter;
};

/** \class Sensor
	* \brief Class Sensor is a superclass of all sensor which implements WmHardware and UpdatingSensor
	* \details By implementing these interfaces we can call Sensor->update in the sensorhandler instead of having to handles each
	* Sensor separately.
	*/
class Sensor : public WmHardware, public UpdatingSensor{
public:
	
protected:
	Sensor(UartCommunicator* uart):
		WmHardware(uart)
		{}
};


/** \class TempSensor
	* \brief This class handles the data of the Temperature Sensor and will update its values when called by SensorHandler. 
	*/
class TempSensor : public Sensor{
public:
	TempSensor(UartCommunicator* uart):
		Sensor(uart){}
	
	/** \brief Sets the latestTemp variable by calling function getTemperature()
	*/
	void update(){
		latestTemp = getTemperature();
	}

	/** \brief returns the latestTemp variable
	*/
	int getTemp(){
		return latestTemp;
	}
	
private:
	int latestTemp;

	/** \brief Reqeusts the current temperature by the washingmachine
	*/
	int getTemperature(){
		short t = handleRequest(TEMPERATURE_REQ, STATUS_CMD);
		return t;
	}
};


/** \class WaterLevel
	* \brief This class handles the data of the water level Sensor and will update its values when called by SensorHandler. 
	*/
class WaterLevel : public Sensor{
public:
	WaterLevel(UartCommunicator* uart):
		Sensor(uart){}
	
	/** \brief Sets the latestWaterLEvel variable by calling function getWaterLevel()
	*/
	void update(){
		latestWaterLevel = getWaterLevel();
	}
	
	/** \brief returns the latestWaterLevel variable
	*/
	int getLevel(){
		return latestWaterLevel;
	}

private:
	int latestWaterLevel;

	/** \brief Reqeusts the current water level by the washingmachine
	*/
	int getWaterLevel(){
		short level = handleRequest(WATER_LEVEL_REQ, STATUS_CMD);
		return level;
	}
};


/** \class WMStatusSensor
	* \brief this class handles the status of the washing machine (IE: Idle, running, halted) 
	*/
class WMStatusSensor : public Sensor{
public:
	WMStatusSensor(UartCommunicator* uart):
		Sensor(uart){}
	
	/** \brief updates the washingmachine status
	*/
	void update(){
		getWMStatus();
	}
	
private:
	/** \brief returns the washingmachine status
	*/
	WMStatus getWMStatus(){
		short reply = handleRequest(MACHINE_REQ, STATUS_CMD);
		return converter.replyToWMStatus((reply));
	}
};

/** \class WaterLevel
	* \brief This class handles the data of the Motor sensor and will update its values when called by SensorHandler. 
	*/
class Trommel : public Sensor{
public:
	Trommel(UartCommunicator* uart):
		Sensor(uart){}
	
	/** \brief calls the getRPM() function
	*/
	void update(){
		getRPM();
	}

	/** \brief Sets the RPM and direction of the motor
	* \param i i is the amount of RPM the motor should spin at
	* \param clockwise clockwise is the direction the motor spins (true = clockswise, false = counterclockwise) 
	*/
	void setRPM(unsigned int i, bool clockwise){
		if(i > 1600){ i = 1600;}
		i = i / 25;
		uint8_t rpm = ((clockwise) ? i : i | 0x80);
		
		short reply = handleRequest(SET_RPM_REQ, rpm);
	}
	
	/** \brief Returns the current RPM of the motor
	*/
	short getRPM(){
		short reply = handleRequest(GET_RPM_REQ, STATUS_CMD);
		//cout << "RPMSet = " << reply << endl;
		return reply;
	}
};

/** \class DoorLock
	* \brief This class handles the data of the water level Sensor and will update its values when called by SensorHandler. 
*/
class DoorLock : public WmHardware{
public:
	DoorLock(UartCommunicator* uart):
		WmHardware(uart){}
	
	/** \brief returns the current status of the doorlock
	*/
	Status getDoorLock(){
		short reply = handleRequest(DOOR_LOCK_REQ, STATUS_CMD);
		cout << "DoorLock = " << reply << endl;
		return converter.replyToStatus(reply);
	}
	
	/** \brief Sets the doorlock on or off depending on parameter lock
	* \param lock when this parameter is true the door will be locked, if its false the door will be unlocked 
	*/
	void setDoorLock(bool lock){
		short reply = handleRequest(DOOR_LOCK_REQ, ((lock) ? LOCK_CMD : UNLOCK_CMD));
		cout << "DoorLockSet = " << reply << endl;
	}
	
	
};

/** \class WaterValve
	* \brief This class handles the opening and the closing of the water valve and will update its values when called by SensorHandler. 
*/
class WaterValve : public WmHardware{
public:
	WaterValve(UartCommunicator* uart):
		WmHardware(uart){}
		
	/** \brief Returns the status of the watervalve
	*/
	Status getWaterValve(){
		short reply = handleRequest(WATER_VALVE_REQ, STATUS_CMD);
		//cout << "WaterValve = " << reply << endl;
		return converter.replyToStatus(reply);
	}
	
	/** \brief Sets the watervalve to open or close depending on parameter open
	* \param open when this parameter is true the watervalve will be openen, if its false the watervalve will be closed 
	*/
	void setWaterValve(bool open){
		short reply = handleRequest(WATER_VALVE_REQ, ((open) ? OPEN_CMD : CLOSE_CMD));
		//cout << "WaterValveSet = " << reply << endl;
	}
};

/** \class Pump
	* \brief This class is responsible for handling the pump sensor.
*/
class Pump : public WmHardware{
public:
	Pump(UartCommunicator* uart):
		WmHardware(uart){}
		
	/** \brief Returns the status of the Pump
	*/
	Status getPump(){
		short reply = handleRequest(PUMP_REQ, STATUS_CMD);
		//cout << "PumpStatus = " << reply << endl;
		return converter.replyToStatus(reply);
	}
	
	/** \brief Sets active depending on bool on parameter
	* \param on when this parameter is true the pump will be enabled, if its false the pump will be disabled
	*/
	void setPump(bool on){
		short reply = handleRequest(PUMP_REQ, ((on) ? ON_CMD : OFF_CMD));
		//cout << "PumpStatusSet = " << reply << endl;
	}
};

/** \class SoapDispenser
	* \brief This class is responsible for handling the SoapDispenser sensor.
*/
class SoapDispenser : public WmHardware{
public:
	SoapDispenser(UartCommunicator* uart):
		WmHardware(uart){}
	
	/** \brief Returns the status of the SoapDispenser
	*/
	Status getSoapDispenser(){
		short reply = handleRequest(SOAP_DISPENSER_REQ, STATUS_CMD);
		//cout << "SoapDispenser = " << reply << endl;
		return converter.replyToStatus(reply);
	}
	
	/** \brief Sets active depending on bool open parameter
	* \param open when this parameter is true the soapDispenser will be enabled, if its false the pump will be disabled
	*/
	void setSoapDispenser(bool open){
		short reply = handleRequest(SOAP_DISPENSER_REQ, ((open) ? OPEN_CMD : CLOSE_CMD));
		//cout << "SoapDispenserSet = " << reply << endl;
	}	
};

/** \class HeatingUnit
	* \brief This class is responsible for handling the HeatingUnit sensor.
*/
class HeatingUnit : public WmHardware{
public:
	HeatingUnit(UartCommunicator* uart):
		WmHardware(uart){}
	
	/** \brief Returns the status of the heatingUnit
	*/
	Status getHeatingUnit(){
		short reply = handleRequest(HEATING_UNIT_REQ , STATUS_CMD);
		//cout << "HeatingUnit = " << reply << endl;
		return converter.replyToStatus(reply);
	}
	
	/** \brief Sets active depending on bool on parameter
	* \param on when this parameter is true the heatingunit will be enabled, if its false the heatingunit will be disabled
	*/
	void setHeatingUnit(bool on){
		short reply = handleRequest(HEATING_UNIT_REQ , ((on) ? OPEN_CMD : CLOSE_CMD));
		//cout << "HeatingUnitSet = " << reply << endl;
	}	
};

#endif















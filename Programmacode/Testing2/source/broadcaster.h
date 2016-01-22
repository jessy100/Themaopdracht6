
// Jan Zuurbier september 2014
// De broadcaster is thread-safe gemaakt.
// Dit is nodig omdat verschillende threads hier
// mogelijk tegelijkertijd gebruik van maken.


#ifndef BROADCASTER_H
#define BROADCASTER_H

#include "websocket.h"
#include <list>
#include <mutex>
#include <queue>
#include <string>
#include "Sensor.h"

class Broadcaster
{
public:
	Broadcaster(TempSensor*,WaterLevel*);
	~Broadcaster();
	void add(WebSocket* ws);
	void remove(WebSocket* ws);
	void broadcast(const string& message);
	void setWSData();
	bool CheckForMessages();
	void setReadyForSending(bool status){readyForSending = status;}
	queue<string> *getMessages(){return tempWS->getMessages();}

private:
	list<WebSocket*> theList;
	std::mutex mutex;
	bool readyForSending = false;
	WebSocket* tempWS = nullptr;
	TempSensor* tempSensor;
	WaterLevel* waterLevel;
};

#endif

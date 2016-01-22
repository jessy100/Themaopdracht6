// Jan Zuurbier september 2014
// De broadcaster is thread-safe gemaakt.
// Dit is nodig omdat verschillende threads hier
// mogelijk tegelijkertijd gebruik van maken.

#include "broadcaster.h"

Broadcaster::Broadcaster(TempSensor* tempSensor, WaterLevel* waterLevel):
tempSensor(tempSensor),
waterLevel(waterLevel)
{
}

Broadcaster::~Broadcaster()
{
}

void Broadcaster::add(WebSocket* ws){
	std::lock_guard<std::mutex> lock(mutex);
	theList.push_back(ws);
}

void Broadcaster::remove(WebSocket* ws){
	std::lock_guard<std::mutex> lock(mutex);
	theList.remove(ws);
}

void Broadcaster::broadcast(const string& message){
	std::lock_guard<std::mutex> lock(mutex);
	for (std::list<WebSocket*>::iterator it=theList.begin(); it!=theList.end(); ++it)
		(*it)->handleMessage(message);
}

void Broadcaster::setWSData(){
	if(readyForSending){
		std::lock_guard<std::mutex> lock(mutex);
		for (std::list<WebSocket*>::iterator it=theList.begin(); it!=theList.end(); ++it){
			(*it)->sendData(tempSensor->getTemp(), waterLevel->getLevel());	
		}
	}
	
}

bool Broadcaster::CheckForMessages(){
	std::lock_guard<std::mutex> lock(mutex);
	for (std::list<WebSocket*>::iterator it=theList.begin(); it!=theList.end(); ++it){
		if((*it)->getMessageCount() != 0){
			tempWS = *it;
			return true;
		}return false;

	}		
}
	
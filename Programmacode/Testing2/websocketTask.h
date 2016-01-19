#include <iostream>
#include <thread>
#include "wasprogrammaTask.h"
#include "broadcaster.h"

#ifndef MY_LISTENER_H
#define MY_LISTENER_H
class MyListener : public WebSocketListener{
public:

	MyListener(Broadcaster & b):
	b(b)
	{}

	void onTextMessage( const string& s){
		b.broadcast(s);
		cout << "Recieved: " << s << endl;
	}

	void onClose(WebSocket* ws){
		b.remove(ws);
		delete ws;
	}

private:
	Broadcaster & b;
};
#endif

#ifndef WEBSOCKET_TASK_H
#define WEBSOCKET_TASK_H



//broadcaster meegeven aan de Websocket task, vervolgens ML aanmaken met als parameter &b.
//of: laat broadcaster alle websockets returnen, daar doorheen loopen in de wasprogrammaTask. 
class Websocket_task : public thread{
public: 
	Websocket_task(Broadcaster & b):
	b(b)
	{}
	void run() {
		try {
			TCPServerSocket servSock(8080);
			cout << "server running: " << servSock.getLocalAddress().getAddress() << endl;
			MyListener ml(b);
			for (;;) {
				TCPSocket *sock = servSock.accept();
				WebSocket* ws = new WebSocket(sock);
				ws->setListener(&ml);
				b.add(ws);
			}
		} 
		catch (SocketException &e) {
			cerr << e.what() << endl;           // Report errors to the console
		}
	}
private:
	Broadcaster & b;
	
};

#endif
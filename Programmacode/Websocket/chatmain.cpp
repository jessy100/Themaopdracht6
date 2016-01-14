#include <iostream>
#include "PracticalSocket.h"
#include "websocket.h"
#include "broadcaster.h"
#include <thread>

using namespace std;

Broadcaster b;

class MyListener : public WebSocketListener{
public:
	void onTextMessage( const string& s, WebSocket* ws){
		b.broadcast(s);
		cout << "Recieved: " << s << endl;
	}

	void onClose(WebSocket* ws){
		b.remove(ws);
		delete ws;
	}
};

MyListener ml;

void runserver()
{
	try {
    TCPServerSocket servSock(8080);
	cout << "server running: " << servSock.getLocalAddress().getAddress() << endl;
        for (;;) {
		TCPSocket *sock = servSock.accept();
		WebSocket* ws = new WebSocket(sock);
		ws->setListener(&	ml);
		b.add(ws);
    }
  } catch (SocketException &e) {
    cerr << e.what() << endl;           // Report errors to the console
  }
}

int main(int argc, char **argv)
{
	thread serverthread(runserver);
    serverthread.join();
}

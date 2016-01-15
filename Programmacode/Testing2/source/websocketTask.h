#include <iostream>
#include <thread>

Broadcaster b;

class MyListener : public WebSocketListener{
public:
	void onTextMessage( const string& s){
		b.broadcast(s);
		cout << "Recieved: " << s << endl;
		
	}

	void onClose(WebSocket* ws){
		b.remove(ws);
		delete ws;
	}
};

MyListener ml;

class Websocket_task : public thread{
public: 
	void run() {
		try {
			TCPServerSocket servSock(8080);
			cout << "server running: " << servSock.getLocalAddress().getAddress() << endl;
			for (;;) {
				cout << "before accepting websocket" << endl;
				TCPSocket *sock = servSock.accept();
				WebSocket* ws = new WebSocket(sock);
				ws->setListener(&	ml);
				b.add(ws);
				cout << "after accepting websocket" << endl;
			}
		} 
		catch (SocketException &e) {
			cerr << e.what() << endl;           // Report errors to the console
		}
	}
};
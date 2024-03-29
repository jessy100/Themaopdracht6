#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "PracticalSocket.h"
#include <thread>
#include <string>
#include <queue>


using namespace std;

class WebSocketException : public std::runtime_error {
public:

  WebSocketException(const std::string &message) throw();
  WebSocketException(const std::string &message, const std::string &detail) throw();
};

class WebSocketListener;

/* Gemaakt door Jan Zuurbier, sept 2014
 * Alleen voor onderwijsdoelen.
 * Vrij om te gebruiken en aan te passen.
 */


class WebSocket
{
public:
	WebSocket(TCPSocket*);
	~WebSocket();
	void close();
	void setListener(WebSocketListener*);
	void handleMessage(const string &message) throw (WebSocketException, SocketException);
	string getForeignAddress() { return sock->getForeignAddress().getAddress(); }

private:
	TCPSocket* sock;
	thread* thr;
	WebSocketListener* theListener = NULL;
	char* data;
	int datalen;
	bool closed;
	bool closing;
	void performHandshake() throw(WebSocketException, SocketException);
	void processFrame() throw(WebSocketException, SocketException);
	void handleConnection() ;
	void sendClose(const char*, size_t) throw (SocketException);
	void sendPong(const char*, size_t) throw (SocketException);
    queue <string> commands;

};

class WebSocketListener
{
public:
	virtual void onTextMessage(const string& s, WebSocket* ws) = 0;
	virtual void onClose(WebSocket* ws) = 0;
};


#endif // WEBSOCKET_H

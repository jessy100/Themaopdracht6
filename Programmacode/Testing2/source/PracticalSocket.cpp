/*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "PracticalSocket.h"

#include <errno.h>           // For errno
#include <signal.h>  //added
using namespace std;

#ifdef WIN32
static bool initialized = false;
#endif

///////////////////////////////////////////////////////////////////////////////
// SocketException
///////////////////////////////////////////////////////////////////////////////

SocketException::SocketException(const string &message) throw() :
  runtime_error(message) {
}

SocketException::SocketException(const string &message, const string &detail) throw() :
  runtime_error(message + ": " + detail) {
}

///////////////////////////////////////////////////////////////////////////////
// SocketTimedOutException
///////////////////////////////////////////////////////////////////////////////

SocketTimedOutException::SocketTimedOutException(const string &message) throw() :
  SocketException(message) {
}

///////////////////////////////////////////////////////////////////////////////
// SocketAddress
///////////////////////////////////////////////////////////////////////////////

static addrinfo *getAddressInfo(const char *host, const char *service,
                               SocketAddress::AddressType atype)
  throw(SocketException) {
  // Create criteria for the socket we require.
  addrinfo addrCriteria;
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_UNSPEC;             // Any address family

  switch (atype) {
  case SocketAddress::TCP_SOCKET:
    // Ask for addresses for a TCP socket.
    addrCriteria.ai_socktype = SOCK_STREAM;
    addrCriteria.ai_protocol = IPPROTO_TCP;
    break;
  case SocketAddress::TCP_SERVER:
    // Ask for addresses for a TCP server socket
    addrCriteria.ai_socktype = SOCK_STREAM;
    addrCriteria.ai_protocol = IPPROTO_TCP;
    addrCriteria.ai_flags = AI_PASSIVE;
    break;
  case SocketAddress::UDP_SOCKET:
    // Ask for addresses for a UDP socket.
    addrCriteria.ai_socktype = SOCK_DGRAM;
    addrCriteria.ai_protocol = IPPROTO_UDP;
    break;
  }

  // Get linked list of remote addresses
  addrinfo *servAddrs;
  int rtnVal = getaddrinfo(host, service, &addrCriteria, &servAddrs);
  if (rtnVal != 0)
    throw SocketException("getaddrinfo() failed", gai_strerror(rtnVal));

  return servAddrs;
}

SocketAddress::SocketAddress(const char *host, const char *service,
                             AddressType atype)
  throw(SocketException) {
  addrinfo *servAddrs = getAddressInfo(host, service, atype);

  // Extract just the first address.
  if (servAddrs == NULL)
    throw SocketException("No matching socket address");

  addrLen = servAddrs->ai_addrlen;
  memcpy( &addr, servAddrs->ai_addr, addrLen );

  freeaddrinfo(servAddrs);
}

SocketAddress::SocketAddress(const char *host, in_port_t port,
                             AddressType atype)
  throw(SocketException) {
  // Convert the numeric port request into a string.
  char service[6];
  snprintf(service, sizeof(service), "%d", port);
  addrinfo *servAddrs = getAddressInfo(host, service, atype);

  // Extract just the first address.
  if (servAddrs == NULL)
    throw SocketException("No matching socket address");

  addrLen = servAddrs->ai_addrlen;
  memcpy( &addr, servAddrs->ai_addr, addrLen );

  freeaddrinfo(servAddrs);
}

SocketAddress::SocketAddress(sockaddr *addrVal, socklen_t addrLenVal) {
  addrLen = addrLenVal;
  memcpy( &addr, addrVal, addrLen );
}

string SocketAddress::getAddress() const throw(SocketException) {
  void *numericAddress;

  switch (((sockaddr *)&addr)->sa_family) {
  case AF_INET:
    numericAddress = &(((sockaddr_in *)&addr)->sin_addr.s_addr);
    break;
  case AF_INET6:
    numericAddress = &(((sockaddr_in6 *)&addr)->sin6_addr.s6_addr);
    break;
  default:
    throw SocketException( "Unknown address type" );
  }

  static char addrBuffer[INET6_ADDRSTRLEN];
  if (inet_ntop(((sockaddr *)&addr)->sa_family, numericAddress, addrBuffer,
                sizeof(addrBuffer)) == NULL) {
    throw SocketException("Unable to get address");
  }

  return addrBuffer;
}

in_port_t SocketAddress::getPort() const throw(SocketException) {
  switch (((sockaddr *)&addr)->sa_family) {
  case AF_INET:
    return ntohs(((sockaddr_in *)&addr)->sin_port);
    break;
  case AF_INET6:
    return ntohs(((sockaddr_in6 *)&addr)->sin6_port);
    break;
  default:
    throw SocketException( "Unknown address type" );
  }
}

vector<SocketAddress> SocketAddress::lookupAddresses(const char *host,
                                                        const char *service,
                                                        AddressType atype)
  throw(SocketException) {
  addrinfo *servAddrs = getAddressInfo(host, service, atype);

  // Push a copy of each address onto our list.
  vector< SocketAddress > addrList;
  for (addrinfo *curAddr = servAddrs; curAddr != NULL;
       curAddr = curAddr->ai_next)
    addrList.push_back(SocketAddress((sockaddr *)(curAddr->ai_addr),
                                     curAddr->ai_addrlen));

  freeaddrinfo(servAddrs);

  return addrList;
}

vector<SocketAddress> SocketAddress::lookupAddresses(const char *host,
                                                        in_port_t port,
                                                        AddressType atype)
  throw(SocketException) {
  // Convert the numeric port request into a string.
  char service[6];
  snprintf(service, sizeof(service), "%d", port);
  addrinfo *servAddrs = getAddressInfo(host, service, atype);

  // Push a copy of each address onto our list.
  vector< SocketAddress > addrList;
  for (addrinfo *curAddr = servAddrs; curAddr != NULL;
       curAddr = curAddr->ai_next)
    addrList.push_back(SocketAddress((sockaddr *)(curAddr->ai_addr),
                                     curAddr->ai_addrlen));

  freeaddrinfo(servAddrs);

  return addrList;
}

///////////////////////////////////////////////////////////////////////////////
// Socket
///////////////////////////////////////////////////////////////////////////////

Socket::Socket() {
#ifdef WIN32
  if (!initialized) {
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD(2, 0); // Request WinSock v2.0
    if (WSAStartup(wVersionRequested, &wsaData) != 0) { // Load WinSock DLL
      throw SocketException("WSAStartup() failed", "Unable to load WinSock DLL");
    }
    initialized = true;
  }
#endif

  // No socket descriptor defined yet.
  sockDesc = -1;
}

Socket::~Socket() {
  if (sockDesc >= 0) {
    close();
  }
}

void Socket::cleanUp() throw(SocketException) {
#ifdef WIN32
  if (WSACleanup() != 0) {
    throw SocketException("WSACleanup() failed", "Unable to clean up Winsock DLL");
  }
#endif
}

SocketAddress Socket::getLocalAddress() throw(SocketException) {
  sockaddr_storage addr;
  socklen_t addrLen = sizeof(addr);

  if (getsockname(sockDesc, (sockaddr *) &addr, &addrLen) < 0) {
    throw SocketException("Fetch of local address failed (getsockname())");
  }

  return SocketAddress((sockaddr *)&addr, addrLen);
}

void Socket::close() {
#ifdef WIN32
  ::closesocket(sockDesc);
#else
  shutdown(sockDesc,SHUT_RD);
  ::close(sockDesc);
#endif
  sockDesc = -1;
}

void Socket::createSocket(const SocketAddress &address, int type,
                          int protocol) throw(SocketException) {
  // Destroy the old socket if there was one.
  if (sockDesc >= 0)
    close();

  int domain = PF_INET;

  // Look at the address and see what domain we should ask for.
  if (address.getSockaddr()->sa_family == AF_INET6)
    domain = PF_INET6;

  sockDesc = socket(domain, type, protocol);
  if ( sockDesc  < 0 )
    throw SocketException(string("Can't create socket : ") +
                          strerror(errno));
}

///////////////////////////////////////////////////////////////////////////////
// CommunicatingSocket
///////////////////////////////////////////////////////////////////////////////

void CommunicatingSocket::send(const void *buffer, int bufferLen)
  throw(SocketException) {
  if (::send(sockDesc, (raw_type *)buffer, bufferLen, 0) < 0) {
    throw SocketException("Send failed (send())");
  }
}

size_t CommunicatingSocket::recv(void *buffer, int bufferLen)
  throw(SocketException) {
  int rtn = ::recv(sockDesc, (raw_type *) buffer, bufferLen, 0);
  if (rtn < 0) {
    throw SocketException("Receive failed (recv())");
  }

  return rtn;
}

size_t CommunicatingSocket::recvFully(void *buffer, int bufferLen)
  throw(SocketException) {
  int rcount = 0;
  int len = ::recv(sockDesc, (raw_type *) buffer, bufferLen, 0);
  while (len > 0 && rcount + len < bufferLen) {
    rcount += len;
    len = ::recv(sockDesc, (raw_type *) (((char *) buffer) + rcount),
                 bufferLen - rcount, 0);
  }

  if (len < 0)
    throw SocketException("Receive failed (recv())");

  return rcount + len;
}


SocketAddress CommunicatingSocket::getForeignAddress() throw(SocketException) {
  sockaddr_storage addr;
  socklen_t addrLen = sizeof(addr);

  if (getpeername(sockDesc, (sockaddr *) &addr, &addrLen) < 0) {
    throw SocketException("Fetch of foreign address failed (getpeername())");
  }

  return SocketAddress((sockaddr *)&addr, addrLen);
}

///////////////////////////////////////////////////////////////////////////////
// SocketStreamBuffer
///////////////////////////////////////////////////////////////////////////////

/** Subclass of basic_streambuf for reading and writing to
    instances of TCPSocket. */
template <class CharT, class Traits = std::char_traits<CharT> >
class SocketStreamBuffer : public std::basic_streambuf<CharT, Traits> {
public:
  typedef typename Traits::int_type                 int_type;

  SocketStreamBuffer(TCPSocket *sock) {
    // Save the a copy of the socket we are wrapping.
    SocketStreamBuffer::sock = sock;
    this->setg(inBuffer, inBuffer + sizeof(inBuffer),
         inBuffer + sizeof(inBuffer));
    this->setp(outBuffer, outBuffer + sizeof(outBuffer));
    extra = 0;
  }

protected:
  int_type overflow(int_type c = Traits::eof()) {
    // Push out anything in the buffer.
    sync();

    // If an extra character was passed in, put it into our buffer.
    if (c != Traits::eof()) {
      this->sputc(Traits::to_char_type(c));
    }

    return 0;
  }

  int sync() {
    // Write out the contents of the buffer.
    sock->send(outBuffer, (this->pptr() - outBuffer) * sizeof(CharT));
    this->setp(outBuffer, outBuffer + sizeof(outBuffer));
    return 0;
  }

  int_type underflow() {
    // Read up to a buffer full.
    size_t len = sock->recv(inBuffer, sizeof(inBuffer) * sizeof(CharT));

    // Report eof if we didn't get anything.
    if (len == 0) {
      return Traits::eof();
    }

    // Adjust the base class buffer pointers and return the first
    // character read.
    this->setg(inBuffer, inBuffer, inBuffer + len / sizeof(CharT));
    return this->sgetc();
  }

private:
  // Pointer to the socket used as the associated sequence.
  TCPSocket *sock;

  // Input and output buffers for characters waiting to write or ready
  // to read.
  CharT inBuffer[1024];
  CharT outBuffer[1024];

  // Number of extra bytes remaining from the last read but not returned
  // to the caller because they are fewer than sizeof(CharT)
  // We don't use this yet, but we should.
  size_t extra;
};

///////////////////////////////////////////////////////////////////////////////
// TCPSocket
///////////////////////////////////////////////////////////////////////////////

TCPSocket::TCPSocket() {
  myStream = NULL;
}

TCPSocket::~TCPSocket() {
  if (myStream != NULL) {
    delete myStream;
    delete myStreambuf;
  }
}

TCPSocket::TCPSocket(const char *foreignAddress,
                     in_port_t foreignPort)
  throw(SocketException) {
  myStream = NULL;
  sockDesc = -1;

  // Create criteria for the socket we require.
  addrinfo addrCriteria;
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_UNSPEC;             // Any address family
  addrCriteria.ai_socktype = SOCK_STREAM;         // Only streaming sockets
  addrCriteria.ai_protocol = IPPROTO_TCP;         // Only TCP protocol

  // Turn the port number into a string.
  char service[6];
  snprintf(service, sizeof(service), "%d", foreignPort);

  // Get linked list of remote addresses
  addrinfo *servAddrs;
  int rtnVal = getaddrinfo(foreignAddress, service,
                           &addrCriteria, &servAddrs);
  if (rtnVal != 0)
    throw SocketException("getaddrinfo() failed", gai_strerror(rtnVal));

  // Try each address until we get one that we can connect.
  for (addrinfo *curAddr = servAddrs; curAddr != NULL && sockDesc < 0;
       curAddr = curAddr->ai_next) {
    // Try to make a new socket.
    if ((sockDesc = socket(curAddr->ai_family, curAddr->ai_socktype,
                           curAddr->ai_protocol)) >= 0 ) {
      // Try to connect the socket to the requested foreign address.
      if (::connect(sockDesc, curAddr->ai_addr, curAddr->ai_addrlen) < 0) {
        // If we fail, close the socket
        ::close(sockDesc);
        sockDesc = -1;
      }
    }
  }

  freeaddrinfo(servAddrs);

  if (sockDesc == -1) {
    throw SocketException("Unable to connect");
  }
}

TCPSocket::TCPSocket(int desc) {
  myStream = NULL;
  sockDesc = desc;
}

void TCPSocket::bind(const SocketAddress &localAddress) throw(SocketException) {
  createSocket(localAddress, SOCK_STREAM, IPPROTO_TCP);

  if (::bind(sockDesc, localAddress.getSockaddr(),
             localAddress.getSockaddrLen()) < 0)
    throw SocketException(string("Call to bind() failed :  ") +
                          strerror(errno));
}

void TCPSocket::connect(const SocketAddress &foreignAddress)
  throw(SocketException) {
  if (sockDesc < 0)
    createSocket(foreignAddress, SOCK_STREAM, IPPROTO_TCP);

  if (::connect(sockDesc, foreignAddress.getSockaddr(),
                foreignAddress.getSockaddrLen()) < 0)
    throw SocketException(string("Call to connect() failed :  ") +
                          strerror(errno));
}

iostream &TCPSocket::getStream() throw(SocketException) {
  if (myStream == NULL) {
    myStreambuf = new SocketStreamBuffer<char>(this);
    myStream = new iostream(myStreambuf);
  }
  return *myStream;
}

///////////////////////////////////////////////////////////////////////////////
// TCPServerSocket
///////////////////////////////////////////////////////////////////////////////

int findTCPServerSocketDesc(const char *localAddr, const char *localPort) {
  // Construct the server address structure
  addrinfo addrCriteria; // Criteria for address
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_UNSPEC; // AF_XXXX according to RFC 3493
  addrCriteria.ai_flags = AI_PASSIVE;
  addrCriteria.ai_socktype = SOCK_STREAM;
  addrCriteria.ai_protocol = IPPROTO_TCP;

  struct addrinfo *servAddr; // List of server addresses
  int result;
  if ((result = getaddrinfo(localAddr, localPort, &addrCriteria,
                            &servAddr)) != 0)
    throw SocketException("getaddrinfo() failed: %s", gai_strerror(result));

  // Create socket for incoming connections
  int desc;
  if ((desc = socket(servAddr->ai_family, servAddr->ai_socktype,
                         servAddr->ai_protocol)) < 0)
    throw SocketException("Can't create socket");
  int enable = 1;
  setsockopt(desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
  // Bind to the local address
  result = bind(desc, servAddr->ai_addr, servAddr->ai_addrlen);
  // Free address list, whether or not the bind worked.
  freeaddrinfo(servAddr);

  // Throw an exception if the bind failed.
  if (result < 0) {
    throw SocketException("can't perform bind");
  }

  return desc;
}

TCPServerSocket::TCPServerSocket() {
}

TCPServerSocket::TCPServerSocket(in_port_t localPort, int queueLen)
  throw(SocketException) {
  char buffer[ 1024 ];
  snprintf(buffer, sizeof(buffer), "%d", localPort);
  sockDesc = findTCPServerSocketDesc(NULL, buffer);
  setListen(queueLen);
}

void TCPServerSocket::bind(const SocketAddress &localAddress)
  throw(SocketException) {
  createSocket(localAddress, SOCK_STREAM, IPPROTO_TCP);

  if (::bind(sockDesc, localAddress.getSockaddr(),
             localAddress.getSockaddrLen()) < 0)
    throw SocketException(string("Call to bind() failed :  ") +
                          strerror(errno));

  // This is temporary.
  setListen(5);
}

TCPSocket *TCPServerSocket::accept() throw(SocketException) {
  int newConnSD;
  if ((newConnSD =:: accept(sockDesc, NULL, 0)) < 0) {
    throw SocketException("Accept failed (accept())");
  }

  return new TCPSocket(newConnSD);
}

void TCPServerSocket::setListen(int queueLen) throw(SocketException) {
  if (listen(sockDesc, queueLen) < 0) {
    throw SocketException("Set listening socket failed (listen())");
  }
}

///////////////////////////////////////////////////////////////////////////////
// UDPSocket
///////////////////////////////////////////////////////////////////////////////

UDPSocket::UDPSocket() throw (SocketException): timeOutPeriod(0) {
	SocketAddress adr("0.0.0.0", (const char*) NULL, SocketAddress::UDP_SOCKET);
	createSocket(adr, SOCK_DGRAM, IPPROTO_UDP);
}

void UDPSocket::setBroadcast() throw(SocketException)  {
  // If this fails, we'll hear about it when we try to send.  This will allow
  // system that cannot broadcast to continue if they don't plan to broadcast
  int broadcastPermission = 1;
  int n = setsockopt(sockDesc, SOL_SOCKET, SO_BROADCAST,
             (raw_type *) &broadcastPermission, sizeof(broadcastPermission));
  if( n < 0) throw SocketException("Broadcast allowance set failed (setsockopt())");
}

void UDPSocket::bind(const SocketAddress &localAddress)
  throw(SocketException) {
  createSocket(localAddress, SOCK_DGRAM, IPPROTO_UDP);
  int n = ::bind(sockDesc, localAddress.getSockaddr(), localAddress.getSockaddrLen());
  if (n < 0) throw SocketException("can not perform bind");
}

void UDPSocket::connect(const SocketAddress &foreignAddress)
  throw(SocketException) {
  if (sockDesc < 0)
    createSocket(foreignAddress, SOCK_DGRAM, IPPROTO_UDP);

  if (::connect(sockDesc, foreignAddress.getSockaddr(),
                foreignAddress.getSockaddrLen()) < 0)
    throw SocketException(string("Call to connect() failed :  ") +
                          strerror(errno));
}

void UDPSocket::disconnect() throw(SocketException) {
  sockaddr_in nullAddr;
  memset(&nullAddr, 0, sizeof(nullAddr));
  nullAddr.sin_family = AF_UNSPEC;

  // Try to disconnect
  if (::connect(sockDesc, (sockaddr *) &nullAddr, sizeof(nullAddr)) < 0) {
#ifdef WIN32
    if (errno != WSAEAFNOSUPPORT) {
#else
    if (errno != EAFNOSUPPORT) {
#endif
      throw SocketException("Disconnect failed (connect())");
    }
  }
}

void UDPSocket::sendTo(const void *buffer, int bufferLen,
                       const SocketAddress &foreignAddress)
  throw(SocketException) {
  // Write out the whole buffer as a single message.
  if (sendto(sockDesc, (raw_type *) buffer, bufferLen, 0,
             foreignAddress.getSockaddr(),
             foreignAddress.getSockaddrLen()) != bufferLen) {
    throw SocketException("Send failed (sendto())");
  }
}

int UDPSocket::recvFrom(void *buffer, int bufferLen,
                        SocketAddress &sourceAddress)
  throw(SocketException) {
  sockaddr_storage clntAddr;
  socklen_t addrLen = sizeof(clntAddr);
  //added
  alarm(timeOutPeriod);
  int rtn = recvfrom(sockDesc, (raw_type *) buffer, bufferLen, 0,
                     (sockaddr *) &clntAddr, (socklen_t *) &addrLen);
  if (rtn < 0) {
	  if(errno == EINTR)
		   throw SocketTimedOutException("socket timed out");
		else

			throw SocketException("Receive failed (recvfrom())");
  }
  sourceAddress = SocketAddress((sockaddr *)&clntAddr, addrLen);

  return rtn;
}

void UDPSocket::setMulticastTTL(unsigned char multicastTTL)
  throw(SocketException) {
  if (setsockopt(sockDesc, IPPROTO_IP, IP_MULTICAST_TTL,
                 (raw_type *) &multicastTTL, sizeof(multicastTTL)) < 0) {
    throw SocketException("Multicast TTL set failed (setsockopt())");
  }
}

 void UDPSocket::setMulticastLoop(bool loop) throw(SocketException) {
	 int multicastLoop = (loop ) ? 1 : 0;
	 if (setsockopt(sockDesc, IPPROTO_IP, IP_MULTICAST_LOOP,
                 (raw_type *) &multicastLoop, sizeof(multicastLoop)) < 0) {
		throw SocketException("MulticastLoop set failed (setsockopt())");
	 }
 }

 //added
 void CatchAlarm(int sgtype){
	 //do nothing
	 std::cout << sgtype;
	 
 }

 //added
 void UDPSocket::setTimeOut(int sec) throw (SocketException) {
	// Set signal handler for alarm signal
  struct sigaction handler; // Signal handler
  handler.sa_handler = CatchAlarm;
  if (sigfillset(&handler.sa_mask) < 0) // Block everything in handler
    throw SocketException("sigfillset failed");
  handler.sa_flags = 0;
	if (sigaction(SIGALRM, &handler, 0) < 0)
		throw SocketException("sigaction failed");
	timeOutPeriod = sec;
 }

 void UDPSocket::joinGroup(const string &multicastGroup) throw(SocketException) {
   struct ip_mreq multicastRequest;

   multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
   multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
   if (setsockopt(sockDesc, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                  (raw_type *) &multicastRequest, sizeof(multicastRequest)) < 0) {
     throw SocketException("Multicast group join failed (setsockopt())");
   }
 }

 void UDPSocket::leaveGroup(const string &multicastGroup) throw(SocketException) {
   struct ip_mreq multicastRequest;

   multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
   multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
   if (setsockopt(sockDesc, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                  (raw_type *) &multicastRequest, sizeof(multicastRequest)) < 0) {
     throw SocketException("Multicast group leave failed (setsockopt())");
   }
 }

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cassert>
#include <iostream>

#define EMTU 1500 // Ethernet MTU size
#define PORT 8888 // The port on which to listen for incoming data

// Initialize Winsock
int Init()
{
  WSADATA wsa;
  const auto error = WSAStartup(MAKEWORD(2, 2), &wsa);

  return error;
}

// Make a socket--a handle (basically a pointer to a block of system-allocated
// RAM that holds the data.).
SOCKET CreateSocket(const int protocol)
{
  // UDP, socket type datagram
  auto type = SOCK_DGRAM;

  // If TCP, socket type stream
  if (protocol == IPPROTO_TCP)
  {
    type = SOCK_STREAM;
  }

  // INVALID_SOCKET if fail, in which case call WSAGetLastError()
  const auto sock = socket(AF_INET, type, protocol);

  return sock;
}

// Make an address--a data structure that's more than just IP address;
// Allocate it yourself.
sockaddr_in* CreateAddress(const char* ip, const int port)
{
  // Allocate memory for the sockaddr in structure.
  const auto addr = new sockaddr_in;
  addr->sin_family = AF_INET;
  // Remember to go from host byte order to network byte order.
  addr->sin_port = htons(port);
 
  if (ip == nullptr)
  {
    // Bind to all IP addresses local machine may have.
    addr->sin_addr.S_un.S_addr = INADDR_ANY;
  }
  else
  {
    // Take dotted-decimal string and turn it into a 32-bit IP address,
    // in network byte order. Return INADDR_NONE on fail.
    inet_pton(addr->sin_family, ip, &(addr->sin_addr));
  }

  return addr;
  // Caller will be responsible for free().
}

int Send(const SOCKET sock, char const* buf, const int len, sockaddr_in* dest)
{
  const auto sendlen = sendto(sock, buf, len, 0,
    reinterpret_cast<sockaddr*>(dest), sizeof(sockaddr_in));

  if (sendlen == SOCKET_ERROR)
  {
    return -1;
  }

  return sendlen;
}

int Receive(const SOCKET sock, char* buf, const int maxSize)
{
  sockaddr from{};
  int fromlen = sizeof(sockaddr);

  // Specify a socket, provide a buffer with a maximum size.
  const auto recvlen = recvfrom(sock, buf, maxSize, 0, &from, &fromlen);
  if (recvlen == SOCKET_ERROR)
  {
    return -1;
  }

  return recvlen;
}

// Associates a socket with a remote address.
int Connect(const SOCKET sock, sockaddr_in* addr)
{
  if (connect(sock, reinterpret_cast<sockaddr*>(addr), sizeof(sockaddr_in))
    == SOCKET_ERROR)
  {
    return WSAGetLastError();
  }

  return 0;
}

// Frees up everything assiciated with the socket.
void Close(const SOCKET sock)
{
  closesocket(sock);
}

// Shuts down everything and unloads the DLL.
void Deinit()
{
  WSACleanup();
}


int main(int argc, char** argv)
{
  if (Init() != 0)
  {
    std::cout << "Start up failed. Error code : " << WSAGetLastError();
    exit(EXIT_FAILURE);
  }
  //std::cout << "Initialized" << std::endl;

  // Make a socket and an address.
  const auto sock = CreateSocket(IPPROTO_UDP);
  const auto addr = CreateAddress("104.131.138.5", PORT);

  //const auto connectResult = Connect(sock, addr);
  //assert(connectResult == 0);

  // Get the sending buffer.
  std::string sendbuf = argv[0];
  const auto pos = sendbuf.find_last_of('\\');
  sendbuf = sendbuf.substr(pos + 1);
  const auto csendbuf = sendbuf.c_str();

  // Send data over UDP and listen for a response.
  const auto sendlen = Send(sock, csendbuf, sendbuf.length(), addr);
  assert(sendlen >= 0);
  char recvbuf[EMTU]{};
  const auto recvlen = Receive(sock, recvbuf, sizeof(recvbuf));
  assert(recvlen >= 0);

  // Close the socket and shut down Winsock.
  Close(sock);
  Deinit();

  std::cout << recvbuf << std::endl;

  std::getchar();

  return 0;
}

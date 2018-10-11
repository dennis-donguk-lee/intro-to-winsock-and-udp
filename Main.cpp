#include <WinSock2.h>
#include <WS2tcpip.h>
#include <experimental/filesystem>

// Make a socket--a handle (basically a pointer to a block of system-allocated
// RAM that holds the data.).
SOCKET CreateSocket(const int protocol)
{
  auto sock = INVALID_SOCKET;
  // UDP, socket type datagram
  auto type = SOCK_DGRAM;

  // If TCP, socket type stream
  if (protocol == IPPROTO_TCP)
  {
    type = SOCK_STREAM;
  }

  // INVALID_SOCKET if fail, in which case call WSAGetLastError()
  sock = socket(AF_INET, type, protocol);

  return sock;
}

// Make an address--adata structure that's more than just IP address;
// Allocate it yourself.
sockaddr_in* CreateAddress(char* ip, const int port)
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

int Send(const SOCKET sock, char* buf, const int len, sockaddr_in* dest)
{
  const auto sentlen = sendto(sock, buf, len, 0, reinterpret_cast<sockaddr*>(dest)
    , sizeof(sockaddr_in));

  if (sentlen == SOCKET_ERROR)
  {
    return -1;
  }

  return sentlen;
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


int main()
{
  return 0;
}

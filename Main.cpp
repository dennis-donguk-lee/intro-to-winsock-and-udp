#include <WinSock2.h>
#include <WS2tcpip.h>

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
    // Take dotted_decimal string and turn it into a 32-bit IP address,
    // in network byte order. Return INADDR_NONE on fail.
    inet_pton(addr->sin_family, ip, &(addr->sin_addr));
  }

  return addr;
  // Caller will be responsible for free().
}

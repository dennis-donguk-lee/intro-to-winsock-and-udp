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


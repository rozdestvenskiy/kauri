#ifndef SERVER_NET_API_H
#define SERVER_NET_API_H

#ifdef _WIN32
#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN
//
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment (lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include <inttypes.h>
#include <iostream>
#include <string>
#include <string.h>
#include <vector>

using namespace std;

struct sockaddr_in serv_addr;
struct hostent *server;

#ifdef _WIN32
  SOCKET socket_create()
  {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    int res;

    res = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (res != 0)
    {
        printf("WSAStartup failed with error: %d\n", res);
        cerr << "WSAStartup failed with error" << endl;
        exit(1);
    }
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    res = getaddrinfo("127.0.0.1", "8080", &hints, &result);
    if (res != 0)
    {
        printf("getaddrinfo failed with error: %d\n", res);
        WSACleanup();
        exit(1);
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // Create a SOCKET for connecting to server
        sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sock == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        res = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (res == SOCKET_ERROR)
        {
            closesocket(sock);
            sock = INVALID_SOCKET;
            continue;
        }
        break;
    }




    freeaddrinfo(result);

    return sock;
  }
#else
  int socket_create()
  {
    struct sockaddr_in serv_addr;
    struct hostent *server;
    const char* short_options = "-:a:p:";
    string ipv4_addr_str = "127.0.0.1";
    string port_str = "8080";
    int ip_flag = 0;
    int port_flag = 0;
    char buffer[256];
    int rez;
    int option_index = -1;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        printf("Error wile creating socket\n");
        exit(1);
    }

    int port = stoi(port_str);
    if (port == 0)
    {
        printf("Error: wrong format of port number\n");
        exit(1);
    }
    //server = getaddrinfo(ipv4_addr_str);
    /*
    if (server == NULL)
    {
        printf("Error: no such host\n");
        exit(1);
    }*/
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Error while connecting\n");
        close(sock);
        exit(1);
    }
    return sock;
  }
#endif

#ifdef _WIN32
  int socket_quit(SOCKET sock)
  {
    return WSACleanup();
  }
#else
  int socket_quit(int sock)
  {
    close(sock);
    return 0;
  }
#endif

#ifdef _WIN32
  string socket_recv(SOCKET sock, unsigned long long length)
  {
    char *buff = (char*) malloc(sizeof(char) * length);
    int x = recv(sock, buff, length, 0);
    if (x == -1)
    {
      cout << "err" << endl;
      exit(1);
    }
    string str = (const char*)buff;
    delete(buff);
    return str;
  }
#else
  string socket_recv(int sock, unsigned long long length)
  {
    char *buff = (char*) malloc(sizeof(char) * length);
    int x = recv(sock, buff, length, 0);
    if (x == -1)
    {
      cout << "err" << endl;
      exit(1);
    }
    string str = (const char*)buff;
    delete(buff);
    return str;
  }
#endif

#ifdef _WIN32
  int socket_send(SOCKET sock, char* msg)
  {
      return send(sock, msg, (int)strlen(msg), 0);
  }
#else
  int socket_send(int sock, char* msg)
  {
      return send(sock, msg, strlen(msg), 0);
  }
#endif

#ifdef _WIN32
int socket_send_length(SOCKET sock, unsigned long long length)
{
  vector <unsigned char> res;
  int kbytes = 0;
  if (length > 127)
  {
    res.push_back(0);
    for (int i = 0; i < sizeof(unsigned long long); i++)
    {
      if (length > (1 << (8 * i)))
      {
        kbytes++;
      }
      else
      {
        break;
      }
    }
    res[0] |= 128;
    res[0] |= kbytes;
    for (int i = kbytes - 1; i >= 0; i--)
    {
      res.push_back(((255 << (i * 8)) & length) >> (i * 8));
    }
  }
  else
  {
    res.push_back(length);
  }

  for (int i = 0; i < res.size(); i++)
  {
    send(sock, &res[i], sizeof(unsigned char), 0);
  }
    return 0;
}
#else
  int socket_send_length(int sock, unsigned long long length)
  {
    vector <unsigned char> res;
    int kbytes = 0;
    if (length > 127)
    {
      res.push_back(0);
      for (int i = 0; i < sizeof(unsigned long long); i++)
      {
        if (length > (1 << (8 * i)))
        {
          kbytes++;
        }
        else
        {
          break;
        }
      }
      res[0] |= 128;
      res[0] |= kbytes;
      for (int i = kbytes - 1; i >= 0; i--)
      {
        res.push_back(((255 << (i * 8)) & length) >> (i * 8));
      }
    }
    else
    {
      res.push_back(length);
    }

    for (int i = 0; i < res.size(); i++)
    {
      send(sock, &res[i], sizeof(unsigned char), 0);
    }
      return 0;
  }
#endif




#endif

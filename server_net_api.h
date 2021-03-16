#ifndef SERVER_NET_API_H
#define SERVER_NET_API_H

#ifdef _WIN32
#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN
//
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>
#pragma comment (lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#endif
#include <inttypes.h>
#include <iostream>
#include <string>
#include <string.h>
#include <vector>

using namespace std;

struct sockaddr_in serv_addr;
struct hostent* server;

#ifdef _WIN32
SOCKET socket_create()
{
    WSADATA wsa_data;
    SOCKET sock = INVALID_SOCKET;
    struct addrinfo* result = NULL;
    struct addrinfo hints;
    int res;

    res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
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
    hints.ai_flags = AI_PASSIVE;

    res = getaddrinfo("127.0.0.1", "8080", &hints, &result);
    if (res != 0)
    {
        printf("getaddrinfo failed with error: %d\n", res);
        WSACleanup();
        exit(1);
    }

    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        exit(1);
    }

    res = bind(sock, result->ai_addr, (int)result->ai_addrlen);
    if (res == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    res = listen(sock, SOMAXCONN);
    if (res == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        exit(1);
    }
    return sock;
}
#else
int socket_create()
{
    string port_str = "8080";
    string ip_addr_str = "127.0.0.1";
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        printf("Error wile creating socket\n");
        exit(1);
    }
    int port = stoi(port_str);
    int res;
    int fx = 1;
    res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &fx, sizeof(int));
    if (res != 0)
    {
        printf("Error: wrong sock options\n");
        exit(1);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(port);
    res = bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (res != 0)
    {
        printf("Error: while binding\n");
        cerr << "Error: while binding" << endl;
        exit(1);
    }
    listen(sock, 10);
    return sock;
}
#endif

#ifdef _WIN32
SOCKET socket_connect(SOCKET sock)
{
    SOCKET client = accept(sock, NULL, NULL);
    if (client == INVALID_SOCKET)
    {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    return client;
}
#else
int socket_connect(int sock)
{
    int client = accept(sock, (struct sockaddr*)NULL, NULL);
    return client;
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
    cout << "length = " << x << endl;
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
string socket_recv_length(SOCKET sock)
{
    char* buff = (char*)malloc(sizeof(char) * 250);
    int x = recv(sock, buff, 250, 0);
    //cout << buff << endl;
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
unsigned long long socket_recv_length(int sock)
{
    char *sx = (char*) malloc(sizeof(char) * 4);
    int x = recv(sock, sx, 1, 0);
    unsigned char ch = (unsigned char)sx[0];
    vector <unsigned char> vect;
    unsigned long long res = 0;
    if (ch < 128)
    {
      vect.push_back(ch);
      res += vect[0];
    }
    else
    {
      vect.push_back(ch);
      int kbytes = vect[0] & 127;
      for (int i = 1; i < kbytes + 1; i++)
      {
        recv(sock, sx, sizeof(unsigned char), 0);
        ch = sx[0];
        vect.push_back(ch);
        res += (vect[i] << (8 * (kbytes - i)));
      }
    }
    return res;
}
#endif

#ifdef _WIN32
int socket_send(SOCKET sock, char* msg)
{
    return send(sock, msg, sizeof(msg), 0);
}
#else
int socket_send(int sock, char* msg)
{
    return send(sock, msg, strlen(msg), 0);
}
#endif

typedef struct
{
  //string msg;
  unsigned long long length;
  int client_sock;
  struct sockaddr_in client;
} thread_dataS;

#ifdef _WIN32
static void* threadFunc(void* data)
{
  thread_dataS *data = (thread_dataS *) thread_data_in;
  //data->client_sock = thread_data_in->client_sock;
  //data->length = thread_data_in->length;
  string msg = socket_recv(data->client_sock, data->length);
  //sleep(5);
  cout << msg << endl;

  //pthread_detach(pthread_self());
  //delete(data);
  //pthread_exit(0);

}

int thread_create(SOCKET client_sock, unsigned long long length)
{
  thread_dataS *thread_data_in = new thread_dataS;
  thread_data_in->client_sock = client_sock;
  thread_data_in->length = length;

  int x = _beginthreadex(0, 0, threadFunc, thread_data_in, 0, NULL);
  if (x == 0)
  {
    cerr << "error creating thread" << endl;
    exit(1);
  }
  delete(thread_data_in);
  return 0;
}




#else


static void* threadFunc(void* thread_data_in)
{
  thread_dataS *data = (thread_dataS *) thread_data_in;
  //data->client_sock = thread_data_in->client_sock;
  //data->length = thread_data_in->length;
  string msg = socket_recv(data->client_sock, data->length);
  //sleep(5);
  cout << msg << endl;



  pthread_detach(pthread_self());
  //delete(data);
  pthread_exit(0);
}

int thread_create(int client_sock, unsigned long long length)
{
  pthread_t *new_thread = new pthread_t;
  if (new_thread == NULL)
  {
    cerr << "Error while creating thread" << endl;
    exit(1);
  }
  thread_dataS *thread_data_in = new thread_dataS;
  thread_data_in->client_sock = client_sock;
  thread_data_in->length = length;
  //thread_data_in->msg = msg;

  int rr;
  rr = pthread_create(new_thread, NULL, threadFunc, (void*)thread_data_in);
  if (rr != 0)
  {
    cerr << "Error while creating thread" << endl;
    exit(1);
  }

  delete(thread_data_in);
  return 0;
}

#endif




#endif

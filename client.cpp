#include <errno.h>
#include <inttypes.h>
#include <iostream>
#include <string>
#include "client_net_api.h"

struct option *long_options;

using namespace std;

int main(int argc, char* argv[])
{
  char *msg = (char*)malloc(sizeof(char) * 100);
  snprintf(msg, sizeof("01234567890XYZ"), "01234567890XYZ");

  #ifdef _WIN32
    SOCKET sock = socket_create();
  #else
    int sock = socket_create();
  #endif

  socket_send(sock, msg);

  socket_quit(sock);




  return 0;
}

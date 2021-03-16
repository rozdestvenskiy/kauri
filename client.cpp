#include <errno.h>
#include <inttypes.h>
#include <iostream>
#include <string>
#include <ctime>
#include "client_net_api.h"


struct option *long_options;

using namespace std;

int main(int argc, char* argv[])
{
  srand(time(NULL));

  int x = rand() % 30 + 10;
  string msgstr(x, 'A');

  for (int i = 0; i < x; i++)
  {
    //cout << rand() % 26 + 65 << endl;
    msgstr[i] = rand() % 26 + 65;
  }

  char *msg = &msgstr[0];

  cout << msg << endl;
  cout << "length = " << x << endl;




  //snprintf(msg, sizeof("01234567890XYZ"), "01234567890XYZ");
  #ifdef _WIN32
    SOCKET sock = socket_create();
  #else
    int sock = socket_create();
  #endif
  //socket_send(sock, msg);

  socket_send_length(sock, x);

  socket_send(sock, msg);

  socket_quit(sock);

  //delete(msg);


  return 0;
}

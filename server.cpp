#include <errno.h>
#include "server_net_api.h"

//#include <sys/types.h>

#define BUF_SIZE 1024

//int sock;


void* threadFunc(void* thread_data);

/*
typedef struct {
  char *file_name;
	FILE *fout;
  int out_socket;
  struct sockaddr_in client;
} thread_dataS;*/

using namespace std;

int main(int argc, char* argv[])
{
  #ifdef _WIN32
    SOCKET sock = socket_create();
  #else
    int sock = socket_create();
  #endif

  int cl = socket_connect(sock);

  cout << sock << endl;
  cout << cl << endl;

  string msg = socket_recv(cl);

  cout << msg << endl;

  int connfd, n;
  return 0;
}
/*
  while(1)
  {
      str_in = malloc(sizeof(char) * 255);
      if (str_in == NULL)
      {
          exit(1);
      }
      connfd = accept(sock, (struct sockaddr*)NULL, NULL);
      if (connfd == -1)
      {
          exit(1);
      }

      n = read(connfd, str_in, 255);

      if (n == -1)
      {
          exit(1);
      }

      int length = 0;
      for (int i = 0; i < strlen(str_in); i++)
      {
          if (str_in[i] == '\n')
          {
              break;
          }
          else
          {
              length++;
          }
      }
  }
  return 0;
}
/*
      pthread_t *new_thread = malloc(sizeof(pthread_t));
      if (new_thread == NULL)
      {
          exit(1);
      }
      thread_dataS *td_in = malloc(sizeof(thread_dataS));
      if (td_in == NULL)
      {
          exit(1);
      }
      td_in->file_name = malloc(sizeof(char) * (length + 10));
      if (td_in->file_name == NULL)
      {
          exit(1);
      }
      td_in->out_socket = connfd;
  char *xx = malloc(sizeof(char) * 100);
  snprintf(xx, sizeof(char) * 100, "%s%s", "CP", str_in);
  strncpy(td_in->file_name, xx, length + 2);

      int rr;
      rr = pthread_create(&(new_thread), NULL, threadFunc, td_in);
      if (rr != 0)
      {
          exit(1);
      }
      free(str_in);

  }
  close(sock);
*/

/*
void* threadFunc(void* thread_data)
{
    thread_dataS *data = (thread_dataS *) thread_data;
	int ncheck;
	int buffX;
	int bytes;
	int byteC;
	char by;
	read(data->out_socket, &byteC, sizeof(int));
	bytes = ntohl(byteC);

	FILE *res = fopen(data->file_name, "w");
	for (int i = 0; i < bytes - 1; i++)
	{
		ncheck = recv(data->out_socket, &by, sizeof(char), 0);

		if (ncheck == -1)
		{
			close(res);
			break;
		}
		if (by == -1)
		{
			close(res);
			break;
		}
    fprintf(res, "%c", by);
		//putc(by, res);
	}

	int n = write(data->out_socket, "Successful", strlen("Successful"));

    close(data->out_socket);
	  fclose(res);
	//free(data);
    free(thread_data);
    pthread_detach(pthread_self());
    pthread_exit(0);
}
*/

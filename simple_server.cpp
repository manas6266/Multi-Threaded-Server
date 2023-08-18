#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <queue>
#include <netinet/in.h>
#include "http_server.hh"
#include <pthread.h>
#include <signal.h>
pthread_mutex_t mutex;
pthread_cond_t conditionvar;
pthread_cond_t conditionvariable2;
pthread_t threadarray[20];
queue<int> buff;
int flag;
int count = 0;
void error(char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}
void producer(int newsockfd)
{
  pthread_mutex_lock(&mutex);
  while(count == 10000)
  {
    printf("Cannot enter request queue");
    pthread_cond_wait(&conditionvariable2,&mutex);
  }
  buff.push(newsockfd);
  count = count + 1;
  pthread_cond_signal(&conditionvar);
  pthread_mutex_unlock(&mutex);
}
void handle_sigint(int sig){
  flag=1;
  pthread_cond_broadcast(&conditionvar);
  for(int i=0;i<20;i++)
  {
    pthread_cancel(threadarray[i]);
    pthread_join(threadarray[i],NULL);
  }
  exit(0);
}
void *startfunction(void *arg)
{
  char buffer[1024];
  int n;
  int my_arg = *((int *)arg);
  // free(arg);
  while (1)
  {

    pthread_mutex_lock(&mutex);
    while (count == 0)
    {
      pthread_cond_wait(&conditionvar, &mutex);
      if(flag==1)
      {
          pthread_mutex_unlock(&mutex);
          pthread_exit(NULL);
      }
    }
    int fdreturned = buff.front();
    buff.pop();
    count--;
    pthread_cond_signal(&conditionvariable2);
    pthread_mutex_unlock(&mutex);
    //while (1)
    {

      bzero(buffer, 1024);
      n = read(fdreturned, buffer, 1024);
      // cout<<"Hi";
      if(n<=0){
        perror("ERROR reading from socket");
        }
      /* send reply to client */
      else {
      HTTP_Response *response = handle_request(buffer);
      n = write(fdreturned, (response->get_string()).c_str(), response->get_string().length());
      if (n < 0)
        perror("ERROR writing to socket");
      delete response;
      }
      close(fdreturned);
    }
  
  }
}

int main(int argc, char *argv[])
{ 
  int sockfd, newsockfd, portno;
  //int array[1024];
  int last = 0, start = 0, count = 0;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  int arr[50];
  pthread_t thread_id;
  signal(SIGINT,handle_sigint);
  if (argc < 2)
  {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  /* create socket */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");
  /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
   */
   
  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind socket to this port number on this machine */

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");

  /* listen for incoming connection requests */

  listen(sockfd, 10000);
  clilen = sizeof(cli_addr);
  for (int i = 0; i < 20; i++)
  {
    arr[i] = i;
    pthread_create(&threadarray[i], NULL, startfunction, (void *)(arr + i));
    // threadarray[i]=thread_id;
  }
  /* read message from client */
  while (1)
  {
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
      error("ERROR on accept");
    producer(newsockfd);


  }
  return 0;
}

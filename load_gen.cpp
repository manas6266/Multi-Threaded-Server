/* run using: ./load_gen localhost <server port> <number of concurrent users>
   <think time (in s)> <test duration (in s)> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<iostream>
#include <netdb.h>

#include <pthread.h>
#include <sys/time.h>
using namespace std;
int time_up;
FILE *log_file;
string url[] = {"GET / HTTP/1.1", "GET /apart1 HTTP/1.1", "GET /apart2 HTTP/1.1", "GET /apart3 HTTP/1.1",
                "GET /apart1/flat11 HTTP/1.1", "GET /apart1/flat12 HTTP/1.1",
                "GET /apart2/flat21 HTTP/1.1", "GET /apart3/flat31 HTTP/1.1", "GET /apart3/flat32 HTTP/1.1"};
// user info struct
struct user_info {
  // user id
  int id;

  // socket info
  int portno;
  char *hostname;
  float think_time;

  // user metrics
  int total_count;
  float total_rtt;
};

// error handling function
void error(char *msg) {
  perror(msg);
  exit(0);
}

// time diff in seconds
float time_diff(struct timeval *t2, struct timeval *t1) {
  return (t2->tv_sec - t1->tv_sec) + (t2->tv_usec - t1->tv_usec) / 1e6;
}

// user thread function
void *user_function(void *arg) {
  /* get user info */
  struct user_info *info = (struct user_info *)arg;

  int sockfd, n;
  char buffer[1024];
  struct timeval start, end;

  struct sockaddr_in serv_addr;
  struct hostent *server;

  while (1) {
    /* start timer */
    gettimeofday(&start, NULL);
    /* TODO: create socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
    error("ERROR opening socket");
    }
    /* TODO: set server attrs */
    /* TODO: connect to server */
    server = gethostbyname(info->hostname);
    if (server == NULL)
    {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(info->portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting\n");
        continue;
    }

        bzero(buffer, sizeof(buffer));
        int rnum = rand() % 9;
     
        //fgets(buffer, 1023, stdin);
        strcpy(buffer,url[rnum].c_str());
        n = write(sockfd, buffer, strlen(buffer));

        if (n < 0)
        {
          perror("ERROR writing to socket\n");
          continue;
        }

        bzero(buffer, sizeof(buffer));

        /* read reply from server */
        
        n = read(sockfd, buffer, 1023);
        if (n < 0)
          {
              perror("ERROR reading from socket\n");
              continue;
          }
    close(sockfd);
    /* end timer */
    gettimeofday(&end, NULL);

    /* if time up, break */
    if (time_up)
      break;

    /* TODO: update user metrics */
    info->total_count=info->total_count+1;
    info->total_rtt+=time_diff(&end,&start);
    /* TODO: sleep for think time */
    usleep(info->think_time * 1000000);
  }

  /* exit thread */
  fprintf(log_file, "User #%d finished\n", info->id);
  fflush(log_file);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  int user_count, portno, test_duration;
  float think_time;
  char *hostname;

  if (argc != 6) {
    fprintf(stderr,
            "Usage: %s <hostname> <server port> <number of concurrent users> "
            "<think time (in s)> <test duration (in s)>\n",
            argv[0]);
    exit(0);
  }

  hostname = argv[1];
  portno = atoi(argv[2]);
  user_count = atoi(argv[3]);
  think_time = atof(argv[4]);
  test_duration = atoi(argv[5]);

  printf("Hostname: %s\n", hostname);
  printf("Port: %d\n", portno);
  printf("User Count: %d\n", user_count);
  printf("Think Time: %f s\n", think_time);
  printf("Test Duration: %d s\n", test_duration);

  /* open log file */
  log_file = fopen("load_gen.log", "w");

  pthread_t threads[user_count];
  struct user_info info[user_count];
  struct timeval start, end;

  /* start timer */
  gettimeofday(&start, NULL);
  time_up = 0;
  for (int i = 0; i < user_count; ++i) {
    /* TODO: initialize user info */
      info[i].id=i;
      info[i].portno=portno;
      info[i].hostname=hostname;
      info[i].think_time=think_time;
      info[i].total_count=0;
      info[i].total_rtt=0;
    /* TODO: create user thread */
      pthread_create(&threads[i],NULL,user_function,(void *)(info+i));
    fprintf(log_file, "Created thread %d\n", i);
  }
  
  /* TODO: wait for test duration */
  sleep(test_duration);
  fprintf(log_file, "Woke up\n");

  /* end timer */
  time_up = 1;
  gettimeofday(&end, NULL);

  /* TODO: wait for all threads to finish */
 for(int i=0;i<user_count;i++)
 {
   pthread_join(threads[i],NULL);
 }
  /* TODO: print results */
  float rtt=0;
  int total=0;
  int responsetime=0;
  for(int i=0;i<user_count;i++){
    total+=info[i].total_count;
    rtt+=info[i].total_rtt;
  }
  printf("Thruput: %d\n",total/test_duration);
  printf("Response time: %f\n",rtt/total);

  /* close log file */
  fclose(log_file);

  return 0;
}
#include <stdio.h>
#include "operations.h"
#include "threadsOps.h"
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>


void sigchld_handler(int sig);

int main(int argc,char *argv[]) {
  //read the arguments with some error checking
  if (argc != 7) {
    perror_exit("Wrong input.Give port number pool size and queue size\n");
  }
  int port,threadcount,size,i=0;
  int flagsread[3];
  for (i = 0;i < 3; ++i)
        flagsread[i] = 0;
  i = 0;

  while (i < argc) {
    if (!strcmp(argv[i],"-q")) {
      size = atoi(argv[i + 1]);
      flagsread[0] = 1;
    }
    if (!strcmp(argv[i],"-p")) {
      port = atoi(argv[i + 1]);
      flagsread[1] = 1;
    }
    if (!strcmp(argv[i],"-s")) {
      threadcount = atoi(argv[i + 1]);
      flagsread[2] = 1;
    }
    ++i;
  }

  for (i = 0; i < 3;++i) {
      if (flagsread[i] == 0) {
        perror_exit("You didn't call the function with the right arguments"); 
      }
  }


  //a hashtable is just an array of hashlists here.According to the hashfunction we put each account
  //to the corresponding AccountList.
  AccountList Hashtable[10];
  for (i = 0;i < 10;++i)
    Hashtable[i] = AccountListCreate();
  int sock,newsock;
  struct sockaddr_in server,client;
  socklen_t clientlen;
  struct sockaddr *serverptr=(struct sockaddr *)&server;
  struct sockaddr *clientptr = (struct sockaddr *)&client;
  struct hostent *rem;


  initialize(&pool,size);
  pthread_mutex_init(&mtx,0);
  pthread_cond_init(&cond_nonempty,0);
  pthread_cond_init(&cond_nonfull,0);
  pthread_t *threadids = malloc(sizeof(pthread_t) * threadcount);
  for (i = 0;i < threadcount;++i)
    pthread_create(&threadids[i],0,workfunction,(void *) Hashtable);
  for (i = 0;i < 10;++i)
    pthread_mutex_init(&mutexes[i],0);
  signal(SIGCHLD,sigchld_handler);  //Reap dead chilidern asynchronously

  if ((sock = socket(PF_INET,SOCK_STREAM,0)) < 0)  //Socket creation
    perror_exit("error in socket creation");
  server.sin_family = AF_INET;                       //Internet domain
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(port);
  if (bind(sock,serverptr,sizeof(server)) < 0)     //bind socket to the wanted address
    perror_exit("error in binding");
  if (listen(sock,5) < 0) 
    perror_exit("error in listening");
  printf("Listening for connections to port %d\n",port);
  

//wait for connection from clients.
  while (1) {
    if ((newsock = accept(sock,clientptr,&clientlen)) < 0)
      perror_exit("accept");
    printf("Accepted connections\n");
    place(&pool,newsock);
    pthread_cond_signal(&cond_nonempty);
  }

  for (i = 0; i < threadcount; i++)
    pthread_join(threadids[i], 0);
  pthread_cond_destroy(&cond_nonempty);
  pthread_cond_destroy(&cond_nonfull);
  pthread_mutex_destroy(&mtx);
  for (i = 0; i < 10; i++)
    pthread_mutex_destroy(&mutexes[i]);
  for (i = 0; i < 10; ++i)
    AccountListDestroy(Hashtable[i]);

  return 0;
}



void sigchld_handler (int sig) {
  while (waitpid(-1,NULL,WNOHANG) > 0);
}


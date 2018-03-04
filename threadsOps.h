#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>	     /* sockets */
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	         /* gethostbyaddr */
#include <stdlib.h>	         /* exit */
#include <signal.h>          /* signal */
#include <pthread.h>
#include <unistd.h>

typedef struct {
  int *data;
  int start;
  int end;
  int count;
  int size;
}pool_t;

pthread_mutex_t mtx;
pthread_mutex_t mutexes[10];
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
pool_t pool;

void perror_exit(char *message);
void initialize(pool_t *pool,int size);
void place(pool_t *pool,int data);
int obtain(pool_t *pool);
void *workfunction(void *t);
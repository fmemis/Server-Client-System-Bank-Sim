#include "operations.h"
#include "threadsOps.h"

#define BUFFSIZE 256

void perror_exit(char *message) {
  perror(message);
  exit(EXIT_FAILURE);
}


//We will store the socket descriptor in the pool(practically a queue)
void initialize(pool_t *pool,int size) {
  pool->start = 0;
  pool->end = -1;
  pool->count = 0;
  pool->data = malloc(sizeof(int) * size);
  pool->size = size;
}

void place(pool_t *pool,int data) {
  pthread_mutex_lock(&mtx);
  while (pool->count >= pool->size) {
	printf("The queue is full\n");
	pthread_cond_wait(&cond_nonfull,&mtx);
  }
  pool->end = (pool->end + 1) % pool->size;
  pool->data[pool->end] = data;
  pool->count++;
  pthread_mutex_unlock(&mtx);
}

int obtain(pool_t *pool) {
  int data = 0;
  pthread_mutex_lock(&mtx);
  while (pool->count <= 0) {
	//printf("Queue is empty\n");
	pthread_cond_wait(&cond_nonempty,&mtx);
  }
  data = pool->data[pool->start];
  pool->start = (pool->start + 1) % pool->size;
  pool->count--;
  pthread_mutex_unlock(&mtx);
  return data;
}

//The threadfunction.It takes a socket descriptor from the pool and starts reading and executing commands
void *workfunction(void *t) {
  char buffer[BUFFSIZE];
  AccountList *Hashtable = (AccountList *) t;
  int newsock;
  //the thread will run forever.It will continuously try to take a socket from the pool to do its work
  //if no socket exists,it will wait until one is placed in the pool.
  while (1) {   

	int position,position2;
	char trash[20];
	float amount;

	newsock = obtain(&pool);
	pthread_cond_signal(&cond_nonfull); //send a signal that a new socket descriptor can be placed in the pool

	while (read(newsock,buffer,sizeof(buffer)) > 0) {

	  if (!strncmp(buffer,"add_account",11)) {
		char name[40];
		sscanf(buffer,"%s %f %s",trash,&amount,name); //we don't really need the first string from buffer.Hence we use the variable trash
		position = HashFunction(name); //find in which list of the hashtable, we will save the account
		pthread_mutex_lock(&mutexes[position]); //no other thread can access that place.We avoid race conditions
		if (!AddAccount(amount,name,Hashtable))
		  sprintf(buffer,"Success. Account creation (%s:%f)\n",name,amount);
		else
		  sprintf(buffer,"Error. Account creation failed (%s:%f)\n",name,amount);
		pthread_mutex_unlock(&mutexes[position]); //allow other threads to access that position since we finished
		if ((write(newsock,buffer,BUFFSIZE)) < 0)
		  perror_exit("Problem in writing");
	  }

	  else if (!strncmp(buffer,"print_balance",13)) {
		char name[40];
		sscanf(buffer,"%s %s",trash,name);
		position = HashFunction(name);
		pthread_mutex_lock(&mutexes[position]);
		float balance;
		if (!PrintBalance(name,Hashtable,&balance))
		  sprintf(buffer,"Success. Balance (%s:%f)\n",name,balance);
		else
		  sprintf(buffer,"Error. Balance (%s)\n",name);
		pthread_mutex_unlock(&mutexes[position]);
		if ((write(newsock,buffer,BUFFSIZE)) < 0)
		  perror_exit("Problem in writing");
	  }

	  else if (!strncmp(buffer,"add_transfer",12)) {
		char source[40],dest[40];
		sscanf(buffer,"%s %f %s %s",trash,&amount,source,dest);
		position = HashFunction(source);
		position2 = HashFunction(dest); //we need to forbid access to other threads in a second position of the hashtable in order to make a secure transfer.
		pthread_mutex_lock(&mutexes[position]);
		if (position != position2) { /*make sure that source and destination are not in the same list in our hashtable.
		 	if we try to lock the same mutex a deadlock will occur!!!!*/
		  pthread_mutex_lock(&mutexes[position2]);
		}
		if (!AddTransfer(amount,source,dest,Hashtable))
		  sprintf(buffer,"Success.  Transfer addition (%s:%s:%f)\n",source,dest,amount);
		else
		  sprintf(buffer,"Error. Transfer addition failed (%s:%s:%f)\n",source,dest,amount);
		pthread_mutex_unlock(&mutexes[position]);
		if (position != position2)
		  pthread_mutex_unlock(&mutexes[position2]);
		if ((write(newsock,buffer,BUFFSIZE)) < 0)
		  perror_exit("Problem in writing");
	  }

	  else if (!strncmp(buffer,"print_multi_balance",19)) {
		char *names[100];
		int j,flag = 0,error;
		float balances[100];
		//an array for all the names(up to 100 obviously)
		for (j = 0;j < 100;++j)
		  names[j] = malloc(40);
		int k = 20,i = 0;
		j = 0;
		//put all the account names in an array
		while (1) {
		  while ((buffer[k] != ' ') && (buffer[k] != '\n') && (buffer[k] != '\0')) {
			names[j][i] = buffer[k];
			++k;++i;
		  }
		  names[j][i] = '\0';
		  ++j;
		  i = 0;
		  if (buffer[k] == '\n' || buffer[k] == '\0')
			break;
		  ++k;
		}

		//find the balances for the asked account names
		int counter = j;
		for (j = 0;j < counter;++j) {
		  //lock just a position each time.Makes no sense to lock all demanded account positions since we do not take the balances concurrently.We would just waste resources
		  position = HashFunction(names[j]);
		  pthread_mutex_lock(&mutexes[position]);
		  if ((error = PrintBalance(names[j],Hashtable,&amount)) == 0) {
			balances[j] = amount;
		  }
		  else 
			flag = 1;
		  pthread_mutex_unlock(&mutexes[position]);
		  if (flag)
			break;
		}
		if (!flag) {
		  sprintf(buffer,"%d",counter);
		  if ((write(newsock,buffer,BUFFSIZE)) < 0)
			perror_exit("Problem in writing");
		  sprintf(buffer,"Success. Multi balance (");
		  if ((write(newsock,buffer,BUFFSIZE)) < 0)
			perror_exit("Problem in writing");
		  for (j = 0;j < counter;++j) {
			sprintf(buffer,"%s/%f:",names[j],balances[j]);
			if ((write(newsock,buffer,BUFFSIZE)) < 0)
			  perror_exit("Problem in writing");
		  }
		  sprintf(buffer,")\n");
		  if ((write(newsock,buffer,BUFFSIZE)) < 0)
			perror_exit("Problem in writing");
		}
		else {
		  sprintf(buffer,"Error. Multi-Balance (");
		  if ((write(newsock,buffer,BUFFSIZE)) < 0)
			perror_exit("Problem in writing");
		  for (j = 0;j < counter;++j) {
			sprintf(buffer,"%s:",names[j]);
			if ((write(newsock,buffer,BUFFSIZE)) < 0)
			  perror_exit("Problem in writing");
		  }
		  sprintf(buffer,")\n");
		  if ((write(newsock,buffer,BUFFSIZE)) < 0)
			perror_exit("Problem in writing");
		}
		for (j = 0;j < 100;++j)
		  free(names[j]);

	  }

	  else if (!strncmp(buffer,"add_multi_transfer",18)) {
		int counter = 0;
		char *names[100];
		int j,error,flag = 0;

		sscanf(buffer,"%s %f",trash,&amount);
		
		for (j = 0;j < 100;++j)
		  names[j] = malloc(40);
		j = 0;
		int k = 20,i = 0;
		int count = 20;
		//put all the names in an array
		while (1) {
		  while ((buffer[k] != ' ') && (buffer[k] != '\n') && (buffer[k] != '\0')) {
			names[j][i] = buffer[k];
			++k;++i;++count;
		  }
		  names[j][i] = '\0';
		  ++count;
		  ++j;
		  i = 0;
		  if (buffer[k] == '\n' || buffer[k] == '\0')
			break;
		  ++k;
		}
		
		/*find the balances for asked account names.The account in the first cell of the array
		is the one that gives the money to the accounts in the next cells.*/
		/*we now need restrict access to 2 positions of the hashtable.The first is where the 
		source account is(it remains locked for all the loop).The second changes in each iteration
		and it is the position of the current destination account.When we finish sending money to this
		account we unlock the mutex guarding its position.When we finish sending money to all the accounts,
		we unlock the mutex guarding the source account's position.*/
		position = HashFunction(names[1]);
		pthread_mutex_lock(&mutexes[position]);
		counter = j;
		for (j = 2;j < counter;++j) {
		  position2 = HashFunction(names[j]);
		  if (position !=  position2) { /*make sure that source and destination are not in the same list in our hashtable.
		 	if we try to lock the same mutex a deadlock will occur!!!!*/
			pthread_mutex_lock(&mutexes[position2]);
		  }
		  if ((error = AddTransfer(amount,names[1],names[j],Hashtable)) == 0);
		  else {
			flag = 1;
		  }
		  if (position != position2)
			pthread_mutex_unlock(&mutexes[position2]);
		  if (flag)
			break;
		}
		if (flag) {
		  sprintf(buffer,"Error.  Multi-Transfer addition failed (%s:%f)\n",names[1],amount);
		  if ((write(newsock,buffer,BUFFSIZE)) < 0)
			perror_exit("Problem in writing");
		}
		else {
		  sprintf(buffer,"Multi-Transfer addition (%s:%f)\n",names[1],amount);
		  if ((write(newsock,buffer,BUFFSIZE)) < 0)
			perror_exit("Problem in writing");
		}
		pthread_mutex_unlock(&mutexes[position]);
		for (j = 0;j < 100;++j)
		  free(names[j]);
	  }

	  else
		printf("Error. Unknown command\n");
	}
  close(newsock);
  }
}


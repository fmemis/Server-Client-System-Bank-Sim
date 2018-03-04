OBJS = server.o operations.o structs.o threadsOps.o
OBJS2 = client.o
SOURCE = server.c client.c operations.c structs.c threadsOps.c
HEADER = operations.h structs.h threadsOps.h
OUT = bankserver bankclient
CC = gcc
FLAGS = -c

all: bankserver bankclient

server.o: server.c
	$(CC) $(FLAGS) server.c

client.o: client.c
	$(CC) $(FLAGS) client.c

operations.o: operations.c
	$(CC) $(FLAGS) operations.c

structs.o: structs.c
	$(CC) $(FLAGS) structs.c

threadsOps.o: threadsOps.c
	$(CC) $(FLAGS) threadsOps.c

bankserver: $(OBJS)
	$(CC) $(OBJS) -o bankserver -lpthread
	rm $(OBJS)

bankclient: $(OBJS2)
	$(CC) $(OBJS2) -o bankclient
	rm $(OBJS2)

clean:
	rm -f $(OBJS) $(OBJS2) $(OUT)


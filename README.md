# Server-Client-System-Bank-Sim
A System Programming project in which a bank/server client system is built(Monitor Style - Networking Programming)

##Project Description
In this project we have two main programs that the user executes.A bankserver and a bankclient.

  ###Bankserver
    The program which "manages" the bank.It initializes a thread pool for the worker threads, a connection queue(pool) for the
    clients who want to connect with the bank server,a hashtable to store bank accounts and the neccessary mutexes and condition
    variables.
    The program's master thread waits for connections from clients in a specified port(command line argument).When it receives such
    a connection,it places it(it's socket descriptor) in the connection queue(pool).When a worker thread has finished with its 
    prior work,it waits to obtain a connection(socket descriptor) from the connection queue(The connection queue could be empty so
    it waits until a socket descriptor is placed in it).Worker threads receive these socket descriptors in order to communicate
    with clients and execute their instructions.They send the results back the same way they received the instructions.Through 
    the socket.
    
  ###Bankclient
    The banclient program connects with the bank server in a port and an address specified from the command line.Then it reads
    commands from stdin or a file and sends them to the server for execution.It prints the results it receives from the server.
    
  ###WARNING
  Multiple bankclient programs can run concurrently(a bank has many clients) but there is only one bankserver.

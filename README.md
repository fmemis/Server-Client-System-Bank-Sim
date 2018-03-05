# Server-Client-System-Bank-Sim
A System Programming project in which a bank/server client system is built(Monitor Style - Networking Programming)

## Project Description
In this project we have two main programs that the user executes.A bankserver and a bankclient.

  ### Bankserver
    The program which "manages" the bank.It initializes a thread pool for the worker threads, a connection
    queue(pool) for the clients who want to connect with the bank server,a hashtable to store bank accounts
    and the neccessary mutexes and condition variables. The program's master thread waits for connections 
    from clients in a specified port(command line argument).When it receives such a connection,it places it
    (it's socket descriptor) in the connection queue(pool).When a worker thread has finished with its prior 
    work,it waits to obtain a connection(socket descriptor) from the connection queue(The connection queue could 
    be empty so it waits until a socket descriptor is placed in it).Worker threads receive these socket 
    descriptors in order to communicate with clients and execute their instructions.They send the results back 
    the same way they received the instructions.Through the socket.
    
  ### Bankclient
    The banclient program connects with the bank server in a port and an address specified from the command line.
    Then it reads commands from stdin or a file and sends them to the server for execution.It prints the results 
    it receives from the server.
    
  ### WARNING
  Multiple bankclient programs can run concurrently(a bank has many clients) but there is only one bankserver.
  
  ## Compile and execute instructions
  
  To compile just run make.
  
  To execute bankserver: ./bankserver -p <port> -s <thread_pool_size> -q <queue_size>
  Example: ./bankserver -p 8080 -s 20 -q 30
  
  To execute client: ./bankclient -h <server_host> -p <server_port> -i <command_file>
  Example: ./bankclient -h fotis-computer -p 8080 -i stdin
  
  These are the commands that can be given to the bankclient.
  - add_account <init_amount> name
  
    Create a bank account with an initial amount of money
    
  - add_transfer <amount> <src_name> <dst_name>
  
    Transfer an amount of money from src_name account to dst_name account.
    
  - add_multi_transfer <amount> <src_name> <dst_name1> <dst_name2> ...(up to a 100)
  
    Transfer an amount of money from src_name account to multiple accounts.
    
  - print_balance <name>
  
    Print account balance
    
  - print_multi_balance <name1> <name2> ...
  
    Print multiple accounts balances.
    
  - sleep <time>
  
    The client program will sleep for x secs.
    
  - exit
  
  ## Notes
  
  This project demonstrates the use of hashtables,threads,sockets,mutexes and condition variables which are used 
  to make a bank server-client system where multiple clients can run concurrently,quick safe access to the accounts
  is achieved and race conditions and deadlocks are avoided.
  

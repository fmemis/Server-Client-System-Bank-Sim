#include <sys/types.h>                         /* sockets */
#include <sys/socket.h>                        /* sockets */
#include <netinet/in.h>               /* Internet sockets */
#include <netdb.h>                       /* gethostbyname */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFSIZE 256

void perror_exit(char *message);

void main(int argc, char *argv[]) {
   //read the arguments with some error checking
   if (argc != 7) {
      perror_exit("Wrong input.Give hostname,port number and filename(stdin for keyboard)\n");
   }
  int i = 0,port;
  char hostname[40],filename[40];
  int flagsread[3];
  for (i = 0;i < 3; ++i)
        flagsread[i] = 0;
  i = 0;

  while (i < argc) {
    if (!strcmp(argv[i],"-h")) {
      strcpy(hostname,argv[i + 1]);
      flagsread[0] = 1;
    }
    if (!strcmp(argv[i],"-p")) {
      port = atoi(argv[i + 1]);
      flagsread[1] = 1;
    }
    if (!strcmp(argv[i],"-i")) {
      strcpy(filename,argv[i + 1]);
      flagsread[2] = 1;
    }
    ++i;
   }

   for (i = 0; i < 3;++i) {
      if (flagsread[i] == 0) {
        perror_exit("You didn't call the function with the right arguments"); 
      }
   }



   FILE *in;
   if (!strcmp(filename,"stdin"))
      in = stdin;
   else {
      if ((in = fopen(filename,"r")) == NULL)
         perror_exit("Cannot open file");
   }
   printf("host is %s\n",hostname);


   int sock;struct hostent *rem;
   char buf[BUFFSIZE];
   struct sockaddr_in server;
   struct sockaddr *serverptr = (struct sockaddr *) &server;
   //Socket creation
   if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      perror_exit("error in socket creation");  
   }
   //Find server's IP address 
   if ((rem = gethostbyname(hostname)) == NULL) {
      herror("gethostbyname"); exit(1); 
   }
   
   server.sin_family = AF_INET;        // Internet domain 
   memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
   server.sin_port = htons(port);
   if (connect (sock,serverptr,sizeof(server)) < 0)
      perror_exit("connect");
   printf("Connecting to %s port %d\n",hostname,port);
  
   while (1) {
      
      char text[100];
      fgets(text,99,in);
      /*if (text[strlen(text) -1] == '\n')
         text[strlen(text) - 1] = '\0';*/
      char command[50];
      sscanf(text,"%s",command);
      
      //send the commands to the server to be executed.First do a very light argument check
      if (!strcmp(command,"add_account")) {
         if (strlen(text) > 15) {
            strcpy(buf,text);
            if ((write(sock,buf,BUFFSIZE)) < 0)
               perror_exit("Problem in writing");
            if ((read(sock,buf,BUFFSIZE)) < 0)
               perror_exit("Problem in reading");
            printf("%s",buf);
         }
         else
            printf("You have not given enough arguments for add_account operation\n");
      }
      else if (!strcmp(command,"print_balance")) {
         if (strlen(text) > 15) {
            strcpy(buf,text);
            if ((write(sock,buf,BUFFSIZE)) < 0)
               perror_exit("Problem in writing");
            if ((read(sock,buf,BUFFSIZE)) < 0)
               perror_exit("Problem in reading");
            printf("%s",buf);
         }
         else
            printf("You have not given enough arguments for print_balance operation\n");
      }
      else if (!strcmp(command,"add_transfer")) {
         if (strlen(text) > 18) {
            strcpy(buf,text);
            if ((write(sock,buf,BUFFSIZE)) < 0)
               perror_exit("Problem in writing");
            if ((read(sock,buf,BUFFSIZE)) < 0)
               perror_exit("Problem in reading");
            printf("%s",buf);
         }
         else
            printf("You have not given enough arguments for add_transaction operation\n");
      }
       else if (!strcmp(command,"print_multi_balance")) {
         if (strlen(text) > 23) {
            strcpy(buf,text);
            if ((write(sock,buf,BUFFSIZE)) < 0)
               perror_exit("Problem in writing");
            if ((read(sock,buf,BUFFSIZE)) < 0)
               perror_exit("Problem in reading");
            int i,count;
            sscanf(buf,"%d",&count);
            count += 2;
            for (i = 0;i < count;++i) {
               if ((read(sock,buf,BUFFSIZE)) < 0)
                  perror_exit("Problem in reading");
               printf("%s",buf);
            }
         }
         else
            printf("You have not given enough arguments for print_multi_balance operation\n");
      }
      else if (!strcmp(command,"add_multi_transfer")) {
         if (strlen(text) > 27) {
            strcpy(buf,text);
            if ((write(sock,buf,BUFFSIZE)) < 0)
               perror_exit("Problem in writing");
            if ((read(sock,buf,BUFFSIZE)) < 0)
                  perror_exit("Problem in reading");
               printf("%s",buf);
         }
         else
            printf("You have not given enough arguments for print_multi_balance operation\n");
      }
      else if (!strcmp(command,"sleep")) {
         int stime;
         if (strlen(text) > 6) {
            sscanf(text,"%s %d",command,&stime);
            sleep(stime);
         }
         else
            printf("You have not given enough arguments for sleep operation\n");
      }

      else if (!strcmp(command,"exit")) {
         printf("Client is exiting\n");
         break;
      }
   }
   
   close(sock);
   fclose(in);
}

void perror_exit(char *message) {
   perror(message);
   exit(EXIT_FAILURE);
}
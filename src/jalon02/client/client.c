#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include "../commons/contrib.h"
#include <sys/select.h>
#include <resolv.h>
#include "../commons/network.h"
#include "client.h"
#include "server.h"
#include <sys/select.h>
#include <time.h>


//Initialization of socket parameters
int type=SOCK_STREAM;
int protocol=IPPROTO_TCP;
int family=AF_INET;

//Initialization of the address
struct sockaddr_in set_addr_info(const char* addr, int family, int port) {
  struct sockaddr_in SOCKADDR_IN;
  inet_aton(addr,&SOCKADDR_IN.sin_addr);
  SOCKADDR_IN.sin_family=family;
  SOCKADDR_IN.sin_port=htons(port);
  return SOCKADDR_IN;
}

// Initialisation of the buffer
int size_buffer=200;

int main(int argc,char** argv) {

  if (argc != 3)
    {
      fprintf(stderr,"usage: RE216_CLIENT hostname port.\n");
      return 1;
    } 

  //Initialisation of address (with first argument for the addr and 2nd argument for the port)
  struct sockaddr_in SOCKADDR_IN;
  char *addr=argv[1];
  int port=atoi(argv[2]);
  SOCKADDR_IN=set_addr_info(addr,AF_INET,port);

  // Creation of the socket
  int socket;
  socket=do_socket(family,type,protocol);

  //Defining parameters of the select function
  fd_set my_fd_set;
  fd_set fd_temp;
  FD_ZERO(&my_fd_set);
  FD_SET(socket,&my_fd_set);
  FD_SET(fileno(stdin),&my_fd_set);
  int max_fd=socket+1;
  
  // Connection of the socket
  do_connect(socket,(struct sockaddr *)&SOCKADDR_IN,sizeof(SOCKADDR_IN));
  printf("Connected by the adress %s at the port n°%s.\n", argv[1], argv[2]);

  //Infinite loop in order to can select new client
  for(;;) {

    fd_temp=my_fd_set;
    int tempo;
    tempo=select(max_fd,&fd_temp,NULL,NULL,NULL);
    
    //Give priority to writing or reading right
    if(FD_ISSET(fileno(stdin),&fd_temp)) {
      
      //In case of sending message
      char msg[size_buffer];
      bzero(msg,size_buffer);
      fgets(msg,size_buffer,stdin);
      do_send_client(socket,msg,size_buffer);
      
      //Exit the chat by writing "/quit"
      if (strncmp(msg, "/quit\n",6) == 0) {
	break;
      }
    } else if(FD_ISSET(socket,&fd_temp)) {
      
      //In case of receiving message
      char msg_back[size_buffer];
      int tempo_recvc=recv(socket,msg_back,size_buffer,0);
      
      if (tempo_recvc != -1) {
	printf("%s\n", msg_back);
      } else {
	printf("ERROR : An error occured while receiving the message.\n");
	perror("recv"); exit(EXIT_FAILURE);
      }    
    }
  }
  
  // To close the connection with the server
  close(socket);
  printf("The Connection with the server is closed now. See you later!\n ");
  return 0;
  
}

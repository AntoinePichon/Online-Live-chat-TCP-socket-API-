#include <stdio.h>
#include <regex.h>
#include <resolv.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include "user.h"
#include "callbacks.h"
#include "network.h"


//do_socket()
int do_socket(int domain, int type, int protocol) {
  int sock;
  sock = socket(domain, type, protocol);
  
  if (sock != -1) {
    printf("Creating socket...\n");
    printf("Socket has been created succesfully!\n");
  } else {
    printf("ERROR : An error occured while creating the socket.\n");
        perror("socket"); exit(EXIT_FAILURE);
  }
  
  return sock;
}

//do_connect()
void do_connect(int socket, const struct sockaddr *addr, socklen_t addrlen) {
  
  int res;
  printf("Connecting to the server...\n");
  res = connect(socket, addr, addrlen);
  if (res != -1)
    printf("Connection succesfull, you are connected now!\n");
  else if (res == -1) {
    printf("ERROR : An error occured while connecting to the server.\n");
    perror("connect");exit(EXIT_FAILURE);
    }
}

//do_bind()
void do_bind(int sock, struct sockaddr_in serv_addr, int addrlen){
  
  printf("Waiting for binding...\n");
  
  int resb = bind(sock,  (struct sockaddr *)&serv_addr, addrlen);
  
  if(resb != -1) {
    printf("Binding realized successfully!\n");
    printf("Listening to any client...\n");
  } 
  else {
    printf("ERROR : An error occured while binding.\n");
    perror("bind"); exit(EXIT_FAILURE);
    
  }
}

//do_accept()
int do_accept(int sock, struct sockaddr_in client_addr,int addrlen){
  
  
  int resa = accept(sock,(struct sockaddr *)&client_addr,(socklen_t *)&addrlen);
  
  if ( resa == -1) {
    printf("ERROR : An error occured while accepting phase.\n");
    perror("accept"); exit(EXIT_FAILURE);
  }
  
  return resa;
  
}

//do_send_server
void do_send_server (int sock, char buffer[], int size_buffer) {
  
  int tempo_send;
  tempo_send = send(sock, buffer, size_buffer, 0);
  
  if (tempo_send == -1){
    printf("ERROR : An error occured while sending message.\n");
    perror("send"); exit(EXIT_FAILURE);
  }
  
}

//do_send_client
void do_send_client (int sock, char buffer[], int size_buffer) {
  
  int tempo_send;
  tempo_send = send(sock, buffer, size_buffer, 0);
  
  if (tempo_send == -1){
    printf("ERROR : An error occured while sending message.\n");
    perror("send"); exit(EXIT_FAILURE);
  }
}

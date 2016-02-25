/////////////////////////////////////////////////////////////////////////////////////////
//BORDEAUX INP ENSEIRB-MATMECA
//DEPARTEMENT TELECOM
//RE216 PROGRAMMATION RESEAUX
//{daniel.negru,joachim.bruneau_-_queyreix,nicolas.herbaut}@ipb.fr
////////////////////////////////////////////////////////////////////////////////////////

#ifndef SERVER_H_SDFGEQHAEHZEGFR
#define SERVER_H_SDFGEQHAEHZEGFR

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <user.h>



/* Initialize a new socket and controlling for error */
int do_socket(int family, int type, int protocol);

/* Perform the binding */
void do_bind(int sock, struct sockaddr_in serv_addr, int addrlen);

/* Accept connections from the clients */
int do_accept(int sock, struct sockaddr_in client_addr,int addrlen);

/* Connect the socket to the address */
void do_connect(int sock, const struct sockaddr *addr, socklen_t addrlen);

/* Sending message management */
void do_send_server (int sock, char buffer[], int size_buffer);
void do_send_client (int sock, char buffer[], int size_buffer);



#endif //SERVER_H_SDFGEQHAEHZEGFR

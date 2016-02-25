#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include "lines.h"

int main(int argc,char** argv) {

    int s;
    struct addrinfo hints, *res, *p;
    int status;
    int ret;
    char ipstr[INET6_ADDRSTRLEN];
    char server_input[MSG_SIZE];
    char user_input[MSG_SIZE];
    size_t count_char;
    ssize_t sent;

    memset(server_input, 0, MSG_SIZE);
    memset(user_input, 0, MSG_SIZE);

    if (argc != 3) {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if ((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    printf("IP addresses for %s:\n", argv[1]);
//
    for(p = res; p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
//
        //get the socket
        s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

        //connect to remote socket
        if(connect(s, res->ai_addr, res->ai_addrlen)!=0) {
            perror("connect error");


            return 2;
        } else {


            while (1)
            {
                printf("\nPlease enter your line:");
		fflush(stdout);

                //get user input from stdin
                read_line(STDIN_FILENO, user_input, MSG_SIZE);

                //send user input to server
		send_line(s, user_input, strlen(user_input));

                //receive feedback from server
                memset(server_input, '\0', MSG_SIZE);
                read_line(s, server_input, MSG_SIZE);

                fprintf(stdout,"message from server:");
		fflush(stdout);
		display_line(server_input, strlen(server_input));

		if (strcmp(server_input, "quit\n") == 0)
			break;
            }
        }
    }

	close(s);
	freeaddrinfo(res); // free the linked list

    return 0;


}

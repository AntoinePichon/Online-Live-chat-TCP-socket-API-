#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "lines.h"

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[MSG_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int yes = 1;
    int status;

    //create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //check for socket validity
    if (sockfd < 0)
        error("ERROR opening socket");

    //Reusing already in use adresses
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		perror("Error");

    //clean the serv_add structure
    memset(&serv_addr, 0, sizeof(serv_addr));

    //cast the port from a string to a char*
    portno = atoi(argv[1]);

    //internet family protocol
    serv_addr.sin_family = AF_INET;

    //we bind to any ip form the host
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //we bind on the tcp port specified
    serv_addr.sin_port = htons(portno);

    //perform the binding
    if ((status = bind(sockfd, (struct sockaddr *) &serv_addr,
                       sizeof(serv_addr))) < 0) {
        perror("ERROR on binding");

    }

    //specify the socket to be a server socket and listen for at most 5 concurrent client
    listen(sockfd, 5);

    clilen = sizeof(cli_addr);

    for (;;) {

        //accept connection from client
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0)
            error("ERROR on accept");

        while (1)
        {

            //reset the buffer
            memset(buffer, 0, MSG_SIZE);

            //read what the client has to say
            n = read_line(newsockfd, buffer, MSG_SIZE);

            //a client -> server error occured, deal with it
            if (n < 0) {
                error("ERROR reading from socket");
                break;
            }

            //we write back to the client
            n = send_line(newsockfd, buffer, n);

            //a server -> client error occured, deal with it
            if (n < 0) {
                error("ERROR writing to socket");
                break;
            }

	    if (strcmp (buffer, "quit\n")==0) 
		break;
        }

        //clean up
        fsync(newsockfd);
        close(newsockfd);
    }

    close(sockfd);

    return 0;
}

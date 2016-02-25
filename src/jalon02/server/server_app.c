#include <resolv.h>
#include <string.h>
#include <sys/select.h>
#include <stdlib.h>
#include "../commons/network.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "user.h"
#include <time.h>

int main(int argc, char** argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }


  // Initialization of the server address
    struct sockaddr_in serv_addr;
    int port = atoi(argv[1]);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(port);


    //Creation of the socket
    int socket;
    socket=do_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //  Binding
    do_bind(socket,serv_addr,sizeof(serv_addr));

    // Specify the socket to be a server socket and listen for at most 20 concurrent client
    listen(socket,20);

    
    // Initialization of the select() parameters
    fd_set my_fd_set;
    fd_set fd_temp;
    FD_ZERO(&my_fd_set);
    FD_SET(socket,&my_fd_set); // adding the server socket to my_fd_set
    int max_fd = socket+1;

    //Initialization of the user list storing user profiles created after the accept() function.
    puser user_list = NULL;
    char* name_by_default;
    

    // Initialization of all the buffers and variables used in all the following options
    int size_buffer = 300;
    char* who_is_nick;
     
    //Variable used in for loop for browsing through the user list
    int k;

    /* Infinite loop */
    for (;;) 
    {
        fd_temp=my_fd_set;

        // select() is used to get the active sockets : Client or Server
        int select_temp;
        select_temp = select(max_fd,&fd_temp,NULL,NULL,NULL);

	int i;
   	    
        for(i=3;i<max_fd;i++) {
            if(FD_ISSET(i, &fd_temp)) {

                // New connections management
                if(i==socket) {

                    // Accepting connections from the differents clients
                    int new_sock_client;
                    new_sock_client = do_accept(socket,serv_addr,sizeof(serv_addr));
                    printf("CLIENT  %d IS NOW CONNECTED ON THE CHAT ! \n",new_sock_client-socket);

                    if (max_fd <= new_sock_client)
                        max_fd = new_sock_client +1;

                    FD_SET(new_sock_client,&my_fd_set); // Adding the new socket to my_fd_set

                    // Creating a new user profile 
                    struct user new_user;
                    strcpy(new_user.name,"Client");
                    new_user.socket_client = new_sock_client;
                    new_user.next_user = user_list;

                    // Connection Data Management 
                    char buffer[512];
                    char date_hour[40]; 

		    /* Date and hour management */
                    time_t timer_seconds = time(NULL);
                    struct tm * timeinfo = localtime(&timer_seconds);
                   
  
		    // /whois <nickname> displaying management
                    strftime(date_hour, sizeof(date_hour), "%d/%m/%Y@%H:%M", timeinfo); 
		    sprintf(buffer,"%s WITH IP ADDRESS 127.0.0.1 AND PORT %d \n",date_hour,port); 
                    strcpy(new_user.connect_data,buffer);

                    // Add a new user to the user_list
                    user_list = add_user(user_list,new_user);
                }

                // Client management
                else {

                    // Initialization of the buffer for incomming and outcomming information about any client 
                    char buffer_client[size_buffer];
                    bzero(buffer_client,size_buffer);
                    
                    // Get user information for the profile by the socket
                    puser user = get_user_by_socket(user_list,i);

                    // Read the sending message of the client

		    /*Receive the message from the socket by using recv */
		    int tempo_recvs=recv(i,buffer_client,size_buffer,0);


		    if (tempo_recvs != -1 && strncmp(buffer_client,"/quit\n",6) && strcmp(user->name,"Client") && strncmp(buffer_client,"/nick ",6) && strncmp(buffer_client,"/who\n",5) && strncmp(buffer_client,"/whois ",7)) {

        printf("[%s]: %s",user->name, buffer_client);

		    } else if(tempo_recvs==-1){
        printf("ERROR : AN ERROR OCCURED  WHILE RECEIVING MESSAGE \n");
        perror("recv"); exit(EXIT_FAILURE);
		    }
                    


                
                    // /nick management
                    if(strncmp(buffer_client,"/nick ",6)==0) {

                        // Get the client input and remove the segmentation fault 
                        name_by_default = strchr(buffer_client,' ') +sizeof(char);
                        strcpy(name_by_default+(strlen(name_by_default)-1)*sizeof(char),"\0");

                        int already_used=0;
			
                        
                        
                        // We browse through the user list in order to check if the nickname chosen by the Client is already used
                        for(k=4;k<max_fd;k++) {
                            if (k != i) {
                                puser nickuser = get_user_by_socket(user_list,k);
                                if(strcmp(nickuser->name,name_by_default)==0){
                                    strcpy(buffer_client,"ERROR : NICKNAME ALREADY USED ! \n");
                                    already_used=1;
                                    break;
                                }
                            }
                        }

                        // If the nickname chosen by the Client isn't already used
                        if(already_used!=1 && strcmp(user->name,name_by_default)) {

			  // When a Client visit the chat for the first time
                            if (!strcmp(user->name,"Client")) {
                                strcpy(user->name,name_by_default);

                                sprintf(buffer_client,"WELCOME ON THE CHAT %s \n",user->name);
                                printf("[SERVER]: %s",buffer_client);
                            } 

                            // If a Client is already connected on the chat with a nickname but he used /nick again, he can change his nickname
                            else {
                                printf("[SERVER]: %s IS NOW KNOWN AS %s \n",user->name,name_by_default);
                                strcpy(user->name,name_by_default);
                                sprintf(buffer_client,"YOUR NEW NICKNAME IS  %s \n",user->name);
                            }
                        }
                        do_send_server(i, buffer_client, size_buffer);                      
                    }


                    // If a Client doesn't have a nickname yet, he can't talk on the chat 
                    else if(strcmp(user->name,"Client")==0) {
                        strcpy(buffer_client,"ERROR : YOU MUST CHOOSE A NICKNAME BY USING /nick BEFORE CHATTING ! \n");
                        do_send_server(i, buffer_client, size_buffer);
                    }


                    // /who management
                    else if(strncmp(buffer_client,"/who\n",5)==0) {
                        strcpy(buffer_client,"MORE INFORMATION ABOUT ONLINE USERS -> THESE CLIENTS ARE CURRENTLY ONLINE  :\n");
                        for(k=4;k<max_fd;k++){
                            puser whouser = get_user_by_socket(user_list,k);
                            if (strcmp(whouser->name,"Client")){
                                char who_name[60];
                                sprintf(who_name," %s \n",whouser->name);
                                strcat(buffer_client,who_name);
                            }
                        }
                        do_send_server(i, buffer_client, size_buffer);
                    }

                    // /whois management
                    else if(strncmp(buffer_client,"/whois ",7)==0) {
                        who_is_nick = strchr(buffer_client,' ') +sizeof(char);
                        strcpy(who_is_nick+(strlen(who_is_nick)-1)*sizeof(char),"\0");
                        puser whois_user;
                        whois_user = get_user_by_name(user_list,who_is_nick);
                        if (whois_user != NULL) {
                            sprintf(buffer_client,"MORE INFORMATION ABOUT CLIENT  %s : HE IS CONNECTED SINCE   %s \n", whois_user->name, whois_user->connect_data);
                        }
                        else{
                            strcpy(buffer_client,"ERROR : THERE IS NO ANY CLIENTS CONNECTED  \n");
                        }
                        do_send_server(i, buffer_client, size_buffer);
                    }


                    // /quit management
                    else if(strncmp(buffer_client,"/quit\n",6)==0) {
                        close(i);
                        FD_CLR(i,&my_fd_set);
                        printf("[SERVER]: %s JUST LEFT THE CHAT -> THE CLIENT %d IS DISCONNECTED !   \n",user->name,user->socket_client-socket);                        
                        break;
                    }


                    //Sending back message to client
                    else {
                    strcpy(buffer_client,"\0");
                    do_send_server(i, buffer_client, size_buffer);
                    }

                }
            }
        }
    }

    // Closing socket
    close(socket);

    return 0;
}

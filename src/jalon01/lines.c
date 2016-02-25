#include "lines.h"

ssize_t send_line(int fd, void *buf, size_t len){

	/* Variables */
	int nleft;
	int nwritten;
	char * ptr;
	ptr = buf;
	nleft = len;

	/* perform the send */
	while( nleft > 0 ){
		if( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if(errno == EINTR){
				nwritten = 0;
				perror("");
			}
			return -1;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return len;
}

ssize_t display_line(char * buf, int len){
	send_line(STDOUT_FILENO, buf, len);
	//printf("\n");
}

ssize_t read_line(int fd, char * buf, size_t len){

	/* Variables */
	int i;
	char c;
	int ret;
	char * ptr;
	ptr = buf;
	int cnt = 0;

	/* Perform the read */
	for (i = 0 ; i < len; i++){

		ret = read(fd, &c, 1);

		if( ret == 1 ){
			ptr[cnt++] = c;

			if( c == '\n'){
				ptr[cnt] = '\0';
				return i+1;
			}
		}
		else if( 0 == ret ) {
			ptr[cnt] = '\0';
			break;
		}
	}
	ptr[len] = '\0';

	/* Empty stdin buffer in the case of too large user_input */
	if( fd == STDIN_FILENO && i == len ){
		char ss[10*MSG_SIZE];
		ret = read(fd, ss, 10*MSG_SIZE);
	}

	return i;
}


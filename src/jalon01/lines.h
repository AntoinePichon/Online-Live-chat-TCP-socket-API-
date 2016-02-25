/*
 * lines.h
 */

#ifndef _LINES_H_
#define _LINES_H_

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define MSG_SIZE 1024

ssize_t send_line(int fd, void *buf, size_t len);

ssize_t display_line(char * buf, int len);

ssize_t read_line(int fd, char * buf, size_t len);

#endif


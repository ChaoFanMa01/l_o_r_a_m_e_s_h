#ifndef SERIAL_PORT_COMMUN_H
#define SERIAL_PORT_COMMUN_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>   // Multiplexing I/O using epoll functions.

#define BUF_SIZE 100

int read_a_char(int);
int getline_fd(int, char *);
int add_epoll_read_event(int, int);
int add_epoll_write_event(int, int);
int delete_epoll_read_event(int, int);
int delete_epoll_write_event(int, int);
int modify_epoll_to_read_event(int, int);
int modify_epoll_to_write_event(int, int);
int init_epoll(int [], int, int [], int);
int read_line(int, char *, int);

#endif
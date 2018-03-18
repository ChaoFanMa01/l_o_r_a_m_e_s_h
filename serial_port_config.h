/** \file serial_port_config.h
 *
 * Header file for declaring functions used to configure serial ports.
 */

#ifndef SERIAL_PORT_COMMUN_CONFIG_H
#define SERIAL_PORT_COMMUN_CONFIG_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>     /* Error number definitions */
#include <termios.h>   /* POSIX termina control definition */

static struct termios     save_termios;
int init_serial_port(const char *);
int raw_send_init_nparity(const char *);
int raw_send_init_parity(const char *);
int raw_receive_init_nparity(const char *);
int raw_receive_init_parity(const char *);
int raw_recv_send_init_nparity(const char *);
int raw_recv_send_init(const char *, int);
int change_vmin(int, int);

#endif
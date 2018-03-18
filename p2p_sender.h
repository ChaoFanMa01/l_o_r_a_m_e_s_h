/** \file p2p_sender.h
 *
 * Function delcarations for senders in 
 * point-to-point communication test.
 */

#ifndef P2P_SENDER_H
#define P2P_SENDER_H

#include "header.h"
#include "io_ops.h"                 // I/O functions
#include "serial_port_config.h"     // Configure serial port
#include "as32_config.h"            // Configure AS-32 LoRa module
#include "gps_analyzer.h"           // Get GPS information

#define LORA_LIMIT 1      /**< The maximal number of characters
                                that can be sent via a single LoRa
                                packet. If we transmit a packet whose
                                size is greater than this value, we
                                cannot guarantee the receiver will 
                                receive a complete packet.
                            */

struct timeval time_difference(struct timeval *restrict, struct timeval *restrict);
void add_address(char *);
char *str_reverse(char *);
char *itoa(int num, char *);
char *p2p_test_packet(char *, int, char *);
int p2p_send_packet(int, char *);
int p2p_sender(int, int, int);

#endif
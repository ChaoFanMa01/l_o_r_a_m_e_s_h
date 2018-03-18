/** \file as32_config.h
 *
 * Header inclusions, macro definitions, and function
 * declarations for configuring LoRa module as32-ttl-100.
 */

#ifndef _AS32_CONFIG_H
#define _AS32_CONFIG_H

#include <unistd.h>  // For read, write, and sleep.
#include <fcntl.h>   // For file and directory operations.
#include <string.h>  // memset().
#include "header.h"  // Message output.

#define ADDH          0x00    //< The higher address of this module.
#define ADDL          0x01    //< The lower address of this module.
#define SPEED         0x1a    /*< Setting parity check and baud rate 
                               * of the serial port, and the air 
                               * rate of lora.
                               */
#define CHAN          0x17    //< The communication channel of lora.
#define OPTION        0xc4    /*< Optional settings: transparent or fixed
                               * location transmit, I/O driven mode,
                               * awake time, FEC and transmit power.
                               */
#define PERSIST_CMD   0xc0    /*< Denoting a persist command, which will be
                               * kept after power down.
                               */
#define TEMP_CMD      0xc2    /*< Denoting a temporary command, which will
                               * discarded after power down.
                               */
#define PERSIST       0       //< Denoting a persist command.
#define TEMPORARY     1       // Denoting a temporary command.

int read_as32_param(int, char []);
int read_as32_version(int, char []);
int clear_line_feed(int);
int reset_as32(int);
int set_transmit_param(int, int);

#endif
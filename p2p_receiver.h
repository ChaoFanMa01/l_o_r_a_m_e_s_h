/** \file p2p_receiver.h
 *
 * Function declarations for receivers in 
 * point-to-point communication test.
 */

#ifndef _P2P_RECEIVER_H
#define _P2P_RECEIVER_H

#include "header.h"
#include "io_ops.h"                 // I/O functions
#include "serial_port_config.h"     // Configure serial port
#include "as32_config.h"            // Configure AS-32 LoRa module
#include "gps_analyzer.h"           // Get GPS information

#define TIMER 20  /**< The time (in seconds) to compute
                   * packet reception rate.
                   */

// The number of accepted packets in a run (TIMER seconds).
static int       cnt = 0;
// The sequence number of the first accepted packet in a test run.
static int       first = -1;
// The sequence number of the last accepted packet in a test run.
static int       last = -1;
// The sequence number of the currently accepted packet.
static long      sequence;
// The latitude of the sender.
static double    latitude;
// The longitude of the sender.
static double    longitude;
// The altitude of the sender.
static double    altitude;
// The packet reception rate of this test run.
static double    prr;
// The GPS information of receiver.
static gps_info  gps;
// The distance between sender and receiver.
static double    distance;

static void sig_alrm(int);
int is_complete_packet(char *);

#endif
/** \file p2p_receiver.c
 *
 * Function definitions for the senders in
 * point-to-point communication test.
 *
 * Receiver operation sequence (ver. 0.0):
 *
 *                  ---------------------------
 *                  |       start timer       |
 *                  ---------------------------
 *                                |
 *                                V
 *                  ---------------------------
 *                  | block receive a packet  |
 *       ---------->| record lost packets and |
 *       |          | accept packets.         |
 *       |          ---------------------------
 *       |                        |
 *       |                        V
 *       |                       / \
 *       |           N         / if  \
 *       |<------------------/ over 20 \
 *       |                   \ seconds /
 *       |                     \  ?  /
 *       |                       \ /
 *       |                        | Y
 *       |                        V
 *       |         -------------------------------
 *       |         | Reset timer, compute packet |
 *       |         | recept rate and distance,   |
 *       |         | reset all counters.         |
 *       |         -------------------------------
 *       |                        |
 *       --------------------------
 */

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "p2p_receiver.h"

/** \rn static void sig_alrm(int signo)
 *
 * Signal handler for signal SIGALRM.
 */
static void sig_alrm(int signo) {
    if (signo == SIGALRM) {
        if (signal(SIGALRM, sig_alrm) == SIG_ERR)
            exit(-1);
        // If we have more received packets than the packets 
        // sent by sender, we have received one packet belonging
        // to last test run. So the PRR of this test run is 100%.
        if (cnt > TIMER) {
            print_msg("\033[47;31mPRR: 100.00%%\033[0m\n");
        } else if (cnt == TIMER - 1) {
            // If we lost only 1 packet and the received packets
            // are all in order, the last sent packet will appear
            // in the next test run. So we also have a PRR of
            // 100% in this test run.
            if (last - first == cnt)
                printf("\033[47;31mPRR: 100.00%%\033[0m\n");
            // Else, we lost 1 packet.
            else
                printf("\033[47;31mPRR: %.2lf%%\033[0m\n", (double)(cnt) / (double)(TIMER) * 100);
        } else {
            printf("\033[47;31mPRR: %.2lf%%\033[0m\n", (double)(cnt) / (double)(TIMER) * 100);
        }
        // Reset variables for next test run.
        first = -1;
        last = -1;
        cnt = 0;
        // Restart the alarm timer.
        alarm(TIMER);
    }
}

/** \fn int is_complete_packet(char *packet)
 *
 * Check whether a complete packet is received.
 * \param packet The received packet.
 * \return Return -1 on error, a positive 
 *         integer on success.
 */
int is_complete_packet(char *packet) {
    int cnt = 0;
    for (int i = 0; *(packet + i) != '\0'; i++)
        if (*(packet + i) == ',')
            cnt++;
    if (cnt != 5)
        return -1;
    return cnt;
}

int main(int argc, char *argv[]) {
    int lora_fd, gps_fd;
    char buf[100], param[20], gps_information[100];

    if (argc != 3)
        error_dump("argument misconfiguration.");
    if ((lora_fd = raw_receive_init_nparity(argv[1])) < 0)
        error_dump("fail");
    if ((gps_fd = raw_receive_init_nparity(argv[2])) < 0)
        error_dump("fail");
    
    // Install signal handler for signal SIGALRM.
    if (signal(SIGALRM, sig_alrm) == SIG_ERR)
        exit(-1);

    // Start alarm timer.
    alarm(TIMER);
    
    while (1) {
        // Block read a packet.
        if (read_line(lora_fd, buf, 100) < 0)
            continue;
        // Check whether this packet is complete.
        if (is_complete_packet(buf) < 0)
            continue;
        // Get the sequence number of this packet.
        if (get_nth_parameter(buf, 0, param) == NULL)
            continue;
        sequence = strtod(param, NULL);
        // Get the latitude of the sender.
        if (get_nth_parameter(buf, 1, param) == NULL)
            continue;
        latitude = strtod(param, NULL);
        // Get the longitude of the sender.
        if (get_nth_parameter(buf, 3, param) == NULL)
            continue;
        longitude = strtod(param, NULL);
        // Get the altitude of the sender.
        if (get_nth_parameter(buf, 5, param) == NULL)
            continue;
        altitude = strtod(param, NULL);
        // Record the sequence number of the first 
        // accept packet in this test run.
        if (first < 0)
            first = sequence;
        // Record the sequence number of the last
        // accept packet in this test run.
        last = sequence;
        // Increment the number of accept packets.
        cnt++;
        // Get the GPS information of the receiver.
        while (1) {
            if (read_raw_gps(gps_fd, gps_information) < 0)
                error_dump("gps read error");
            if (is_gpgga(gps_information) == TRUE) {
                get_gps_info(gps_information, &gps);
                // Compute the distance between the sender
                // and the receiver.
                distance = get_distance(latitude, longitude, 
                    gps.latitude, gps.longitude);
                break;
            }
        }
        printf("Seq:%5ld, sender's GPS info: (%lf, %lf)\n"
               "          receiver's GPS info: (%lf, %lf)\n"
               "distance: %lf m\n",
            sequence, latitude, longitude, gps.latitude, 
            gps.longitude, distance);
    }
    return 0;
}
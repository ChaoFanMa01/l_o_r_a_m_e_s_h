/** \file p2p_sender.c
 *
 * Function definitions for the senders in
 * point-to-point communication test.
 *
 * Sender operation sequence:
 *
 *         ---------------
 *         | sender open  |
 *         ---------------
 *                 |
 *                 v
 *        --------------------
 *    -->|   send a packet    |
 *    |   --------------------
 *    |_____________|
 *
 * A packet consists of sequence number and GPS
 * information. Its formation is listed as follow:
 *
 * ---------------------------------------------------------...
 * | sequence number, latitude, hesisphere (north or south),
 * ---------------------------------------------------------...
 *
 * ...-------------------------------------------------
 *     longitude, hesisphere (west or east), altitude |
 * ...-------------------------------------------------
 */
#include <sys/time.h>    // For time operations
#include <string.h>      // For strlen()
#include "p2p_sender.h"


/** \fn struct timeval time_difference(struct timeval *restrict e, struct timeval *restrict b)
 * 
 * Calculate the time elapse between time e and time b.
 * \param e The end time, which is greater than time b.
 * \param b The beginning time.
 * \return Return the time elapse.
 */
struct timeval time_difference(struct timeval *restrict e,
    struct timeval *restrict b) {
    struct timeval diff;
    if (e->tv_usec > b->tv_usec)
        diff.tv_usec = e->tv_usec - b->tv_usec;
    else {
        e->tv_sec--;
        diff.tv_usec = 1000000 - b->tv_usec + e->tv_usec;
    }
    diff.tv_sec = e->tv_sec - b->tv_sec;
    return diff;
}

/** \fn void add_address(char *buf)
 *
 * Add the LoRa network address to the head of this 
 * packet. This packet will be sent to a subnetwork,
 * in which all nodes have the same subnetwork address.
 * This subnetwork address is placed at the first 
 * three bytes of a packet. When the receiver accpets
 * this packet, the subnetwork address is automatically
 * removed.
 *
 * \param buf The packet.
 */
void add_address(char *buf) {
    for (int i = strlen(buf); i >= 0; i--)
        *(buf + i + 3) = *(buf + i);
    *buf = 0x12;
    *(buf + 1) = 0x34;
    *(buf + 2) = 0x17;
}

/** \fn char *str_reverse(char *str)
 *
 * Reverse a given string.
 * \param str The given string.
 * \return Always return the given string.
 */
char *str_reverse(char *str) {
    for (int i = 0, len = strlen(str), ch; i < len / 2; i++) {
        ch = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = ch;
    }
    return str;
}

/** \fn char *itoa(int num, char *str)
 *
 * Transform an integer to a string corresponding 
 * to its value.
 * \param num The integer.
 * \param str The string.
 * \return The string.
 */
char *itoa(int num, char *str) {
    int i, j, ten;

    for (i = 1, ten = 10; num / ten != 0; i++, ten *= 10) ;

    for (j = 0, ten = 1; j < i; j++, ten *= 10)
        str[j] = '0' + (num / ten) % 10;
    str[j] = '\0';

    str_reverse(str);

    return str;
}

/** \fn char *p2p_test_packet(char *packet, int seq, char *gps_info)
 *
 * Create a packet according to GPS information and sequence
 * number.
 * \param packet The address of packet to tbe created.
 * \param seq The sequence number of this packet.
 * \param gps_info The GPS information of this sender.
 * \return Return the address of this packet.
 */
char *p2p_test_packet(char *packet, int seq, char *gps_info) {
    char str[20];
    int len;

    itoa(seq, str);
    len = strlen(strcpy(packet, str));
    packet[len++] = ',';
    packet[len] = '\0';

    if (get_latitude(gps_info, str) == NULL)
        return NULL;
    len += strlen(strcpy(packet + len, str));
    packet[len++] = ',';
    packet[len] = '\0';

    if (get_ns_hemisphere(gps_info, str) == NULL)
        return NULL;
    len += strlen(strcpy(packet + len, str));
    packet[len++] = ',';
    packet[len] = '\0';

    if (get_longitude(gps_info, str) == NULL)
        return NULL;
    len += strlen(strcpy(packet + len, str));
    packet[len++] = ',';
    packet[len] = '\0';

    if (get_ew_hemisphere(gps_info, str) == NULL)
        return NULL;
    len += strlen(strcpy(packet + len, str));
    packet[len++] = ',';
    packet[len] = '\0';

    if (get_altitude(gps_info, str) == NULL)
        return NULL;
    len += strlen(strcpy(packet + len, str));
    packet[len++] = '\n';
    packet[len] = '\0';

    return packet;
}

/** \fn int p2p_send_packet(int lora_fd, char *packet)
 *
 * Send a packet through LoRa module.
 * \param lora_fd The file descriptor of the LoRa 
 *        serial port.
 * \param packet The address of the packet to be sent.
 */
int p2p_send_packet(int lora_fd, char *packet) {
    int cnt = 0, len;
    char lora_packet[10];
    while (cnt < strlen(packet)) {
        memset(lora_packet, '\0', sizeof(lora_packet));
        strncpy(lora_packet, packet + cnt, LORA_LIMIT);
 //       add_address(lora_packet);
        change_vmin(lora_fd, strlen(lora_packet));
        printf("%s - %ld\n",lora_packet, strlen(lora_packet));
        write(lora_fd, lora_packet, strlen(lora_packet));
        cnt += LORA_LIMIT;
    }
    return cnt;
}

int p2p_sender(int lora_fd, int gps_fd, int num) {
    char gps_info[BUF_SIZE], buf[BUF_SIZE];
    int seq = 0, cnt;
    struct timeval begin, end, interval;

    while (1) {
        gettimeofday(&begin, NULL);
        for (int i = 0; i < num;) {
            if (read_raw_gps(gps_fd, gps_info) < 0)
                error_dump("gps read error");
            if (is_gpgga(gps_info) == TRUE) {
                p2p_test_packet(buf, seq++, gps_info);
                p2p_send_packet(lora_fd, buf);
                write(STDOUT_FILENO, "--->", 4);
                write(STDOUT_FILENO, buf, strlen(buf));
                i++;
            }
        }
        gettimeofday(&end, NULL);
        interval = time_difference(&end, &begin);
        printf("---->time elapse: %d s %d ms\n", 
            (int)interval.tv_sec, (int)interval.tv_usec);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int lora_fd, gps_fd;
    if (argc != 3)
        error_dump("argument misconfiguration.");
    if ((lora_fd = raw_send_init_nparity(argv[1])) < 0)
        error_dump("fail");
    if ((gps_fd = raw_receive_init_nparity(argv[2])) < 0)
        error_dump("fail");

    p2p_sender(lora_fd, gps_fd, 10);

    return 0;
}
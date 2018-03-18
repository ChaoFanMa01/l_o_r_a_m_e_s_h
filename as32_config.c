/** \file as32_config.c
 *
 * Function definitions for configuring LoRa module
 * AS32-TTL-100.
 */

#include "as32_config.h"

/** \fn int set_transmit_param(int spfd, int persist_or_temporary)
 *
 * Set the transmit parameters of LoRa module.
 *
 * \param spfd The descriptior of a open serial port which 
 *        communicates with the LoRa module.
 * \param persist_or_temporary Flag denoting whether the
 *        configure parameters are permanently or temporarily 
 *        written to the LoRa module.
 * \return Returns 0 on success, -1 on failure.
 */
int set_transmit_param(int spfd, int persist_or_temporary) {
    char cmd[6];
    memset(cmd, 0, 6 * sizeof(char));

    // Records the configuration command in a buffer.
    if (persist_or_temporary == PERSIST) {
        print_msg("persist");
        cmd[0] = 0xc0;
    }
    else {
        print_msg("temporary");
        cmd[0] = 0xc2;
    }
    cmd[1] = 0x56;
    cmd[2] = 0x78;
    cmd[3] = 0x1a;
    cmd[4] = 0x17;
    cmd[5] = 0xc4;

    for (int i = 0; i < 6; i++)
        print_msg("0x%x", cmd[i]);

    // Write the command to LoRa module.
    if (write(spfd, cmd, 6) < 0)
        error_dump("fail to write command.");
    return OK;
}

/** \fn int clear_line_feed(int spfd)
 * 
 * Clear the line feed in the input buffer.
 *
 * \param spfd The descriptior of a open serial port which 
 *        communicates with the LoRa module.
 * \return Returns the character immediately after a line
 *         feed on success, or -1 on error.
 */
int clear_line_feed(int spfd) {
    char ch;

    while (1) {
        if (read(spfd, &ch, 1) < 0)
            error_dump("clear line feed error.");
        // The integers representing line feeds are
        // 10 and 13.
        if (ch != 10 && ch != 13)
            return ch;
    }

    return ERROR;
}

/** \fn int reset_as32(int spfd)
 *
 * Reset the LoRa module.
 *
 * \param spfd The descriptior of a open serial port which 
 *        communicates with the LoRa module.
 * \return Returns 0 on success, -1 on error.
 */
int reset_as32(int spfd) {
    char cmd[3];
    memset(cmd, 0, 3 * sizeof(char));
    cmd[0] = 0xc4;
    cmd[1] = 0xc4;
    cmd[2] = 0xc4;

    for (int i = 0; i < 3; i++)
        printf("0x%x ", cmd[i]);
    printf("\n");

    if (write(spfd, cmd, 3) < 0)
        error_dump("fail to write command.");
    return OK;
}

/** \fn int read_as32_version(int spfd, char version[])
 *
 * Read the version of a given LoRa module.
 *
 * \param spfd The descriptior of a open serial port which 
 *        communicates with the LoRa module.
 * \param version The buffer provided by user to hold the
 *        string representing the module version.
 * \return Returns the length of version string on success,
 *         -1 on error.
 */
int read_as32_version(int spfd, char version[]) {
    char cmd[3];
    int i;
    memset(cmd, 0, 3 * sizeof(char));
    cmd[0] = 0xc3;
    cmd[1] = 0xc3;
    cmd[2] = 0xc3;

    if (write(spfd, cmd, 3) < 0)
        error_dump("fail to write command.");
    print_msg("begin reading");
    for (i = 0; i < 16; i++)
        if (read(spfd, version + i, 1) < 0)
            error_dump("fail to read parameters.");

    return i - 1;
}

/** \fn int read_as32_param(int spfd, char param[])
 *
 * Read the transmit setting of a LoRa module.
 *
 * \param spfd The descriptior of a open serial port which 
 *        communicates with the LoRa module.
 * \param param The buffer provided by user to hold the 
 *        configuration of the given LoRa module.
 * \return Returns the number of bytes presenting the configuration
 *         of the given LoRa module in the param buffer.
 */
int read_as32_param(int spfd, char param[]) {
    char cmd[3];
    int i;

    memset(cmd, 0, 3 * sizeof(char));

    cmd[0] = 0xc1;
    cmd[1] = 0xc1;
    cmd[2] = 0xc1;

    /*
     * Be aware of that when write command to the lora module,
     * the command codes should be written at once, i.e., the
     * VMIN parameter of serial port should be set equal to the length
     * of command. For example, to gain the current configuration,
     * we should send the command 0xc1 0xc1 0xc1, and the VMIN should
     * be set as 3.
     * Furthermore, the serial port should have a baud rate of 
     * 9600 and 8-bit data, and no parity checks should be enabled.
     */
    sleep(1);
    if (write(spfd, cmd, 3) < 0)
        error_dump("fail to write command.");
    for (int i = 0; i < 6; i++) {
        if (read(spfd, param + i, 1) < 0)
            error_dump("fail to read parameters.");
    }

    return i - 1;
}
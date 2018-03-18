/** \file serial_port_config.c
 * 
 * Source file for defining functions used to 
 * configure serial port.
 */

#include "header.h"
#include "serial_port_config.h"

/** \fn int change_vmin(int fd, int vmin)
 *
 * Change the vim parameter of a given serial port.
 * \param fd File descriptor of a open serial port.
 * \param vmin The new value of vmin.
 * \return Return 0 on success, -1 on failure.
 */
int change_vmin(int fd, int vmin) {
    struct termios buf;

    if (tcgetattr(fd, &buf) < 0) {
        switch (errno) {
            case ENOTTY:
                error_dump("cannot get serial port attributions,\n"
                    "or accessing a file that is not a serial port!");
            default:
                error_exit(errno);
        }
    }

    /*
     * Saving old serial port setting in order to rescure
     * configuration failure.
     */
    save_termios = buf;

    buf.c_cc[VMIN] = vmin;

    if (tcsetattr(fd, TCSANOW, &buf) < 0) {
        print_msg("fail to set port attributes.");
        return ERROR;
    }

    if (tcgetattr(fd, &buf) < 0) {
        print_msg("fail to get port attributes.");
        tcsetattr(fd, TCSANOW, &save_termios);
        return ERROR;
    }
 
    if (buf.c_cc[VMIN] != vmin) {
        tcsetattr(fd, TCSANOW, &save_termios);
        print_msg("configuration failure.");
        return ERROR;
    }

    return OK;
}

/** \fn int raw_recv_send_init(const char *portname, int length)
 * 
 * Configure a serial port to be capable of reading and writing
 * in the raw I/O mode with a given vmin.
 * The baud rate is set to 9600 bps. No parity check is performed,
 * and the character size is set to 8.
 * 
 * \param portname The serial port name, e.g., /dev/ttyUSB0.
 * \param length The given value of vmin.
 * \return Return 0 on success, -1 on failure.
 */
int raw_recv_send_init(const char *portname, int length) {
    int fd;
    struct termios buf;

    if ((fd = open(portname, O_RDWR | O_NOCTTY)) < 0)
        error_dump("fail to open serial port!");

    if (tcgetattr(fd, &buf) < 0) {
        close(fd);
        switch (errno) {
            case ENOTTY:
                error_dump("cannot get serial port attributions,\n"
                    "or accessing a file that is not a serial port!");
            default:
                error_exit(errno);
        }
    }

    buf.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
    buf.c_cflag |= CS8;
    cfsetospeed(&buf, B9600);

    buf.c_lflag &= ~(ICANON | ECHO | ECHOE);

    buf.c_iflag &= ~(IXON | IXOFF | IXANY);

    buf.c_oflag &= ~OPOST;

    buf.c_cc[VMIN] = length;
    buf.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &buf) < 0) {
        close(fd);
        print_msg("fail to set port attributes.");
        return ERROR;
    }

    if (tcgetattr(fd, &buf) < 0) {
        close(fd);
        print_msg("fail to get port attributes.");
        return ERROR;
    }
 
    if ((buf.c_cflag & (CSIZE | PARENB | CS8 | CSTOPB)) != CS8 ||
        (cfgetospeed(&buf) != B9600) ||
        (buf.c_oflag & OPOST) ||
        (buf.c_lflag & (ICANON | ECHO | ECHOE)) ||
        (buf.c_iflag & (IXON | IXOFF | IXANY)) ||
        (buf.c_cc[VMIN] != length) ||
        (buf.c_cc[VTIME] != 0)
        ) {
        close(fd);
        print_msg("configuration failure.");
        return ERROR;
    }

    return fd;
}

/** \fn int raw_recv_send_init_nparity(const char *portname)
 * 
 * Configure a serial port to be capable of reading and writing
 * in the raw I/O mode.
 * The baud rate is set to 9600 bps. No parity check is performed,
 * and the character size is set to 8. The value of vmin is 1.
 * 
 * \param portname The serial port name, e.g., /dev/ttyUSB0.
 * \return Return 0 on success, -1 on failure.
 */
int raw_recv_send_init_nparity(const char *portname) {
    int fd;
    struct termios buf;

    if ((fd = open(portname, O_RDWR | O_NOCTTY)) < 0)
        error_dump("fail to open serial port!");

    if (tcgetattr(fd, &buf) < 0) {
        close(fd);
        switch (errno) {
            case ENOTTY:
                error_dump("cannot get serial port attributions,\n"
                    "or accessing a file that is not a serial port!");
            default:
                error_exit(errno);
        }
    }

    buf.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
    buf.c_cflag |= CS8;
    cfsetospeed(&buf, B9600);

    buf.c_lflag &= ~(ICANON | ECHO | ECHOE);

    buf.c_iflag &= ~(IXON | IXOFF | IXANY);

    buf.c_oflag &= ~OPOST;

    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &buf) < 0) {
        close(fd);
        print_msg("fail to set port attributes.");
        return ERROR;
    }

    if (tcgetattr(fd, &buf) < 0) {
        close(fd);
        print_msg("fail to get port attributes.");
        return ERROR;
    }
 
    if ((buf.c_cflag & (CSIZE | PARENB | CS8 | CSTOPB)) != CS8 ||
        (cfgetospeed(&buf) != B9600) ||
        (buf.c_oflag & OPOST) ||
        (buf.c_lflag & (ICANON | ECHO | ECHOE)) ||
        (buf.c_iflag & (IXON | IXOFF | IXANY)) ||
        (buf.c_cc[VMIN] != 1) ||
        (buf.c_cc[VTIME] != 0)
        ) {
        close(fd);
        print_msg("configuration failure.");
        return ERROR;
    }

    return fd;
}

/** \fn int raw_receive_init_parity(const char *portname)
 * 
 * Configure a serial port to be capable of reading
 * in the raw I/O mode.
 * The baud rate is set to 9600 bps. Even or odd arity check is performed,
 * and the character size is set to 7 with 1 stop bit. The value of vmin is 1.
 * 
 * \param portname The serial port name, e.g., /dev/ttyUSB0.
 * \return Return 0 on success, -1 on failure.
 */
int raw_receive_init_parity(const char *portname) {
    int fd;
    struct termios buf;

    if ((fd = open(portname, O_RDONLY | O_NOCTTY)) < 0)
        error_dump("fail to open serial port!");

    if (tcgetattr(fd, &buf) < 0) {
        close(fd);
        switch (errno) {
            case ENOTTY:
                error_dump("cannot get serial port attributions,\n"
                    "or accessing a file that is not a serial port!");
            default:
                error_exit(errno);
        }
    }

    /*
     * Even parity checking.
     * PARODD: use odd parity instead of even. Here, we clear this.
     */
    buf.c_cflag &= ~(CSIZE | PARODD | CSTOPB);
    buf.c_cflag |= (CS7 | PARENB);
    cfsetospeed(&buf, B9600);

    /*
     * Odd parity checking.
     */
    /*
    buf.c_cflag &= ~(CSIZE | CSTOPB);
    buf.c_cflag |= (CS7 | PARENB | PARODD);
    cfsetospeed(&buf, B9600);
    */
    
    /*
     * Setting c_lfag.
     */
    buf.c_lflag &= ~(ICANON | ECHO | ECHOE);

    /*
     * Setting c_iflag.
     *
     * INPCK: Enable parity check.
     * ISTRIP: Strip parity bits.
     */
    buf.c_iflag &= ~(IXON | IXOFF | IXANY);
    buf.c_iflag |= (INPCK | ISTRIP);

    /*
     * Setting c_cc array.
     */
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;

    /*
     * Establish our configurations.
     */
    if (tcsetattr(fd, TCSANOW, &buf) < 0) {
        close(fd);
        print_msg("fail to set port attributes.");
        return ERROR;
    }

    /*
     * Check whether our configurations are established
     * successfully. See reasons turns to
     * raw_send_init_nparity() function.
     */
    if (tcgetattr(fd, &buf) < 0) {
        close(fd);
        print_msg("fail to get port attributes.");
        return ERROR;
    }
 
    if ((buf.c_cflag & (CSIZE | CS7 | CSTOPB | PARODD)) != CS7 ||
        (cfgetospeed(&buf) != B9600) ||
        (buf.c_iflag & (INPCK | ISTRIP)) != (INPCK | ISTRIP) ||
        (buf.c_lflag & (ICANON | ECHO | ECHOE)) ||
        (buf.c_iflag & (IXON | IXOFF | IXANY)) ||
        (buf.c_cc[VMIN] != 1) ||
        (buf.c_cc[VTIME] != 0)
        ) {
        close(fd);
        print_msg("configuration failure.");
        return ERROR;
    }

    return fd;
}

/** \fn int raw_receive_init_nparity(const char *portname)
 * 
 * Configure a serial port to be capable of reading
 * in the raw I/O mode.
 * The baud rate is set to 9600 bps. No arity check is performed,
 * and the character size is set to 8. The value of vmin is 1.
 * 
 * \param portname The serial port name, e.g., /dev/ttyUSB0.
 * \return Return 0 on success, -1 on failure.
 */
int raw_receive_init_nparity(const char *portname) {
    int fd;
    struct termios buf;

    if ((fd = open(portname, O_RDONLY | O_NOCTTY)) < 0)
        error_dump("fail to open serial port!");

    if (tcgetattr(fd, &buf) < 0) {
        close(fd);
        switch (errno) {
            case ENOTTY:
                error_dump("cannot get serial port attributions,\n"
                    "or accessing a file that is not a serial port!");
            default:
                error_exit(errno);
        }
    }

    /*
     * Setting c_cflag. See detail turns to the comments in 
     * raw_send_init_nparity() function.
     */
    buf.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
    buf.c_cflag |= CS8;
    cfsetospeed(&buf, B9600);

    /*
     * We then configure the local modes member c_lflag, which
     * controls how INPUT characters are managed by the serial 
     * driver.
     *
     * ISIG: Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT 
     * signals. Because the raw input mode is adopted, we clear
     * this bit, i.e., ignore these signals.
     *
     * ICANON: Enable canonical input (else raw). So, we clear
     * this bit.
     *
     * ECHO: Enable echoing of input characters.
     * ECHOE: Echo erase character as BS-SP-BS.
     * In the raw mode, we simply disable echoing.
     */
    buf.c_lflag &= ~(ICANON | ECHO | ECHOE);

    /*
     * Next, we set the input modes member c_iflag, which
     * controls any input processing that is done to 
     * characters received on the serial port.
     *
     * IXON, IXOFF: Enable software flow control. Disabled in 
     * this function.
     *
     * IXANY: Allow any character to start flow again. Disabled
     * in this function.
     */
    buf.c_iflag &= ~(IXON | IXOFF | IXANY);

    /*
     * Finally, we set timeout configuration.
     * Two elements of the c_cc array are used for timeout: VMIN 
     * and VTIME.
     * 
     * VMIN: specifies the minimum number of characters to read.
     * If it is set to 0, then the VTIME value specifies the time
     * to wait for every character read. Note that this does not 
     * mean that a read call for N bytes will wait for N character
     * to come in. Rather, the timeout will apply to the first
     * character and the read call will return the number of characters
     * immediately available (up to the number you requested).
     *
     * If VMIN is non-zero, VTIME specifies the time to wait for the
     * first character read. If a character is read within the time 
     * given, any read will block until all VMIN characters are read.
     * That is, once the first character is read, the serial interface
     * driver expects to receive an entire packet of characters (VMIN 
     * bytes total). If no character is read within the time allowed,
     * then the call to read returns 0. This method allows you to tell
     * serial driver you need exactly N bytes and any read call will 
     * return 0 or N bytes. However, the timeout only applies to the 
     * first character read, so if for some reason the driver misses
     * one character inside the N-byte-packet then the read call could
     * block forever waiting for additional input characters.
     *
     * VTIME specifies the amout of time to wait for incoming characters
     * in tenths of seconds. If VTIME is set to 0 (the default), reads 
     * will block idenfinitely unless the NDELAY option is set on the
     * port with open or fcntl.
     */
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;

    /*
     * Establish our configurations.
     */
    if (tcsetattr(fd, TCSANOW, &buf) < 0) {
        close(fd);
        print_msg("fail to set port attributes.");
        return ERROR;
    }

    /*
     * Check whether our configurations are established
     * successfully. See reasons turns to
     * raw_send_init_nparity() function.
     */
    if (tcgetattr(fd, &buf) < 0) {
        close(fd);
        print_msg("fail to get port attributes.");
        return ERROR;
    }
 
    if ((buf.c_cflag & (CSIZE | PARENB | CS8 | CSTOPB)) != CS8 ||
        (cfgetospeed(&buf) != B9600) ||
        (buf.c_lflag & (ICANON | ECHO | ECHOE)) ||
        (buf.c_iflag & (IXON | IXOFF | IXANY)) ||
        (buf.c_cc[VMIN] != 1) ||
        (buf.c_cc[VTIME] != 0)
        ) {
        close(fd);
        print_msg("configuration failure.");
        return ERROR;
    }

    return fd;
}

/** \fn int raw_send_init_nparity(const char *portname)
 * 
 * Configure a serial port to be capable of writing
 * in the raw I/O mode.
 * The baud rate is set to 9600 bps. No arity check is performed,
 * and the character size is set to 8. The value of vmin is 1.
 * 
 * \param portname The serial port name, e.g., /dev/ttyUSB0.
 * \return Return 0 on success, -1 on failure.
 */
int raw_send_init_nparity(const char *portname) {
    int fd;
    struct termios buf;
    
    /*
     * The O_NOCTTY flag tells UNIX that this program doesn't
     * want to be the "controlling terminal" for that port. If
     * you don't specify this then any input (such as keyboard
     * abort signals and so forth) will affect your process.
     * The O_NDELAY flag tells UNIX that this program doesn't
     * care what state the DCD line is - whether the other end
     * of the port is up and running. If you don't specify this
     * flag, your process will be put to sleep until the DCD 
     * signal line is the space voltage.
     */
    if ((fd = open(portname, O_WRONLY | O_NOCTTY | O_NDELAY)) < 0)
        error_dump("fail to open serial port!");
    /*
     * tcgetattr() returns -1 and sets errno to ENOTTY when
     * fd refers to a non-terminal device.
     */
    if (tcgetattr(fd, &buf) < 0) {
        close(fd);
        switch (errno) {
            case ENOTTY:
                error_dump("cannot get serial port attributions,\n"
                    "or accessing a file that is not a serial port!");
            default:
                error_exit(errno);
        }
    }

    /*
     * After successfully opening a serial port and getting its
     * termios structure, we should then configure this serial
     * port through its termios structure.
     * The termios structure is defined in the <termios.h> header,
     * and contains at least the following five elements: c_iflag,
     * c_oflag, c_cflag, c_lflag, and c_cc[NCCS].
     */

    /*
     * We first configure the c_cflag, which controls the baud rate,
     * number of data bits, parity, stop bits, and hardware flow
     * control.
     *
     * CSIZE: bit mask for data bits. We first clear the data bits
     * setting through this mask.
     * 
     * PARENB: Enable parity bit. We clear this bit to disable 
     * the parity checking.
     *
     * CSTOPB: 2 stop bits (1 otherwise). We clear this bit to use
     * only 1 stop bit.
     *
     * CS5, CS6, CS7, and CS8: stands for 5, 6, 7, and 8 data bits
     * respectively. Here, we use 8 data bits. Noting that to set
     * data bits, we first clear this setting useing the CSIZE bit
     * mask.
     *
     * cfsetospeed(): used to set the baud rate of this port.
     * We should use this function to set baud rate so as to write
     * portable codes, since the termios structure varies across
     * different operating systems. Another thing should be noted 
     * is that the cfsetospeed() function only changes the termios
     * structure, not directly setting the baud rate of this port.
     * So, we should call the tcsetattr() function to change the
     * serial port.
     * B9600: macro for baud rate of 9600 bps. The macros for baud
     * rate are B0, B50, B75, B110, B134, B150, B200, B300, B600,
     * B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600,
     * B76800, and B115200.
     */
    buf.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
    buf.c_cflag |= CS8;
    cfsetospeed(&buf, B9600);

    /*
     * We then configure the c_oflag, which contains the output
     * filtering options.
     * 
     * OPOST: postprocess output (not set = raw output). We use
     * raw output here.
     */
    buf.c_oflag &= ~OPOST;

    /*
     * Because we only configure this serial port to send data,
     * we need not to setting its input feature, i.e., c_iflag,
     * c_lflag and c_cc array.
     */

    /*
     * To enable our configuration, we call the tcsetattr()
     * function.
     *
     * TCSANOW: make changes now without waiting for data
     * to complete.
     * TCSADRAIN: wait until everything has been transmitted.
     * TCSAFLUSH: flush input and output buffers and make
     * the change.
     * We use the TCSANOW macro to make an immediate change
     * to this serial port.
     */
    if (tcsetattr(fd, TCSANOW, &buf) < 0) {
        close(fd);
        print_msg("fail to set port attributes.");
        return ERROR;
    }

    /*
     * The return status of tcsetattr() can be confusing to
     * use correctly. This function returns OK if it was able 
     * to perform any of the requested actions, even if it 
     * couldn't perform all the requested actions. If the function
     * returns OK, it is our responsibility to see whether all the 
     * requested actions were performed. This means that after we
     * call tcsetattr() to set the desired attributes, we need to
     * call tcgetattr() and compare the actual terminal's attributes
     * to desired attributes to detect any differences.
     */

    /*
     * Getting newly changed attributes.
     */
    if (tcgetattr(fd, &buf) < 0) {
        close(fd);
        print_msg("fail to get port attributes.");
        return ERROR;
    }
    /*
     * If serial port is not changed according to our 
     * configuration, close this port and return error.
     */
    if ((buf.c_cflag & (CSIZE | PARENB | CS8)) != CS8 ||
        (buf.c_oflag & OPOST) ||
        (cfgetospeed(&buf) != B9600)
        ) {
        close(fd);
        print_msg("configuration failure.");
        return ERROR;
    }

    return fd;
}

/** \fn int raw_send_init_parity(const char *portname)
 * 
 * Configure a serial port to be capable of writing
 * in the raw I/O mode.
 * The baud rate is set to 9600 bps. Even or odd parity check is performed,
 * and the character size is set to 7 with 1 stop bit. The value of vmin is 1.
 * 
 * \param portname The serial port name, e.g., /dev/ttyUSB0.
 * \return Return 0 on success, -1 on failure.
 */
int raw_send_init_parity(const char *portname) {
    int fd;
    struct termios buf;

    if ((fd = open(portname, O_WRONLY | O_NOCTTY | O_NDELAY)) < 0)
        error_dump("fail to open serial port!");

    if (tcgetattr(fd, &buf) < 0) {
        close(fd);
        switch (errno) {
            case ENOTTY:
                error_dump("cannot get serial port attributions,\n"
                    "or accessing a file that is not a serial port!");
            default:
                error_exit(errno);
        }
    }

    /*
     * Even parity checking.
     * PARODD: use odd parity instead of even. Here, we clear this.
     */
    buf.c_cflag &= ~(CSIZE | PARODD | CSTOPB);
    buf.c_cflag |= (CS7 | PARENB);
    cfsetospeed(&buf, B9600);

    /*
     * Odd parity checking.
     */
    /*
    buf.c_cflag &= ~(CSIZE | CSTOPB);
    buf.c_cflag |= (CS7 | PARENB | PARODD);
    cfsetospeed(&buf, B9600);
    */

    buf.c_oflag &= ~OPOST;

    if (tcsetattr(fd, TCSANOW, &buf) < 0) {
        close(fd);
        print_msg("fail to set port attributes.");
        return ERROR;
    }

    if (tcgetattr(fd, &buf) < 0) {
        close(fd);
        print_msg("fail to get port attributes.");
        return ERROR;
    }
 
    if ((buf.c_cflag & (CSIZE | CS7)) != CS7 ||
        (buf.c_cflag & PARENB) != PARENB ||
        (buf.c_cflag & PARODD) ||
        (buf.c_oflag & OPOST) ||
        (cfgetospeed(&buf) != B9600)
        ) {
        close(fd);
        print_msg("configuration failure.");
        return ERROR;
    }

    return fd;
}

int init_serial_port(const char *port) {
    int fd;
    struct termios buf;

    if ((fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
        error_dump("fail to open serial port!");

    if (tcgetattr(fd, &buf) < 0)
        error_dump("not a serial port");
    save_termios = buf;

    /*
     * Echo off, canonical mode off, extended input 
     * processing off, signal chars off.
     */
    buf.c_lflag &= ~(ECHO | ECHOE | ICANON | IEXTEN | ISIG);

    /*
     * No SIGINT on BREAK, CN-to-NL off, input parity
     * check off, don't strip 8th bit on input, output
     * flow control off.
     */
    buf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    /*
     * Clear size bits, parity checking off.
     */
    buf.c_cflag &= ~(CSIZE | PARENB);

    /*
     * Set 8 bits/char
     */
    buf.c_cflag |= CS8;
    buf.c_cflag &= ~CSTOPB;

    /*
     * Output processing off.
     */
    buf.c_oflag &= ~(OPOST);

    /*
     * Set baud rate.
     */
    cfsetospeed(&buf, B9600);

    /*
     * Case B: 1 byte at a time, no timer.
     */
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSANOW, &buf) < 0)
        return -1;

    /*
     * Verify that the changes stuck. tcsetattr can return 0
     * on partial success.
     */
    if (tcgetattr(fd, &buf) < 0) {
        tcsetattr(fd, TCSAFLUSH, &save_termios);
        return -1;
    }
    if ((buf.c_lflag & (ECHO | ICANON | IEXTEN | ISIG)) || 
        (buf.c_iflag & (BRKINT | ICRNL | INPCK | ISTRIP | IXON)) ||
        (buf.c_cflag & (CSIZE | PARENB | CS8)) != CS8 ||
        (buf.c_oflag & OPOST) || buf.c_cc[VMIN] != 1 ||
        buf.c_cc[VTIME] != 0) {
        /*
         * Only some of the change were made. Restore the 
         * original settings.
         */
        tcsetattr(fd, TCSANOW, &save_termios);
        return -1;
    }
    return fd;
}
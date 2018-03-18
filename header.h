/** \file header.h
 *
 * This file defines macros for printing messages and 
 * denoting function return conditions.
 */

#ifndef SERIAL_PORT_COMMUN_HEADER
#define SERIAL_PORT_COMMUN_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define OK     0 /**< For OK status */
#define ERROR -1 /**< For error status */

#define TRUE   1 /**< For true status */
#define FALSE  0 /**< For false status */

/**
 * Prints common messages, wrapper function for 
 * printf() function.
 */
#define print_msg(str, var...) (printf(str "\n", ## var))

/**
 * Prints error messsage provided by errno flag, 
 * and terminates with exit(-1).
 */
#define error_exit(error) ({\
                printf("%s" "\n", strerror(error));\
                exit(ERROR);\
})

/**
 * Prints error message from user, and terminates
 * with exit(-1).
 */
#define error_dump(str, var...) ({\
                printf(str "\n", ## var);\
                exit(ERROR);\
                                 })

/**
 * Prints error message from user, and return a
 * null pointer.
 */
#define error_return_null(str, var...) ({\
                printf(str "\n", ## var);\
                return NULL;\
                                       })

/**
 * Prints error message from user, and returns -1.
 */
#define error_return_negative(str, var...) ({\
                printf(str "\n", var...);\
                return ERROR;\
                                            })

/** 
 * Prints error message from user, and return nothing.
 */
#define error_return_nothing(str, var...) ({\
                printf(str "\n", ## var);\
                return;\
})

#endif
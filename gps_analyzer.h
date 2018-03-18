/** \file gps_analyzer.h
 * 
 * Type definitions, function declarations for analyzing
 * GPS information.
 */

#ifndef _GPS_ANALYZER_H
#define _GPS_ANALYZER_H

#include <stdlib.h>   // for strtod()
#include <string.h>   // for memset(), strncmp()
#include "serial_port_config.h"
#include "io_ops.h"

// The size of a buffer storing GPS information.
#define GPS_INFO_SIZE 100
#define PI            3.14159
// The radius of earth. This value is used to 
// calculate the distance between two points
// represented by latitude and longitude.
static const double EARTH_RADIUS = 6378.137;

/** \typedef gps_info
 * Structure for storing GPS information.
 */
typedef struct {
    double utc_time;       /**< UTC time */
    double latitude;       /**< Latitude */
    char   ns_hemisphere;  /**< North or Sorth hemisphere */
    double longitude;      /**< Longitude */
    char   ew_hemisphere;  /**< East or west hemisphere */
    double altitude;       /**< Altitude */
} gps_info;

int read_raw_gps(int, char *);
int is_gpgga(char *);
char *get_nth_parameter(char *, int, char *);
char *get_utc_time(char *, char *);
char *get_latitude(char *, char *);
char *get_ns_hemisphere(char *, char *);
char *get_longitude(char *, char *);
char *get_ew_hemisphere(char *, char *);
char *get_altitude(char *, char *);
gps_info *get_gps_info(char *, gps_info *);
void print_gps(const gps_info);
static double rad(double);
double get_distance(double, double, double, double);

#endif
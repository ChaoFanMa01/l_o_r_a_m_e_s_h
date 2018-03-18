/** \file gps_analyzer.c
 *
 * Function definitions for analyzing GPS information.
 */
#include <math.h>
#include "header.h"
#include "gps_analyzer.h"

/** \fn int read_raw_gps(int fd, char *buf)
 *
 * Read raw GPS information from Ublox NEO-7N module.
 * \param fd The serial port where GPS module is mounted.
 * \param buf Where the raw GPS information will be stored.
 * \return Returns -1 on error, non-negative integer which 
 *         denotes the character number of information on 
 *         success.
 */
int read_raw_gps(int fd, char *buf) {
    int cnt = 0;

    memset(buf, '\0', GPS_INFO_SIZE);
    while ((*(buf + cnt) = read_a_char(fd)) != '\n' 
        && cnt < GPS_INFO_SIZE) {
        if (*(buf + cnt) == '\r')
            *(buf + cnt) = '\0';
        cnt++;
    }

    *(buf + cnt) = '\0';

    if (cnt >= GPS_INFO_SIZE) return -1;
    return cnt;
}

/** \fn int is_gpgga(char *cmd)
 *
 * Check whether given command is a GPGGA information.
 * \param cmd The command.
 * \return Returns 1 if is, 0 otherwise.
 */
int is_gpgga(char *cmd) {
    if (strncmp(cmd, "$GPGGA", 6))
        return FALSE;
    else
        return TRUE;
}

/** \fn char *get_nth_parameter(char *cmd, int n, char *buf)
 * 
 * Read the nth parameter from the raw GPS information.
 * \param cmd Raw GPS information.
 * \param n Denoting which parameter to be found.
 * \param buf Where to store the nth parameter.
 * \return Returns NULL on error, or the address storing
 *         the nth parameter on success.
 */
char *get_nth_parameter(char *cmd, int n, char *buf) {
    int i, j;
    if (n >= GPS_INFO_SIZE)
        return NULL;
    memset(buf, '\0', sizeof(buf));

    // Skip the first n - 1 parameters. 
    for (i = 0, j = 0; j < GPS_INFO_SIZE && i < n; j++)
        if (*(cmd + j) == ',')
            i++;

    // Check whether find the nth parameter.
    if (j >= GPS_INFO_SIZE || i != n)
        return NULL;

    // Read the nth parameter.
    for (i = 0; *(cmd + j) != ',' && *(cmd + j) != '\0'; i++, j++)
        *(buf + i) = *(cmd +j);
    *(buf + i) = '\0';
    return buf;
}

/** \fn char *get_utc_time(char *cmd, char *param)
 *
 * Get the UTC time from the GPGGA information.
 * \param cmd The raw GPS information.
 * \param param Where to store the UTC time.
 * \return Returns NULL on error, or address storing
 *         UTC time on success.
 */
char *get_utc_time(char *cmd, char *param) {
    if (is_gpgga(cmd) != TRUE)
        return NULL;
    return get_nth_parameter(cmd, 1, param);
}

/** \fn char *get_latitude(char *cmd, char *param)
 *
 * Get the latitude from the GPGGA information.
 * \param cmd The raw GPS information.
 * \param param Where to store the latitude.
 * \return Returns NULL on error, or address storing
 *         latitude on success.
 */
char *get_latitude(char *cmd, char *param) {
    if (is_gpgga(cmd) != TRUE)
        return NULL;
    return get_nth_parameter(cmd, 2, param);
}

/** \fn char *get_ns_hemisphere(char *cmd, char *param)
 *
 * Get the north or south hemisphere from the GPGGA information.
 * \param cmd The raw GPS information.
 * \param param Where to store the hemisphere.
 * \return Returns NULL on error, or address storing
 *         hemisphere on success.
 */
char *get_ns_hemisphere(char *cmd, char *param) {
    if (is_gpgga(cmd) != TRUE)
        return NULL;
    return get_nth_parameter(cmd, 3, param);
}

/** \fn char *get_longitude(char *cmd, char *param)
 *
 * Get the longitude from the GPGGA information.
 * \param cmd The raw GPS information.
 * \param param Where to store the longitude.
 * \return Returns NULL on error, or address storing
 *         longitude on success.
 */
char *get_longitude(char *cmd, char *param) {
    if (is_gpgga(cmd) != TRUE)
        return NULL;
    return get_nth_parameter(cmd, 4, param);
}

/** \fn char *get_ew_hemisphere(char *cmd, char *param)
 *
 * Get the east or west hemisphere from the GPGGA information.
 * \param cmd The raw GPS information.
 * \param param Where to store the hemisphere.
 * \return Returns NULL on error, or address storing
 *         hemisphere on success.
 */
char *get_ew_hemisphere(char *cmd, char *param) {
    if (is_gpgga(cmd) != TRUE)
        return NULL;
    return get_nth_parameter(cmd, 5, param);
}

/** \fn char *get_altitude(char *cmd, char *param)
 *
 * Get the altitude from the GPGGA information.
 * \param cmd The raw GPS information.
 * \param param Where to store the altitude.
 * \return Returns NULL on error, or address storing
 *         altitude on success.
 */
char *get_altitude(char *cmd, char *param) {
    if (is_gpgga(cmd) != TRUE)
        return NULL;
    return get_nth_parameter(cmd, 9, param);
}

/** \fn gps_info *get_gps_info(char *cmd, gps_info *pg)
 *
 * Get the basic GPS information from the GPGGA command.
 * \param cmd The raw GPS information.
 * \param pg The address of structure representing GPS
 *           information.
 * \return Returns NULL on error, or non-NULL address on
 *         success.
 */
gps_info *get_gps_info(char *cmd, gps_info *pg) {
    char param[20];
    if (get_utc_time(cmd, param) == NULL)
        return NULL;
    pg->utc_time = strtod(param, NULL);

    if (get_latitude(cmd, param) == NULL)
        return NULL;
    pg->latitude = strtod(param, NULL);

    if (get_ns_hemisphere(cmd, param) == NULL)
        return NULL;
    pg->ns_hemisphere = param[0];

    if (get_longitude(cmd, param) == NULL)
        return NULL;
    pg->longitude = strtod(param, NULL);

    if (get_ew_hemisphere(cmd, param) == NULL)
        return NULL;
    pg->ew_hemisphere = param[0];

    if (get_altitude(cmd, param) == NULL)
        return NULL;
    pg->altitude = strtod(param, NULL);

    return pg;
}

/** \fn void print_gps(const gps_info gps)
 *
 * Print the basic GPS information based on the 
 * GPGGA command.
 */
void print_gps(const gps_info gps) {
    print_msg("UTC time: %lf.", gps.utc_time);
    print_msg("Latitude: %lf.", gps.latitude);
    if (gps.ns_hemisphere == 'N')
        print_msg("North hemisphere.");
    else
        print_msg("South hemisphere.");
    print_msg("Longitude: %lf.", gps.longitude);
    if (gps.ns_hemisphere == 'W')
        print_msg("West hemisphere.");
    else
        print_msg("East hemisphere.");
    print_msg("Altitude: %lf.", gps.altitude);
}

/** \fn static double rad(double d)
 *
 * Calculate rad.
 */
static double rad(double d)
{
    return d * PI / 180.0;
}

/** fn double get_distance(double lat1, double lng1, double lat2, double lng2)
 *
 * Compute the distance between two points which 
 * are represented by latitude and longitude.
 *
 * \param lat1 The latitude of the first point.
 * \param lng1 The longitude of the first point.
 * \param lat2 The latitude of the second point.
 * \param lng2 The longitude of the second point.
 * \return Return the distance between these two 
 *         points in terms of meter.
 */
double get_distance(double lat1, double lng1, double lat2, double lng2)
{
    double radLat1 = rad(lat1);
    double radLat2 = rad(lat2);
    double a = radLat1 - radLat2;
    double b = rad(lng1) - rad(lng2);
    double s = 2 * asin(sqrt(pow(sin(a/2),2) + 
     cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
    s = s * EARTH_RADIUS * 10;
//    s = s * 10000 / 1000;
    return s;
}

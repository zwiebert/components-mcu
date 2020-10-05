#ifdef __cplusplus
  extern "C" {
#endif
#pragma once

#define CIVIL_TWILIGHT_RAD         -0.10471975511966 ///< sun position in RAD, relative to horizon for civil-twilight
#define NAUTICAL_TWILIGHT_RAD      -0.20943951023932 ///< sun position in RAD, relative to horizon for nautical twilight
#define ASTRONOMICAL_TWILIGHT_RAD  -0.31415926535898 ///< sun position in RAD, relative to horizon for astronomical-twilight

#define LONGITUDE_DEG_TO_HOUR(longitude)    ((longitude)*0.06666133376) ///< caclulate longitude in degree to timezone in hour

/**
 * \brief               calculate dusk and dawn time
 * \param sunrise       pointer to out parameter to store result for sunrise/dawn or NULL
 * \param sunset        pointer to out parameter to store result for sunset/dusk or NULL
 * \param timezone      timezone in hours
 * \param day_of_year   number of day (1..366)
 * \param longitude_deg longitude in degree
 * \param latitude_deg  latitude in degree
 * \param horizon       location of the sun in RAD, relative to the horizon at the time we want to calculate
 *
 */
void sun_calculateDuskDawn(double *sunrise, double *sunset, double timezone, double day_of_year, double longitude_deg, double latitude_deg, double horizon);


#ifdef __cplusplus
  }
#endif

#include <math.h>
#include <glib.h>
#include <stdbool.h>
#include "sun.h"
#define ZENITH -.83

float to_rad (float n) {
    return (M_PI/180) * n;
}

float to_deg (float n) {
    return (180/M_PI) * n;
}

// http://edwilliams.org/sunrise_sunset_algorithm.htm
// if the sunset is desired, set sunset to >= 1
// returns -1 if the sun never rises/sets in the specified location
float calculateSun(int year, int month, int day, float lat, float lng, int localOffset, bool sunset) {

    float N1 = floor(275 * month / 9);
    float N2 = floor((month + 9) / 12);
    float N3 = (1 + floor((year - 4 * floor(year / 4) + 2) / 3));
    float N = N1 - (N2 * N3) + day - 30;

    float lngHour = lng / 15.0;

    float t = N + ((!sunset ? 6 : 18 - lngHour) / 24);

    float M = (0.9856 * t) - 3.289;

    float L = fmod(M + (1.916 * sin(to_rad(M))) + (0.020 * sin(2 *to_rad(M))) + 282.634,360.0);

    float RA = fmod(to_deg(atan(0.91764 * tan(to_rad(L)))),360.0);

    float Lquadrant  = floor( L/90) * 90;
    float RAquadrant = floor(RA/90) * 90;
    RA = RA + (Lquadrant - RAquadrant);
    RA = RA / 15;

    float sinDec = 0.39782 * sin(to_rad(L));
    float cosDec = cos(asin(sinDec));

    float cosH = (sin(to_rad(ZENITH)) - (sinDec * sin(to_rad(lat)))) / (cosDec * cos(to_rad(lat)));

    if (cosH >  1)
      return -1;
    else if (cosH < -1)
      return -1;

    float H = !sunset ? (360 - to_deg(acos(cosH))) : to_deg(acos(cosH));
    H = H / 15;

    float T = H + RA - (0.06571 * t) - 6.622;

    float UT = fmod(T - lngHour,24.0);

    if ( UT < 0 )
      UT = UT+24;
    else if (UT > 24)
      UT = UT-24;

    return UT + localOffset;
}

float getSunrise(float lat, float lang) {
    int year, month, day;
    GDateTime *current_time = g_date_time_new_now_local();
    g_date_time_get_ymd (current_time, &year, &month, &day);

    int utc_offset = g_date_time_get_utc_offset (current_time)*2.7778*pow(10.0, -10.0);

    free (current_time);
    return calculateSun (year, month, day, lat, lang, utc_offset, false);
}

float getSunset(float lat, float lang) {
    int year, month, day;
    GDateTime *current_time = g_date_time_new_now_local();
    g_date_time_get_ymd (current_time, &year, &month, &day);

    int utc_offset = g_date_time_get_utc_offset (current_time)*2.7778*pow(10.0, -10.0);

    free (current_time);
    return calculateSun (year, month, day, lat, lang, utc_offset, true);
}

bool isDark(float sunrise, float sunset) {
    GDateTime *current_time = g_date_time_new_now_local();

    double rise_hours;
    float rise_minutes = modf (sunrise, &rise_hours)*60;
    double set_hours;
    float set_minutes = modf (sunset, &set_hours)*60;

    GDateTime *rise_time = g_date_time_new_local (g_date_time_get_year (current_time),
                                                  g_date_time_get_month (current_time),
                                                  g_date_time_get_day_of_month (current_time),
                                                  rise_hours,
                                                  rise_minutes,
                                                  0);
    GDateTime *set_time = g_date_time_new_local (g_date_time_get_year (current_time),
                                                  g_date_time_get_month (current_time),
                                                  g_date_time_get_day_of_month (current_time),
                                                  set_hours,
                                                  set_minutes,
                                                  0);

    if (g_date_time_compare (current_time, rise_time) == 1 && g_date_time_compare (current_time, set_time) == -1) {
        free (rise_time);
        free (set_time);
        free (current_time);
        return false;
    }
    free (rise_time);
    free (set_time);
    free (current_time);
    return true;
}

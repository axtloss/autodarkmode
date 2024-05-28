#include <math.h>
#include <glib.h>
#define PI 3.1415926
#define ZENITH -.83

float calculateSunrise(int year,int month,int day,float lat, float lng,int localOffset) {
    /*
    localOffset will be <0 for western hemisphere and >0 for eastern hemisphere
    daylightSavings should be 1 if it is in effect during the summer otherwise it should be 0
    */
    //1. first calculate the day of the year
    float N1 = floor(275 * month / 9);
    float N2 = floor((month + 9) / 12);
    float N3 = (1 + floor((year - 4 * floor(year / 4) + 2) / 3));
    float N = N1 - (N2 * N3) + day - 30;

    //2. convert the longitude to hour value and calculate an approximate time
    float lngHour = lng / 15.0;
    float t = N + ((6 - lngHour) / 24);

    //3. calculate the Sun's mean anomaly
    float M = (0.9856 * t) - 3.289;

    //4. calculate the Sun's true longitude
    float L = fmod(M + (1.916 * sin((PI/180)*M)) + (0.020 * sin(2 *(PI/180) * M)) + 282.634,360.0);

    if ( L < 0 )
      L = L+360;
    else if ( L > 360)
      L = L-360;

    //5a. calculate the Sun's right ascension
    float RA = fmod(180/PI*atan(0.91764 * tan((PI/180)*L)),360.0);

    if ( RA < 0 )
      RA = RA+360;
    else if ( RA > 360)
      RA = RA-360;

    //5b. right ascension value needs to be in the same quadrant as L
    float Lquadrant  = floor( L/90) * 90;
    float RAquadrant = floor(RA/90) * 90;
    RA = RA + (Lquadrant - RAquadrant);

    //5c. right ascension value needs to be converted into hours
    RA = RA / 15;

    //6. calculate the Sun's declination
    float sinDec = 0.39782 * sin((PI/180)*L);
    float cosDec = cos(asin(sinDec));

    //7a. calculate the Sun's local hour angle
    float cosH = (sin((PI/180)*ZENITH) - (sinDec * sin((PI/180)*lat))) / (cosDec * cos((PI/180)*lat));

    if (cosH >  1)
      g_print("Never rising\n");
    else if (cosH < -1)
      g_print("Never setting\n");

    //7b. finish calculating H and convert into hours
    float H = 360 - (180/PI)*acos(cosH);   //   if if rising time is desired:

    H = H / 15;

    //8. calculate local mean time of rising/setting
    float T = H + RA - (0.06571 * t) - 6.622;

    //9. adjust back to UTC
    float UT = fmod(T - lngHour,24.0);

    if ( UT < 0 )
      UT = UT+24;
    else if (UT > 24)
      UT = UT-24;

    //10. convert UT value to local time zone of latitude/longitude
    return UT + localOffset;

    }

float calculateSunset(int year,int month,int day,float lat, float lng,int localOffset) {
   /*
    localOffset will be <0 for western hemisphere and >0 for eastern hemisphere
    daylightSavings should be 1 if it is in effect during the summer otherwise it should be 0
    */
    //1. first calculate the day of the year
    float N1 = floor(275 * month / 9);
    float N2 = floor((month + 9) / 12);
    float N3 = (1 + floor((year - 4 * floor(year / 4) + 2) / 3));
    float N = N1 - (N2 * N3) + day - 30;

    //2. convert the longitude to hour value and calculate an approximate time
    float lngHour = lng / 15.0;

    float t = N + ((18 - lngHour) / 24);   //if setting time is desired:

    //3. calculate the Sun's mean anomaly
    float M = (0.9856 * t) - 3.289;

    //4. calculate the Sun's true longitude
    float L = fmod(M + (1.916 * sin((PI/180)*M)) + (0.020 * sin(2 *(PI/180) * M)) + 282.634,360.0);

    if ( L < 0 )
      L = L+360;
    else if ( L > 360)
      L = L-360;

    //5a. calculate the Sun's right ascension
    float RA = fmod(180/PI*atan(0.91764 * tan((PI/180)*L)),360.0);

    if ( RA < 0 )
      RA = RA+360;
    else if ( RA > 360)
      RA = RA-360;

    //5b. right ascension value needs to be in the same quadrant as L
    float Lquadrant  = floor( L/90) * 90;
    float RAquadrant = floor(RA/90) * 90;
    RA = RA + (Lquadrant - RAquadrant);

    //5c. right ascension value needs to be converted into hours
    RA = RA / 15;

    //6. calculate the Sun's declination
    float sinDec = 0.39782 * sin((PI/180)*L);
    float cosDec = cos(asin(sinDec));

    //7a. calculate the Sun's local hour angle
    float cosH = (sin((PI/180)*ZENITH) - (sinDec * sin((PI/180)*lat))) / (cosDec * cos((PI/180)*lat));

    if (cosH >  1)
      g_print("Never rising\n");
    else if (cosH < -1)
      g_print("Never setting\n");


    //7b. finish calculating H and convert into hours

    float H = (180/PI)*acos(cosH); //   if setting time is desired:
    H = H / 15;

    //8. calculate local mean time of rising/setting
    float T = H + RA - (0.06571 * t) - 6.622;

    //9. adjust back to UTC
    float UT = fmod(T - lngHour,24.0);

    if ( UT < 0 )
      UT = UT+24;
    else if (UT > 24)
      UT = UT-24;

    //10. convert UT value to local time zone of latitude/longitude
    return UT + localOffset;

}

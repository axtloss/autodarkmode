#include <stdbool.h>
float to_rad (float n);
float to_deg (float n);

float calculateSun(int year,int month,int day,float lat, float lng,int localOffset, bool sunset);

float getSunrise(float lat, float lang);
float getSunset(float lat, float lang);

bool isDark (float sunrise, float sunset);

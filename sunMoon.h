#ifndef _SUN_MOON_H
#define _SUN_MOON_H

#include <Time.h>
#include <TimeLib.h>
#include <math.h>

class sunMoon {
  public:
    sunMoon() {}
    bool init(int Timezone, float Latitude, float Longitude);    // Timezone in minutes
    uint32_t julianDay(time_t date = 0);
    uint8_t moonDay(time_t date = 0);
    float moonPercent(time_t date);
    float moonPhase(time_t date);
    time_t sunRise(time_t date = 0);
    time_t sunSet(time_t date = 0);
    time_t sunRiseCivilTwilight(time_t date = 0);
    time_t sunSetCivilTwilight(time_t date = 0);
    time_t sunRiseNauticalTwilight(time_t date = 0);
    time_t sunSetNauticalTwilight(time_t date = 0);
    time_t sunRiseAstronomicalTwilight(time_t date = 0);
    time_t sunSetAstronomicalTwilight(time_t date = 0);
    enum forecast { New_Moon,Waxing_Crescent,First_Quarter,Waxing_Gibbous,Full_Moon,Waning_Gibbous,Last_Quarter,Waning_Crescent };
    sunMoon::forecast dayForecast(char mDay = -1);              // moonage [0-29]  
  private:
    int tz;                                                     // GMP offset in minutes
    float longitude, latitude;
    time_t sunTime(bool sunRise, time_t date = 0, float zenith = 90.83);
    float normalize(float v);
    const float toRad = M_PI/180.0;
    const float toDeg = 180.0/M_PI;
    const float twoPi = 2 * M_PI;
/* zenith:                Sun's zenith for sunrise/sunset
    offical      = 90 degrees 50'
    civil        = 96 degrees
    nautical     = 102 degrees
    astronomical = 108 degrees
*/
};

#endif

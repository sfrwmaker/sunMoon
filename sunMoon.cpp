#include "sunMoon.h"

bool sunMoon::init(int Timezone, float Latitude, float Longitude) {

  if ((Timezone < - 720) || (Timezone > 720))  return false;
  if ((Longitude < -180) || (Longitude > 180)) return false;
  if ((Latitude < -90) || (Latitude > 90))     return false;
  tz = Timezone;
  longitude = Longitude;
  latitude = Latitude;
  return true;
}

uint32_t sunMoon::julianDay(time_t date) {

  if (date == 0) date = now();
  date -= tz*60;
  long y = year(date);
  long m = month(date);
  if (m > 2) {
    m = m - 3;
  } else {
    m = m + 9;
    y--;
  }
  long c = y / 100L;          // Compute century
  y -= 100L * c;
  return ((uint32_t)day(date) + (c * 146097L) / 4 + (y * 1461L) / 4 + (m * 153L + 2) / 5 + 1721119L);
}

uint8_t sunMoon::moonDay(time_t date) {

 float IP = normalize((julianDay(date) - 2451550.1) / 29.530588853);
 IP *= 29.530588853;
 uint8_t age = (uint8_t)IP;
 return age;
}

sunMoon::forecast sunMoon::dayForecast(char mDay) {
  static enum forecast f[30] = {
    day_unhappy, day_happy, day_dangerous, day_happy, day_unhappy,
    day_happiest, day_happy, day_happy, day_normal, day_happiest,
    day_happy, day_unhappy, day_dangerous, day_happy, day_normal,
    day_happiest, day_dangerous, day_happy, day_dangerous, day_happiest,
    day_happy, day_unhappy, day_happy, day_normal, day_unhappy,
    day_unhappy, day_happiest, day_happy, day_unhappy, day_happy
  };

  if (mDay < 0) mDay =  moonDay();
  if ((mDay > 29) || mDay < 0) return day_normal;
  return f[mDay];

}

time_t sunMoon::sunRise(time_t date) {
  return sunTime(true, date);
}

time_t sunMoon::sunSet(time_t date) {
  return sunTime(false, date);
}

time_t sunMoon::sunTime(bool sunRise, time_t date) {

  if (date == 0) date = now();
  // Calculate the sunrise and sunset times for date and 'noon time'
  tmElements_t tm;
  breakTime(date, tm);
  tm.Hour    = 12;
  tm.Minute = 0;
  tm.Second = 0;
  date = makeTime(tm);
  date -= tz*60;

  // first calculate the day of the year
  int N1 = 275 * (month(date)) / 9;
  int N2 = (month(date)+9)/12;
  int N3 = 1 + (year(date) - 4 * (year(date)) / 4 + 2) / 3;
  int N = N1 - (N2 * N3) + day(date) - 30;
  
  // convert the longitude to hour value and calculate an approximate time
  float lngHour = longitude / 15.0;
  float t = 0;
  if (sunRise) 
    t = N + ((6 - lngHour) / 24);
  else
    t = N + ((18 - lngHour) / 24);

  // Sun's mean anomaly
  float M = (0.9856 * t) - 3.289;
  M *= toRad;

  // the Sun's true longitude
  float L = M + (1.916*toRad * sin(M)) + (0.020*toRad * sin(2 * M)) + 282.634*toRad;
  if (L < 0)      L += twoPi;
  if (L > twoPi) L -= twoPi;

  // the Sun's right ascension
  float RA = toDeg*atan(0.91764 * tan(L));
  if (RA < 0)   RA += 360;
  if (RA > 360) RA -= 360;

  // right ascension value needs to be in the same quadrant as L
  int Lquadrant  = (floor( L/M_PI_2)) * 90;
  int RAquadrant = (floor(RA/90)) * 90;
  RA += Lquadrant - RAquadrant;
  RA /= 15;         // right ascension value needs to be converted into hours

  // calculate the Sun's declination
  float sinDec = 0.39782 * sin(L);
  float cosDec = cos(asin(sinDec));

float decl = toDeg*asin(sinDec);

  // calculate the Sun's local hour angle
  float cosH = (cos(zenith) - (sinDec * sin(latitude*toRad))) / (cosDec * cos(latitude*toRad));
  if (cosH >  1) return 0;            // the Sun never rises on this location on the specified date
  if (cosH < -1) return 0;            // the Sun never sets on this location on the specified date

  // finish calculating H and convert into hours
  float H = 0;
  if (sunRise) 
    H = 360 - toDeg*acos(cosH);
  else
    H = toDeg*acos(cosH);
  H /= 15;

  // calculate local mean time of rising/setting
  float T = H + RA - (0.06571 * t) - 6.622;
  if (T < 0)  T += 24;
  if (T > 24) T -= 24;
  float UT = T - lngHour;
  float localT = UT + (float)tz / 60.0;

  tm.Hour = (uint8_t)localT;
  localT -= tm.Hour;
  localT *= 60;
  tm.Minute = (uint8_t)localT;
  localT -= tm.Minute;
  localT *= 60;
  tm.Second = (uint8_t)localT;

  time_t ret = makeTime(tm);
  return ret;  
}

float sunMoon::normalize(float v) {
  v -= floor(v); 
  if (v < 0) v += 1;
  return v;
}
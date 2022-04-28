#include <Time.h>
#include <TimeLib.h>
#include <DS3232RTC.h>
#include <sunMoon.h>

#define OUR_latitude    55.751244               // Moscow cordinates
#define OUR_longtitude  37.618423
#define OUR_timezone    180                     // localtime with UTC difference in minutes

sunMoon  sm;
DS3232RTC myRTC;

void printDate(time_t date) {
  char buff[20];
  sprintf(buff, "%2d-%02d-%4d %02d:%02d:%02d",
          day(date), month(date), year(date), hour(date), minute(date), second(date));
  Serial.print(buff);
}

void setup() {
  tmElements_t  tm;                             // specific time

  tm.Second = 0;
  tm.Minute = 12;
  tm.Hour   = 12;
  tm.Day    = 3;
  tm.Month  = 8;
  tm.Year   = 2016 - 1970;
  time_t s_date = makeTime(tm);


  Serial.begin(9600);
  myRTC.begin();
  setSyncProvider(myRTC.get);                     // the function to get the time from the RTC
  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");
  sm.init(OUR_timezone, OUR_latitude, OUR_longtitude);

  Serial.print("Today is ");
  printDate(myRTC.get()); Serial.println("");

  uint32_t jDay = sm.julianDay();               // Optional call
  byte mDay = sm.moonDay();
  time_t sRise = sm.sunRise();
  time_t sSet  = sm.sunSet();
  Serial.print("Today is "); Serial.print(jDay); Serial.println(" Julian day");
  Serial.print("Moon age is "); Serial.print(mDay); Serial.println("day(s)");
  Serial.print("Today sunrise and sunset: ");
  printDate(sRise); Serial.print("; ");
  printDate(sSet);  Serial.println("");


  Serial.print("Specific date was ");
  printDate(s_date); Serial.println("");
  jDay = sm.julianDay(s_date);
  mDay = sm.moonDay(s_date);
  sRise = sm.sunRise(s_date);
  sSet  = sm.sunSet(s_date);
  Serial.print("Specific date sunrise and sunset was: ");
  Serial.print("Julian day of specific date was "); Serial.println(jDay);
  Serial.print("Moon age was "); Serial.print(mDay); Serial.println("day(s)");
  printDate(sRise); Serial.print("; ");
  printDate(sSet);  Serial.println("");

}

void loop() {
  // put your main code here, to run repeatedly:

}

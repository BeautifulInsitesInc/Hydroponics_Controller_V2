#include <RTClib.h> // for DS3231 Real Time Clock
#include "time.h"

RTC_DS3231 rtc; 
DateTime now;

// Variables
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int year, month, day, dayofweek, minute, second, hour,twelvehour; // hour is 24 hour, twelvehour is 12 hour format
bool ispm; // yes = PM
String am_pm;
String current_time;
const long gmtOffset_sec = -18000; // to adjust -5 from UTC for EST time
const int daylightOffset_sec = 3600;
const char* ntpServer = "pool.ntp.org";// NTP server to request epoch time


void initalize_rtc(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  /*
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)); //January 21, 2014 at 3am you would call:
  } 
  */

  // Set time from internet
  struct tm timeinfo;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if(!getLocalTime(&timeinfo)){  // failed to get time, so just keep clock where it is at
    Serial.println("Failed to obtain time");
    return;
  }
  else { // set the clock to correct time
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    Serial.print("Hour : "); Serial.println(timeinfo.tm_hour);
    Serial.print("month : "); Serial.println(timeinfo.tm_mon);
    Serial.print("wday : "); Serial.println(timeinfo.tm_wday);
    Serial.print("mday : "); Serial.println(timeinfo.tm_mday);
    rtc.adjust(DateTime(timeinfo.tm_year, timeinfo.tm_mon, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
  }



}

void setTimeVariables() {
  DateTime now = rtc.now();
  year = now.year(); month = now.month(); day = now.day(); dayofweek = now.dayOfTheWeek(); hour = now.hour(); twelvehour = now.twelveHour(); ispm =now.isPM(); minute = now.minute(); second = now.second();
}

void printDigits(int digit) {// To alwasy display time in 2 digits
  //Serial.print(":");
  tft.print(":");
  if(digit < 10) {
    //Serial.print('0');
    tft.print('0');
  }
  //Serial.print(digit);
  tft.print(digit);
}

void displayTime() { // Displays time in proper format
  if (twelve_hour_clock == true) {
    hour = twelvehour;
    if (ispm == true) am_pm = "PM";
    else am_pm = "AM";
  }
  //Serial.print(hour);
  tft.setCursor(340,0); tft.setTextSize(2);
  if (hour < 10) {
    tft.print(" "); tft.print(hour);
  }
  else tft.print(hour);
  printDigits(minute);
  if (display_seconds == true) printDigits(second);
  if (twelve_hour_clock == true) tft.print(am_pm);
  //Serial.println();
}

// ----get epoch time

unsigned long epochTime; // Variable to save current epoch time

unsigned long getTime() {// Function that gets current epoch time
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

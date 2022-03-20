// =======================================================
// ========== RTC Functions ==============================
// =======================================================
RTC_DS3231 rtc; 
DateTime now;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool display_seconds = false;
int year, month, day, dayofweek, minute, second, hour,twelvehour; // hour is 24 hour, twelvehour is 12 hour format
bool ispm; // yes = PM
String am_pm;
String current_time;

void initalize_rtc(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)); //January 21, 2014 at 3am you would call:
  } 
  DateTime uptime = rtc.now(); // Set time at boot up
}

void setTimeVariables() {
  DateTime now = rtc.now();
  year = now.year(); month = now.month(); day = now.day(); dayofweek = now.dayOfTheWeek(); hour = now.hour(); twelvehour = now.twelveHour(); ispm =now.isPM(); minute = now.minute(); second = now.second();
}

void printDigits(int digit) {// To alwasy display time in 2 digits
  Serial.print(":");
  if(digit < 10) Serial.print('0');
  Serial.print(digit);
}

void displayTime() { // Displays time in proper format
  if (twelve_hour_clock == true) {
    hour = twelvehour;
    if (ispm == true) am_pm = "PM";
    else am_pm = "AM";
  }
  Serial.print(hour);
  printDigits(minute);
  if (twelve_hour_clock == true) Serial.print(am_pm);
  if (display_seconds == true) printDigits(second);
}

// ----get epoch time
const char* ntpServer = "pool.ntp.org";// NTP server to request epoch time
unsigned long epochTime; // Variable to save current epoch time
// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

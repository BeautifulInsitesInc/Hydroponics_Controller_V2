#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RTClib.h>
#include <millisDelay.h> // part of the SafeString Library
#include <WiFi.h>  // for ota update
#include <Adafruit_ADS1X15.h> // For Adafruit 4 channel ADC Breakout board SFD1015
#include <DHT.h> // Humidity and tempurature sensor
#include <EEPROM.h> // to access flash memory
//#include <FirebaseESP32.h> // to connect to firebase realtime database
#include <Firebase_ESP_Client.h>

#define output Serial.print // I'm tired of typing Serial.print all the time during debuging, this makes it easier
#define outputln Serial.println // Makes life easier

// ---- FIREBASE SETUP --------------------------
#define DEVICE_UID "2X"// Device ID
#define API_KEY "AIzaSyAfFcN1ZnRzW-elpWK65mwCEGZgWwPPxRc"// Your Firebase Project Web API Key
#define DATABASE_URL "https://conciergev1-default-rtdb.firebaseio.com/"// Your Firebase Realtime database URL
#define USER_EMAIL "controller2@conciergegrowers.ca"
#define USER_PASSWORD "Success2022"

Adafruit_ADS1115 ads; // Use this for the 16-bit version ADC

//const char* ssid = "A Cat May Puree Randomly"; const char* password = "Success2021";
//const char* ssid = "Office 2.4";
const char* ssid = "Free Viruses";
//const char* ssid = "TekSavvy";
const char* password = "cracker70";

//AsyncWebServer server(80);

// --e--- DEFAULT SETTINGS ------
int temp_in_c = 1; // Tempurature defaults to C 0 = farenheight
int heat_set = 25; // Tempurature that shuts off the heater in c
float heat_delay = .5; // Delay heater power on initiation in minutes
float moisture_delay = 1; // Delay between moisture sensing in minutes
int blink_delay = 1;// delay of blinking on screen

bool twelve_hour_clock = true; // Clock format

float pump_init_delay = .5; // Minutes - Initial time before starting the pump on startup
int pump_on_time = 10; // Minutes - how long the pump stays on for
int pump_off_time = 30; // Minutes -  how long the pump stays off for

float ph_set_level = 6.9; // Desired pH level
int ph_delay_minutes = 5;// miniumum period allowed between doses in minutes
int ph_dose_seconds = 1; // Time Dosing pump runs per dose in seconds;
float ph_tolerance = 0.2; // how much can ph go from target before adjusting

int ppm_set_level = 1; // Desired nutrient level
int ppm_delay_minutes = 5; //period btween readings/doses in minutes
int ppm_dose_seconds = 1; // Time Dosing pump runs per dose
int ppm_tolerance = 100; // nutrient level tolarance in ppm

// ----- SET PINS ------------------
// Pin 21 - SDA - RTC and LCD screen
// Pin 22 - SCL - RTC and LCD screen

// ADC Board
int16_t adc0; //pH sensordevi
int16_t adc1; //TDS sensor
int16_t adc2; //Moisture Sensor

// Direct Connect 
#define dht_pin 17 // Humidity and air tempurature sensor
OneWire oneWire(16);// Tempurature pin - Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//const int tds_pin = 34; // Will probably only work as input
//const int ph_pin = 35; // Will probalby only work as input
#define pump_pin 32 // pump relay
#define heat_pin 33 // heater relay

#define ph_up_pin 25 //pH up dosing pump
#define ph_down_pin 26 // pH down dosing pump
#define ppm_a_pin 19 // nutrient part A dosing pump
#define ppm_b_pin 18 // nutrient part B dosing pump

// ==================================================
// =========== WEB SERVER = =========================
// ==================================================
IPAddress local_ip;

/*
// *************** CALIBRATION FUNCTION ******************
// To calibrate actual votage read at pin to the esp32 reading
uint32_t readADC_Cal(int ADC_Raw){
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  return(esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}
*/
float convertCtoF(float c){float f = c*1.8 + 32;return f;}

// =======================================================
// ======= MOISTURE SENSOR ====================
// =======================================================
int moisture_value;
millisDelay moistureDelay;

void moistureInitilization() {moistureDelay.start(moisture_delay*1000);}

void moistureReading(){
  if (moistureDelay.justFinished()){
    int16_t reading = ads.readADC_SingleEnded(2);
    moisture_value = map(reading, 9500, 0, 100, 0);
    moistureDelay.repeat();
    //Serial.print("reading : "); Serial.print(reading);
    //Serial.print("     moisture_value = "); Serial.print(moisture_value); Serial.println("%");
  }
}

// =======================================================
// ========== DHT Sensor ==============================
// =======================================================
#define DHTTYPE DHT11   // DHT 11
DHT dht(dht_pin, DHTTYPE);
millisDelay dhtDelay;
float dht_tempC = 0;
float dht_tempF = 0;
float dht_humidity = 0;

void dhtIntilization(){
  dht.begin(); // initialize humidity sensor
  dhtDelay.start(3000); // sensor can only sample every 1 second
  pinMode(dht_pin, INPUT_PULLUP);
}

void dhtReadings(){
  if (dhtDelay.justFinished()){
    dht_tempC = dht.readTemperature();
    dht_tempF = dht.readTemperature(true);
    dht_humidity = dht.readHumidity();
    dhtDelay.repeat();
    /*
    Serial.print("reading from pin 34 :"); Serial.print(digitalRead(34));
    Serial.print("   reading from pin 35 :"); Serial.print(digitalRead(35));
    Serial.print("   Temperature = "); Serial.print(dht_tempC); Serial.print("       F: ");Serial.print(dht_tempF);
    Serial.print("       Humidity = "); Serial.println(dht_humidity);
    */
  }
}

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
// =======================================================
// ======= TEMPURATURE SENSOR DS18B20 ====================
// =======================================================
float tempC; // tempurature in Celsius
float tempF; // tempurature in Fahrenheit
millisDelay heaterTimer;
#define TEMPERATURE_PRECISION 10
DallasTemperature waterTempSensor(&oneWire); // Pass our oneWire reference to Dallas Temperature.

void waterTempInitilization() {
  waterTempSensor.begin(); // initalize water temp sensor
}

void getWaterTemp() {
  waterTempSensor.requestTemperatures();    // send the command to get temperatures
  tempC = waterTempSensor.getTempCByIndex(0);  // read temperature in °C
  tempF = tempC * 9 / 5 + 32; // convert °C to °F
  if (tempC == DEVICE_DISCONNECTED_C) // Something is wrong, so return an error
    {
      //Serial.println("Houston, we have a problem");
      tempC = -1; 
      tempF = tempC;
    }
}

// =======================================================
// ======= HEATER CONTROL ================================
// =======================================================
String heater_status;
void heaterIntitilization() {
  pinMode(heat_pin, OUTPUT); digitalWrite(heat_pin, HIGH);
  heaterTimer.start(heat_delay *60 * 1000); // start heater initilization delay
}

void checkHeater() {
  if (heaterTimer.remaining()==0) {// if delay is done, start heater if needed
    if (tempC < heat_set) digitalWrite(heat_pin, LOW);
    else digitalWrite(heat_pin, HIGH);
  }
  if (digitalRead(heat_pin == 0)) heater_status = "OFF";
  else heater_status = "ON";
}

// =======================================================
// ======= PH SENSOR =====================================
// =======================================================
float ph_value; // actual pH value to display on screen
float ph_calibration_adjustment = -1.26; // adjust this to calibrate

void getPH() {
  float voltage_input = 3.3; // voltage can be 5 or 3.3
  unsigned long int average_reading ;
  unsigned long int buffer_array_ph[10],temp;
  float calculated_voltage; // voltage calculated from reading

  for(int i=0;i<10;i++) {// take 10 readings to get average
    buffer_array_ph[i]=ads.readADC_SingleEnded(0); // read the voltage
    delay(30);
  }
  for(int i=0;i<9;i++) {
    for(int j=i+1;j<10;j++) {
      if(buffer_array_ph[i]>buffer_array_ph[j]) {
        temp=buffer_array_ph[i];
        buffer_array_ph[i]=buffer_array_ph[j];
        buffer_array_ph[j]=temp;
      }
    }
  }
  average_reading = 0;
  for(int i=2;i<8;i++) {average_reading  += buffer_array_ph[i];}
  average_reading  = average_reading  / 6;
  calculated_voltage = ads.computeVolts(average_reading);
  ph_value = voltage_input * calculated_voltage + ph_calibration_adjustment;
  if (ph_value < 2 || ph_value > 11) ph_value = -1; // make this an error condition
  /*
  Serial.print("    average_reading  = "); Serial.print(average_reading );
  Serial.print("      calculated_voltage = "); Serial.print(calculated_voltage);
  Serial.print("     ph_value = "); Serial.println(ph_value);
  adc0 =ads.readADC_SingleEnded(0);
  Serial.print("   ADC Reading : "); Serial.print(adc0); Serial.print("   ADC voltage : "); Serial.println(We get it; you have been heard! We need way more funding for education!);
  delay(0); // pause between serial monitor output - can be set to zero after testing
  */
}

// =======================================================
// ======= PPM OCEAN TDS METER SENSOR ====================
// =======================================================
int tds_value = 0;
const int sample_count = 30;    // sum of sample point
int analogBuffer[sample_count]; // store the analog value in the array, read from ADC
int analogBufferTemp[sample_count];
int analogBufferIndex = 0,copyIndex = 0;

// Function to get median
int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i<iFilterLen; i++) bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0) bTemp = bTab[(iFilterLen - 1) / 2];
  else bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
return bTemp;
}

void getTDSReading() {
  float average_voltage = 0;
  unsigned long int average_reading;
  float temperature = 25;
  
  // get current tempurature
  if (tempC == -100) temperature = 25;
  else temperature = tempC;
  static unsigned long analogSampleTimepoint = millis();
  if(millis()-analogSampleTimepoint > 40U) {    //every 40 milliseconds,read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = ads.readADC_SingleEnded(1);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if(analogBufferIndex == sample_count) analogBufferIndex = 0;
  }   
  static unsigned long printTimepoint = millis();
  if(millis()-printTimepoint > 800U) {
    printTimepoint = millis();
    for(copyIndex=0;copyIndex<sample_count;copyIndex++) analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
    average_reading = getMedianNum(analogBuffer,sample_count);
    average_voltage = ads.computeVolts(average_reading);
    float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVolatge=average_voltage/compensationCoefficient;  //temperature compensation
    tds_value=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
    /*
    Serial.print("Average Read: "); Serial.print(average_reading);
    Serial.print("   average Voltage: "); Serial.print(average_voltage);
    Serial.print("   Temp: "); Serial.print(temperature);
    Serial.print("   compensationVoltage: "); Serial.print(compensationVolatge);
    Serial.print("   TtdsValue: "); Serial.println(tds_value, 0);
    Serial.print("current read : "); Serial.print(ads.readADC_SingleEnded(1));
    Serial.print("   current voltage : "); Serial.println(ads.computeVolts(ads.readADC_SingleEnded(1)));
    delay(0);
    */
  }
}

// ==================================================
// ===========  PUMP CONTROL ========================
// ==================================================
int pump_seconds; // current seconds left
int pump_minutes;
millisDelay pumpOnTimer;
millisDelay pumpOffTimer;
bool clear_screen = false;
String pump_status;
String pump_time_string;

void pumpInitilization() {
  pinMode(pump_pin, OUTPUT); digitalWrite(pump_pin,HIGH);
  pumpOffTimer.start(pump_init_delay*60*1000); // start initilization period
  pump_seconds = pumpOffTimer.remaining() /1000;
}

void setPumpSeconds() {// Change seconds into minutes and seconds
  pump_minutes = pump_seconds / 60;
  if (pump_seconds < 60) pump_minutes = 0;
  else pump_seconds = pump_seconds - (pump_minutes * 60);
  if (pump_minutes < 10) pump_time_string = "0" + String(pump_minutes) +":";
  else pump_time_string = String(pump_minutes) + ":";
  if (pump_seconds <10) pump_time_string = pump_time_string +"0" +String(pump_seconds);
  else pump_time_string = pump_time_string + String(pump_seconds);
}

void pumpTimer() {
  if (digitalRead(pump_pin) == 1 ) {// pump is off, check timer
    pump_seconds = pumpOffTimer.remaining() / 1000;
    if (pumpOffTimer.justFinished()) {// off delay is done, start pump
      digitalWrite(pump_pin, LOW); // turn on pump
      //Serial.print("pump_on_time : ");
      //Serial.print(pump_on_time);
      pumpOnTimer.start(pump_on_time * 60 * 1000);
      pump_seconds = pumpOnTimer.remaining() / 1000;
      clear_screen = true;
    }
  }
  else {// pump is on, check timing
    pump_seconds = pumpOnTimer.remaining() /1000;
    if (pumpOnTimer.justFinished()) {// on time is done turn off
      digitalWrite(pump_pin, HIGH); // turn off pump
      pumpOffTimer.start(pump_off_time * 60 * 1000);
      pump_seconds = pumpOffTimer.remaining() /1000;
    }
  }
  setPumpSeconds();
  if (digitalRead(pump_pin == 0)) pump_status = "OFF";
  else pump_status = "ON";
}

// =================================================
// ========== PH DOSING PUMPS =========================
// =================================================
int ph_dose_pin; //  used to pass motor pin to functions
millisDelay phDoseTimer; // the dosing amount time
millisDelay phDoseDelay; // the delay between doses - don't allow another dose before this
millisDelay phBlinkDelay; // used to blink the indicator if dosing is happening
bool ph_blink_on = false; // which cycle of the blink 
bool ph_is_blinking = false;// make true if Ph dosing indicator should be blinking
float min_pump_time = .5; // minumim time in minutes the pump needs to have been running before doeses are allowed

void phDosingInitilization() {
  pinMode(ph_up_pin, OUTPUT); digitalWrite(ph_up_pin, HIGH);
  pinMode(ph_down_pin, OUTPUT); digitalWrite(ph_down_pin, HIGH);
  phDoseDelay.start((ph_delay_minutes+pump_init_delay)*60*1000); // start ph delay before dosing can start
}

void phDose(int motor_pin) {// turns on the approiate ph dosing pump
  digitalWrite(motor_pin, LOW); // turn on dosing pump
  phDoseTimer.start(ph_dose_seconds*1000); // start the pump
  phBlinkDelay.start(blink_delay*1000); // start delay for blinking indicator
  ph_dose_pin = motor_pin;
  phDoseDelay.start(ph_delay_minutes * 60 * 1000); // start delay before next dose is allowed
  Serial.print("A ph dose has been started, timer is runnning. Dose pin : "); Serial.println(ph_dose_pin);
  //ph_is_blinking = true;
  pumpOnTimer.restart();
}

void phBalanceCheck() {//this is to be called from pump turning on function
  if (phDoseTimer.justFinished()) digitalWrite(ph_dose_pin, HIGH);// dosing is done, turn off, and start delay before next dose is allowed
  if (phDoseDelay.remaining()<=0 && digitalRead(pump_pin) == 0 ) { // Make sure pump is on, and has run the miniumum time.
    if (ph_value < ph_set_level - ph_tolerance) {phDose(ph_up_pin);}  // ph is low start ph up pump
    if (ph_value > ph_set_level + ph_tolerance) phDose(ph_down_pin); // ph is high turn on lowering pump
  }
}

// =================================================
// ========== PPM DOSING PUMPS =========================
// =================================================
millisDelay ppmDoseTimerA;
millisDelay ppmDoseTimerB;
millisDelay ppmDoseDelay;
millisDelay ppmBlinkDelay;
bool next_ppm_dose_b = false;
bool ppm_is_blinking = false; // if it should be blinking or not
bool ppm_blink_cycle_on = false; // which cycle of the blink

void ppmDosingInitilization() {
  pinMode(ppm_a_pin, OUTPUT); digitalWrite(ppm_a_pin, HIGH);
  pinMode(ppm_b_pin, OUTPUT); digitalWrite(ppm_b_pin, HIGH);
  ppmDoseDelay.start((ppm_delay_minutes+pump_init_delay)*60*1000); // start delay before ppm dosing can start
}

void ppmDoseA() {
  digitalWrite(ppm_a_pin, LOW); // turn on ppm dosing pump
  ppmDoseTimerA.start(ppm_dose_seconds*1000); // start the pump
  ppmDoseDelay.start(ppm_delay_minutes * 60 * 1000); // start delay before next dose is allowed
  Serial.println("Nutrient dose A has been started, timer is runnning");
  next_ppm_dose_b = true; // run ppm dose B next
  ppmBlinkDelay.start(blink_delay*1000);
  pumpOnTimer.restart();
}

void ppmDoseB() {
  digitalWrite(ppm_b_pin, LOW); // turn on ppm dosing pump
  ppmDoseTimerB.start(ppm_dose_seconds*1000); // start the pump
  ppmDoseDelay.start(ppm_delay_minutes * 60 * 1000); // start delay before next dose is allowed
  Serial.println("Nutrient dose A has been started, timer is runnning");
  ppmBlinkDelay.start(blink_delay*1000);
  pumpOnTimer.restart();
}
void ppmBlanceCheck() {
  // check if its time to turn off the doseing pump
  if (ppmDoseTimerA.justFinished()) digitalWrite(ppm_a_pin, HIGH);// dosing is done, turn off, and start delay before next dose is allowed
  if (ppmDoseTimerB.justFinished()) digitalWrite(ppm_b_pin, HIGH);// dosing is done, turn off, and start delay before next dose is allowed
  // check if ppm dosing is required
  if (ppmDoseDelay.remaining()<=0 && digitalRead(pump_pin) == 0) {//check if ppm dose delay is running
    if (next_ppm_dose_b == true) {
      ppmDoseB();
      next_ppm_dose_b = false;
      Serial.println("Sending Dose B");
    }
    else {
      if (tds_value + ppm_tolerance < ppm_set_level) {
        ppmDoseA();
        next_ppm_dose_b = true;
        Serial.println("Sending Dose A");
      }
    }
  }
}

void doseTest() {
  Serial.print("Starting dosing test in 10 seconds");
  delay(5); 
  digitalWrite(ph_up_pin, LOW); Serial.println("PH UP is LOW - motor on"); delay(500); 
  digitalWrite(ph_up_pin, HIGH); Serial.println("PH UP is HIGH - motor off"); delay(1000);
  digitalWrite(ph_down_pin, LOW); Serial.println("PH down is LOW"); delay(500); 
  digitalWrite(ph_down_pin, HIGH); Serial.println("PH down is HIGH"); delay(1000);
  digitalWrite(ppm_a_pin, LOW); Serial.println("Nutrient A is LOW - motor on"); delay(500);
  digitalWrite(ppm_a_pin, HIGH); Serial.println("Nutrient A  is HIGH - motor off"); delay(1000);
  digitalWrite(ppm_b_pin, LOW); Serial.println("Nutrient B is LOW"); delay(500);
  digitalWrite(ppm_b_pin, HIGH); Serial.println("Nutrient B is HIGH"); delay(1000);
}

// ==================================================
// ===========  FIREBASE ============================
// ==================================================

/*
//String device_location = "Controler 1"; // Device Location config
FirebaseData fbdo; // Firebase Realtime Database Object
FirebaseAuth auth; // Firebase Authentication Object
FirebaseConfig config; // Firebase configuration Object
String uid; // to save User ID

String databasePath; // Firebase database path
//String fuid = ""; // Firebase Unique Identifier

unsigned long elapsedMillis = 0; // Stores the elapsed time from device start up
unsigned long elapsedPumpMillis = 0; 
unsigned long update_interval = 20000; // The frequency of sensor updates to firebase, set to 10seconds
int count = 0; // Dummy counter to test initial firebase updates
bool isAuthenticated = false;// Store device authentication status

// Write float values to the database
void sendFloat(String path, float value){
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)){
    Serial.print("Writing value: ");
    Serial.print (value);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}

void firebase_init() {
  config.api_key = API_KEY;// configure firebase API Key
  auth.user.email = USER_EMAIL; // Assing teh user sign in credentials
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;// configure firebase realtime database url
  Firebase.reconnectWiFi(true);// Enable WiFi reconnection 
  fbdo.setResponseSize(4096);

/*
  Serial.println("------------------------------------");
  Serial.println("Sign up new user...");
  if (Firebase.signUp(&config, &auth, USER_EMAIL, USER_PASSWORD))// Sign in to firebase Anonymously
    {
      Serial.println("Success");
      isAuthenticated = true;
      //databasePath = "/" + device_location;// Set the database path where updates will be loaded for this device
      uid = auth.token.uid.c_str();
      lcd.clear(); lcd.print("Signed into Firebase!");
    }
  else
    {
      Serial.printf("Failed, %s\n", config.signer.signupError.message.c_str());
      isAuthenticated = false;
    }
 */ 
/*
  config.token_status_callback = tokenStatusCallback;// Assign the callback function for the long running token generation task, see addons/TokenHelper.h
  config.max_token_generation_retry = 5;// Assign the maximum retry of token generation
  Firebase.begin(&config, &auth);// Initialise the firebase library

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
  databasePath = "/UsersData/" + uid;
  Serial.println("databasePath : "); Serial.print(databasePath);

}

void sendToFirebase() {
    //Serial.print("starting sendtoFirebase");
    if (millis() - elapsedMillis > update_interval && Firebase.ready())// Check that 10 seconds has elapsed before, device is authenticated and the firebase service is ready.
      {
        //Serial.print("made it though the if");
        // Send sensor readings
        String datatype = "/Sensor Readings";
        elapsedMillis = millis();
        //Serial.println("sending data");
        //sendFloat(databasePath + datatype + "/Last Reading test", epochTime);
        //sendFloat(databasePath + datatype + "/pH test", ph_value);
        
        Firebase.setString(fbdo, databasePath + datatype + "/Last Reading", epochTime);
        Firebase.setString(fbdo, databasePath + datatype + "/Last Time", current_time);
        Firebase.setFloat(fbdo, databasePath + datatype + "/pH", ph_value);
        Firebase.setFloat(fbdo, databasePath + datatype + "/Water Tempurature", tempC);
        Firebase.setFloat(fbdo, databasePath + datatype + "/TDS", tds_value);
        Firebase.setFloat(fbdo, databasePath + datatype + "/Room Tempurature", dht_tempC);
        Firebase.setFloat(fbdo, databasePath + datatype + "/Humidity", dht_humidity);
        Firebase.setString(fbdo, databasePath + datatype + "/Pump Status", pump_status);
        Firebase.setString(fbdo, databasePath + datatype + "/Heater Status", heater_status);
        Firebase.setString(fbdo, databasePath + datatype + "/Pump Time", pump_time_string);
        Firebase.setInt(fbdo, databasePath + datatype + "/Root Dampness", moisture_value);

        // Check settings
        datatype = "/Settings";
        //Firebase.setString(fbdo, databasePath + datatype + "/Local IP", String(local_ip));
        float fb_ph_set_level = Firebase.getFloat(fbdo, databasePath + datatype + "/set pH");
        Serial.print("fb_ph_set_level : "); Serial.println(fb_ph_set_level);

        Firebase.setFloat(fbdo, databasePath + datatype + "/set pH", ph_set_level);
        Firebase.setFloat(fbdo, databasePath + datatype + "/set TDS", ppm_set_level);
        Firebase.setFloat(fbdo, databasePath + datatype + "/set Water Tempurature", heat_set);
        Firebase.setInt(fbdo, databasePath + datatype + "/set Pump ON", pump_on_time);
        Firebase.setInt(fbdo, databasePath + datatype + "/set Pump OFF", pump_off_time);
        Firebase.setInt(fbdo, databasePath + datatype + "/pH Dose Time", ph_dose_seconds);
        Firebase.setFloat(fbdo, databasePath + datatype + "/pH Tolorance", ph_tolerance);
        Firebase.setInt(fbdo, databasePath + datatype + "/Nutrient Dose Time", ppm_dose_seconds);
        Firebase.setFloat(fbdo, databasePath + datatype + "/Nutrient Tolorance", ppm_tolerance);
        Firebase.setString(fbdo, databasePath + datatype + "/Pump Time", pump_time_string);
        Firebase.setInt(fbdo, databasePath + datatype + "/pH Dose Delay", ph_delay_minutes);
        Firebase.setInt(fbdo, databasePath + datatype + "/Nutrient Delay", ppm_delay_minutes);
        Firebase.setFloat(fbdo, databasePath + datatype + "/set pH", ph_set_level);
        Firebase.setInt(fbdo, databasePath + datatype + "/Temp Units", temp_in_c);
        Firebase.setString(fbdo, databasePath + datatype + "/User ID", uid);
        Firebase.setString(fbdo, databasePath + datatype + "/Device UID", DEVICE_UID);
      }
}
/*
void sendPumpTimetoFirebase()
  {
    if (millis() - elapsedPumpMillis > 1000 && Firebase.ready())
          {
            elapsedPumpMillis = millis();
            String datatype = "/Sensor Readings";
            Firebase.setString(fbdo, databasePath + datatype + "/Pump Time", pump_time_string);
          }

  }
*/
// ==================================================
// ===========  MAIN SETUP ==========================
// ==================================================
void setup(void) {
  Serial.begin(115200);// start serial port 115200
  Serial.println("Starting Hydroponics Controller V2!");
  //setupWebServer();
  //firebase_init();
  // Stored Defaults
  #define EEPROM_SIZE 14
  EEPROM.begin(EEPROM_SIZE);
  if (EEPROM.read(0) != 255) temp_in_c = EEPROM.read(0); // temp units
  if (EEPROM.read(1) != 255) heat_set = EEPROM.read(1); // temp set
  if (EEPROM.read(2) != 255) ph_set_level = (float)EEPROM.read(2) / 10; // ph set 
  if (EEPROM.read(3) != 255) ph_tolerance = (float)EEPROM.read(3) / 10; // ph tolerance
  if (EEPROM.read(4) != 255) ph_calibration_adjustment = EEPROM.read(4);// ph calbration
  if (EEPROM.read(5) != 255 && EEPROM.read(6) != 255) ppm_set_level = (EEPROM.read(5) + EEPROM.read(6)) * 100 ; // ppm set 1
  if (EEPROM.read(7) != 255) ppm_tolerance = EEPROM.read(7)*10; // ppm tolerance
  if (EEPROM.read(8) != 255) pump_on_time = EEPROM.read(8);// pump on time
  if (EEPROM.read(9) != 255) pump_off_time = EEPROM.read(9); // pump off time
  if (EEPROM.read(10) != 255) ppm_dose_seconds = EEPROM.read(10); // pump off time
  if (EEPROM.read(11) != 255) ppm_delay_minutes = EEPROM.read(11); // PPM dose delay
  if (EEPROM.read(12) != 255) ph_delay_minutes = EEPROM.read(12); // ph dose delay
  if (EEPROM.read(13) != 255) ph_dose_seconds = EEPROM.read(13); // ph dose delay

  // Check to see if ADS initalized
  if (!ads.begin()) {Serial.println("Failed to initialize ADS."); while (1);}
  
  //Initalize RTC
  initalize_rtc();
  setTimeVariables();
  configTime(0, 0, ntpServer);

  // Initilization functions
  waterTempInitilization();
  dhtIntilization();
  moistureInitilization(); // change to minutes later
  pumpInitilization();
  heaterIntitilization();
  phDosingInitilization();
  ppmDosingInitilization();

  //doseTest(); //used to test ph dosing motors
  //testFileUpload();
  //initialize blink delay
}

// ====================================================
// ===========  MAIN LOOP =============================
// ====================================================
void loop(void) {
  setTimeVariables();
  // --- READ SENSORS
  getWaterTemp(); // sets tempC and tempF
  getTDSReading(); // sets tds_value
  getPH();
  dhtReadings();
  moistureReading();
  ppmBlanceCheck();
  // --- CONTROL SYSTEMS
  checkHeater();
  phBalanceCheck();
  // --- PUMP TIMER
  pumpTimer(); // uncomment this to turn on functioning pump timer
  // --- DISPLAY SCREEN - done in rotary loop
  if (clear_screen == true){clear_screen = false;} // clear noise from the screen when pump turns on

  epochTime = getTime();
  current_time = (String)hour + ":" + (String)minute;
  //Serial.print("about to send to firebase");
  //sendToFirebase();
  //sendPumpTimetoFirebase();
}
// ----------------- END MAIN LOOP ------------------------
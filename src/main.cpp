#include <Arduino.h>
#include <main.h>

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
#define TFT_GREY 0x5AEB // New colour

//--- SHORT CUTS ---------------------------------
#define out Serial.print // I'm tired of typing Serial.print all the time during debuging, this makes it easier
#define outln Serial.println // Makes life easier

#include <wifikeys.h> //coconst char* ssid = "wifi ssid"; const char* password = "your password"
IPAddress local_ip; // To Capture IP address for display on screen

void initWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}
// ======= SPECIAL FUNCTIONS =============================
float convertCtoF(float c){float f = c*1.8 + 32;return f;} // Convert default C into F


#include <sensor_temp.h>
#include <rtc.h>
#include <sensor_ph.h>
#include <sensor_tds.h>
#include <relays.h>
#include <dosing.h>
#include <firebase.h>


// ==================================================
// ===========  MAIN SETUP ==========================
// ==================================================
void setup(void) {
  Serial.begin(115200);// start serial port 115200
  Serial.println("Starting Hydroponics Controller V2!");
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 2);
  // Set the font colour to be white with a black background, set text size multiplier to 1
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);
  // We can now plot text on screen using the "print" class
  tft.println("Hello World! This is my screen");
  tft.setTextSize(4);
  tft.println("This is bigger");

  checkADS();

  initWiFi();
  initFirebase();

  
  //setupWebServer();


/*
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
  if (EEPROM.read(12) != 255) ph_dose_delay = EEPROM.read(12); // ph dose delay
  if (EEPROM.read(13) != 255) ph_dose_time = EEPROM.read(13); // ph dose delay
*/
  // Check to see if ADS initalized
  //if (!ads.begin()) {Serial.println("Failed to initialize ADS."); while (1);}
  
  //Initalize RTC
  //initalize_rtc();
  //setTimeVariables();
  //configTime(0, 0, ntpServer); // for epoch time function

  // Initilization functions
  initDHT();
  initWaterTemp();
  initPH();
  //moistureInitilization(); // change to minutes later
  //pumpInitilization();
  //heaterIntitilization();
  //phDosingInitilization();
  //ppmDosingInitilization();

  //doseTest(); //used to test ph dosing motors
  //testFileUpload();
  //initialize blink delay
}

// ====================================================
// ===========  MAIN LOOP =============================
// ====================================================
void loop(void) {
  getDHTReadings(); // Room temp and humidity
  getWaterTemp(); // Sets water temp C and F variables
  getPHReading();
  //getTDSReading(); // sets tds_value
  //getMoistureReading();

  sendToFirebase();
  //setTimeVariables();
  // --- READ SENSORS
  

  
 
  

  //ppmBlanceCheck();
  // --- CONTROL SYSTEMS
  //checkHeater();
 //phBalanceCheck();
  // --- PUMP TIMER
  //pumpTimer(); // uncomment this to turn on functioning pump timer
  
  //current_time = (String)hour + ":" + (String)minute;
  //Serial.print("about to send to firebase");
  //sendToFirebase();
  //sendPumpTimetoFirebase();
}
// ----------------- END MAIN LOOP ------------------------
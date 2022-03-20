#include <Arduino.h>
#include "main.h"

#include "keys/keys_wifi.h" //coconst char* ssid = "wifi ssid"; const char* password = "your password"
#include "web_functions.h"
#include "spiff_functions.h"

// ======= SPECIAL FUNCTIONS =============================
float convertCtoF(float c){float f = c*1.8 + 32;return f;} // Convert default C into F

#include "sensor_temp.h"
#include "rtc_functions.h"
#include "sensor_ph.h"
#include "sensor_tds.h"
#include "relay_functions.h"
#include "dosing_functions.h"
#include "firebase_functions.h"
#include "eeprom_functions.h"
#include "lcd_functions.h"



// ==================================================
// ===========  MAIN SETUP ==========================
// ==================================================
void setup(void) {
  Serial.begin(115200);// start serial port 115200
  Serial.println("Starting Hydroponics Controller V2!");
  
  spiffInit();
  spiffListFiles();
    
  lcdInit();
  lcdTest();
  
  

  //checkADS();

  //initWiFi();
  //initFirebase();

  
  //setupWebServer();

//loadEepromValues();

  // Check to see if ADS initalized
  //if (!ads.begin()) {Serial.println("Failed to initialize ADS."); while (1);}
  
  //Initalize RTC
  initalize_rtc();
  setTimeVariables();
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

  //sendToFirebase();

  //setTimeVariables();
  //displayTime();
  delay(5000);
  

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
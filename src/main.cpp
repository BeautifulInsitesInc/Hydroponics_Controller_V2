#include <Arduino.h>
//test

// ===== INCLUDE FILES===================
#include "main.h"
#include "lcd_functions.h"
#include "sensor_temp.h"
#include "rtc_functions.h"
#include "sensor_ph.h"
#include "sensor_tds.h"
#include "sensor_moisture.h"
#include "relay_functions.h"
#include "dosing_functions.h"
#include "eeprom_functions.h"
#include "spiff_functions.h"

#include "web_functions.h"
#include "firebase_functions.h"

// ===========  MAIN SETUP ==========================
void setup(void) {
  Serial.begin(115200);// start serial port 115200
  Serial.println("Starting Hydroponics Controller V2!");

  if (!ads.begin()) { // Initialize ADS
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

  sensorReadingDelay.start(sensor_readings_delay*1000);


  spiffInit(); // Prepare SPIFF for file access
  spiffListFiles(); // List the files available
    
  lcdInit(); // Prepare LCD
  lcdSplashScreen(); // Show the intro splash screen
  initWiFi(); // Connect to wifi and start OTA webserver
  //initFirebase();// Connect to firebase
  initalize_rtc();// Initialize the clock
  setTimeVariables(); // Initally set time variables

  // --- Initialize sensors
  initDHT();
  waterTempSensor.begin(); // initalize water temp sensor
  moistureInitilization();
  
  
  
  //checkADS();
  //loadEepromValues();
  
  //pumpInitilization();
  //heaterIntitilization();
  //phDosingInitilization();
  //ppmDosingInitilization();

  //doseTest(); //used to test ph dosing motors
}

// ===========  MAIN LOOP =============================
void loop(void) {
  ws.cleanupClients(); // webserver
  setTimeVariables(); // update time variables
  displayTime(); //display time on screen

  if(sensorReadingDelay.justFinished()){
    getDHTReadings(); // Room temp and humidity
    getWaterTemp(); // Sets water temp C and F variables
    getPHReading(); // Set pH value
    //getTDSReading(); // sets tds_value
    getTDSCurrentReading();
    getMoistureReading();
    sensorReadingDelay.repeat();
  }
  

  //sendToFirebase(); // Send sensor readings and settings to firebase

  
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
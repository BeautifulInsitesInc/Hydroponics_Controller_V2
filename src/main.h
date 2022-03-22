#include <millisDelay.h> // part of the SafeString Library. Used for non pausing timer
#include <Wire.h> // for 12c comunication
#include <Adafruit_ADS1X15.h> // For Adafruit 4 channel ADC Breakout board SFD1015
#include <OneWire.h> // For DS18B20 Water tempurature sensor
#include <time.h> // To get epoch time
	
// =============== SET PINS =============================
// Pin 21 - SDA - RTC and LCD screen
// Pin 22 - SCL - RTC and LCD screen

// ADC Board
Adafruit_ADS1015 ads; // Use 1115  for the 16-bit version ADC (0x48)
#define ph_pin 34  // being temporarily used until new ADC comes in
int16_t adc0; //PH Sensor
int16_t adc1; //TDS sensor
//int16_t adc2; //Moisture Sensor

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

// ======== GLOBAL VARIABLES ===================

// --- Time ---
bool twelve_hour_clock = true; // Clock format
bool display_seconds = true;

int sensor_readings_delay = 5;
millisDelay sensorReadingDelay;

// --- WATER TEMPURATURE ---
bool temp_in_C = true; // True = Celcius, Fales = fahrenheit
float water_temp_C = 0; // tempurature in Celsius
float water_temp_F = 0; // tempurature in Fahrenheit
int temp_in_c = 1; // Tempurature defaults to C 0 = farenheight
String heater_status = "INIT";

//int water_temp_delay = 5; // Time between water temp readings
int heat_set = 25; // Tempurature that shuts off the heater in c
float heat_init_delay = 1; // Delay heater power on initiation in minutes

// --- DHT - ROOM TEMP AND HUMIDITY ---
float dht_tempC = -0;
float dht_tempF = 0;
float dht_humidity = 0;

int dht_delay = 5; // Time between room temp and humidity readings in seconds

// ---Moisture ---
int moisture_delay = 1; // Delay between moisture sensing in minutes

// --- PH ---
float ph_value; // actual pH value to display on screen
//int ph_delay = 5; // Time between ph readings
float ph_calibration_adjustment = -1.09; // adjust this to calibrate

float ph_set_level = 6.9; // Desired pH level
int ph_dose_delay = 5;// miniumum period allowed between doses in minutes
int ph_dose_time = 1; // Time Dosing pump runs per dose in seconds;
float ph_tolerance = 0.2; // how much can ph go from target before adjusting

// ======= TDS SENSOR =============
int tds_value = 0; // value using medium of 10 samples
int tds_current_value = 0 ; // instantaneous value

float pump_init_delay = .5; // Minutes - Initial time before starting the pump on startup
int pump_on_time = 10; // Minutes - how long the pump stays on for
int pump_off_time = 30; // Minutes -  how long the pump stays off for

int ppm_set_level = 1; // Desired nutrient level
int ppm_delay_minutes = 5; //period btween readings/doses in minutes
int ppm_dose_seconds = 1; // Time Dosing pump runs per dose
int ppm_tolerance = 100; // nutrient level tolarance in ppm

// --- PUMP ---
String pump_status = "INIT";

// --- FIREBASE ---
int firebase_delay = 10; // The frequency of sensor updates to firebase, set to 10seconds

// --- LCD Display
int sensor_description_position = 120;
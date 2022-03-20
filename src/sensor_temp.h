// =======================================================
// ======= MOISTURE SENSOR ====================
// =======================================================
/*
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
*/
// =======================================================
// ========== DHT Sensor ==============================
// =======================================================
#define DHTTYPE DHT11   // DHT 11
//#define dhtReadingDelay 1
DHT dht(dht_pin, DHTTYPE); // Currently pin 17
millisDelay dhtDelayTimer;

void initDHT(){
  dht.begin(); // initialize humidity sensor
  dhtDelayTimer.start(dht_delay*1000); // Start the delay between readings
  pinMode(dht_pin, INPUT_PULLUP);
}

void getDHTReadings(){
  if (dhtDelayTimer.justFinished()){
    dhtDelayTimer.repeat();
    if (isnan(dht.readTemperature())){
      dht_tempC = 0;
      dht_tempF = 0;
    }
    else {
      dht_tempC = dht.readTemperature();
      dht_tempF = dht.readTemperature(true);
    }
    if (isnan(dht.readHumidity())) dht_humidity = 0;
    else dht_humidity = dht.readHumidity();
    // --- debugging ----
    //Serial.print("       DHT Temp = C:"); Serial.print(dht_tempC); Serial.print("       F: "); Serial.print(dht_tempF); Serial.print("       Humidity = "); Serial.println(dht_humidity);
    // --- end debugging ---
  }
}

// =======================================================
// ======= TEMPURATURE SENSOR DS18B20 ====================
// =======================================================
millisDelay waterTempDelayTimer;
#define TEMPERATURE_PRECISION 10
DallasTemperature waterTempSensor(&oneWire); // Pass our oneWire reference to Dallas Temperature.

void initWaterTemp() {
  waterTempSensor.begin(); // initalize water temp sensor
  waterTempDelayTimer.start(water_temp_delay*1000); 
}

void getWaterTemp() {
  if (waterTempDelayTimer.justFinished()) {
    
    waterTempSensor.requestTemperatures();    // send the command to get temperatures
    water_temp_C = waterTempSensor.getTempCByIndex(0);  // read temperature in °C
    water_temp_F = water_temp_C * 9 / 5 + 32; // convert °C to °F
    if (water_temp_C == DEVICE_DISCONNECTED_C) // Something is wrong, so return an error
      {
        water_temp_C = 0; // -1 to show error
        water_temp_F = 0;
      }
    waterTempDelayTimer.repeat();
  // debugging
  //Serial.print("Water Temp : "); Serial.print(water_temp_C); Serial.print("C   "); Serial.println(water_temp_F); Serial.print("F");
  }
}
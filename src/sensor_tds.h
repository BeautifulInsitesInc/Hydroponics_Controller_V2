// =======================================================
// ======= PPM OCEAN TDS METER SENSOR ====================
// =======================================================

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
  if (water_temp_C == 0) temperature = 25;
  else temperature = water_temp_C;
  static unsigned long analogSampleTimepoint = millis();
  if(millis()-analogSampleTimepoint > 40U) {    //every 40 milliseconds,read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = ads.readADC_SingleEnded(1);    //read the analog value and store into the buffer
    //analogBuffer[analogBufferIndex] = 100;    //read the analog value and store into the buffer
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
    
    
    int current_tds_value = (133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5;
    Serial.print("Ave: "); Serial.print(average_reading); Serial.print(" Ave Volt: "); Serial.print(average_voltage); Serial.print(" Temp: "); Serial.print(temperature); Serial.print(" comp Volt: "); Serial.print(compensationVolatge);  Serial.print("  TDS Value: "); Serial.println(tds_value, 0);

    // Display on LCD
    tft.setCursor(0,220); tft.setTextSize(3);
    tft.print(tds_value); 
    tft.setCursor(sensor_description_position,220); tft.print("TDS");
    
    
  }
}

void getTDSCurrentReading(){
  float temperature = 25;
  float voltage = 0;
  unsigned long int reading;
  float compensationCoefficient;
  float compensationVolatge;

  if (water_temp_C == 0) temperature = 25;
  else temperature = water_temp_C;

  reading = ads.readADC_SingleEnded(1);
  voltage = ads.computeVolts(reading);
  compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
  compensationVolatge=voltage/compensationCoefficient;  //temperature compensation

  tds_current_value=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value

  // Debugging
  //Serial.print("Read : "); Serial.print(ads.readADC_SingleEnded(1)); Serial.print("   Voltage : "); Serial.print(ads.computeVolts(ads.readADC_SingleEnded(1))); Serial.print("  TDS:"); Serial.print(tds_current_value); Serial.print("    Comp voltage : "); Serial.println(compensationVolatge); 

  // Display on LCD
  tft.setCursor(0,220); tft.setTextSize(3); tft.print("    ");
  tft.setCursor(0,220); tft.print(tds_current_value); 
  tft.setCursor(sensor_description_position,220); tft.print("TDS");
    

}

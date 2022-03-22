

void getPHReading() {
  float voltage_input = 3.3; // voltage can be 5 or 3.3
  unsigned long int average_reading ;
  unsigned long int buffer_array_ph[10],temp;
  float voltage; // voltage calculated from reading

    for(int i=0;i<10;i++) {// take 10 readings to get average
      buffer_array_ph[i]=ads.readADC_SingleEnded(0); // read the voltage from ADC
      //buffer_array_ph[i]=analogRead(ph_pin); // read from chip ADC pin
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
    voltage = ads.computeVolts(average_reading);
    
    //if (ads.readADC_SingleEnded(0) == 0) ph_value = 0; // Sensor is unplugged
    //else ph_value = voltage_input * calculated_voltage + ph_calibration_adjustment;
    ph_value = voltage_input * voltage + ph_calibration_adjustment;

    // Display on LCD
    tft.setCursor(0,190); tft.setTextSize(3); tft.print("     ");
    tft.setCursor(0,190); tft.print(ph_value,2); 
    tft.setCursor(sensor_description_position,190); tft.print("PH");

    // Debugging
    Serial.print("read = "); Serial.print(ads.readADC_SingleEnded(0));  Serial.print("   volts = "); Serial.print(voltage); Serial.print("  ph_value = "); Serial.println(ph_value);
    //Serial.print("current reading "); Serial.print(ads.readADC_SingleEnded(0)); Serial.print("ads.compute :"), Serial.println(ads.computeVolts(ads.readADC_SingleEnded(0)));
}

void checkADS()
    {
        /*
        if (!ads.isConnected()) {
            Serial.print("There is an issue with the ADC");
            }
            else Serial.print("It seems ok");
            if(ads.isReady())
                {
                Serial.print("its ready!");
                adc0 = ads.readADC(0);
                Serial.print("The ADS reading is .... "); Serial.print(adc0);
                }
        */
        //Serial.print("   ADC Reading : "); Serial.print(adc0); 
        //Serial.print("   ADC voltage : "); Serial.println(adc0);

    }

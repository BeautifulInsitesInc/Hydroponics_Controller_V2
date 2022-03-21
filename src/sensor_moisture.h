int moisture_value;
millisDelay moistureDelay;

void moistureInitilization() {moistureDelay.start(moisture_delay*1000);}

void getMoistureReading(){
    if (moistureDelay.justFinished()){
        /*
        int16_t reading = ads.readADC_SingleEnded(2);
        moisture_value = map(reading, 9500, 0, 100, 0);
        moistureDelay.repeat();
        */
        // Display on LCD
        tft.setCursor(0,250); tft.setTextSize(3);
        tft.print(moisture_value); 
        tft.setCursor(100,250); tft.print("SOIL-1");
        tft.setCursor(100,280); tft.print("SOIL-2");

        // Debugging
        //Serial.print("reading : "); Serial.print(reading);
        //Serial.print("     moisture_value = "); Serial.print(moisture_value); Serial.println("%");
    }
}

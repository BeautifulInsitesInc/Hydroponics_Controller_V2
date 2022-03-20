
// =======================================================
// ======= HEATER CONTROL ================================
// =======================================================
/*
millisDelay heaterTimer;
void heaterInit() {
  pinMode(heat_pin, OUTPUT); digitalWrite(heat_pin, HIGH);
  heaterTimer.start(heat_init_delay *60 * 1000); // start heater initilization delay
}

void checkHeater() {
  if (heaterTimer.remaining()==0) {// if delay is done, start heater if needed
    if (water_temp_C < heat_set) digitalWrite(heat_pin, LOW);
    else digitalWrite(heat_pin, HIGH);
  }
  if (digitalRead(heat_pin == 0)) heater_status = "OFF";
  else heater_status = "ON";
}
*/
// ======

// ==================================================
// ===========  PUMP CONTROL ========================
// ==================================================
/*
int pump_seconds; // current seconds left
int pump_minutes;
millisDelay pumpOnTimer;
millisDelay pumpOffTimer;
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
*/
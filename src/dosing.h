// =================================================
// ========== PH DOSING PUMPS =========================
// =================================================
/*
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
  phDoseDelay.start((ph_dose_delay+pump_init_delay)*60*1000); // start ph delay before dosing can start
}

void phDose(int motor_pin) {// turns on the approiate ph dosing pump
  digitalWrite(motor_pin, LOW); // turn on dosing pump
  phDoseTimer.start(ph_dose_time*1000); // start the pump
  ph_dose_pin = motor_pin;
  phDoseDelay.start(ph_dose_delay * 60 * 1000); // start delay before next dose is allowed
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

*/
// =================================================
// ========== PPM DOSING PUMPS =========================
// =================================================
/*
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
  pumpOnTimer.restart();
}

void ppmDoseB() {
  digitalWrite(ppm_b_pin, LOW); // turn on ppm dosing pump
  ppmDoseTimerB.start(ppm_dose_seconds*1000); // start the pump
  ppmDoseDelay.start(ppm_delay_minutes * 60 * 1000); // start delay before next dose is allowed
  Serial.println("Nutrient dose A has been started, timer is runnning");
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
*/

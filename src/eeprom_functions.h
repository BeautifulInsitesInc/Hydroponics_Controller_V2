#include <EEPROM.h> // to access flash memory

void loadEepromValues(){
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
}
// ==================================================
// ===========  FIREBASE ============================
// ==================================================
#define DEVICE_UID "version_2"// Device ID
#define API_KEY "AIzaSyAfFcN1ZnRzW-elpWK65mwCEGZgWwPPxRc"// Your Firebase Project Web API Key
#define DATABASE_URL "https://conciergev1-default-rtdb.firebaseio.com/"// Your Firebase Realtime database URL
#define USER_EMAIL "controller2@conciergegrowers.ca"
#define USER_PASSWORD "Success2022"
millisDelay firebaseDelayTimer;

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String uid; // to save User ID

String databasePath; // Firebase database path
FirebaseJson json;

unsigned long elapsedMillis = 0; // Stores the elapsed time from device start up
unsigned long elapsedPumpMillis = 0; 
int count = 0; // Dummy counter to test initial firebase updates
//bool isAuthenticated = false;// Store device authentication status

void initFirebase() {
  config.api_key = API_KEY;// configure firebase API Key
  auth.user.email = USER_EMAIL; // Assing teh user sign in credentials
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;// configure firebase realtime database url
  Firebase.reconnectWiFi(true);// Enable WiFi reconnection 
  fbdo.setResponseSize(4096);

  Serial.println("------------------------------------");
  Serial.println("Sign up new user...");
/*
  if (Firebase.signUp(&config, &auth, USER_EMAIL, USER_PASSWORD))// Add user if it doest exist
    {
      Serial.println("Success");
      //isAuthenticated = true;
      uid = auth.token.uid.c_str();
      Serial.println("Signed into Firebase!");
    }
  else
    {
      Serial.printf("Failed, %s\n", config.signer.signupError.message.c_str());
      //isAuthenticated = false;
    }
    */
  config.token_status_callback = tokenStatusCallback;// Assign the callback function for the long running token generation task, see addons/TokenHelper.h
  config.max_token_generation_retry = 5;// Assign the maximum retry of token generation
  Firebase.begin(&config, &auth);// Initialise the firebase library

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
  databasePath = "/UsersData/" + uid;
  Serial.print("databasePath : "); Serial.println(databasePath);

  firebaseDelayTimer.start(firebase_delay*1000);
}

void sendToFirebase() {
    //Serial.print("starting sendtoFirebase");
    //if (millis() - elapsedMillis > (firebase_interval*1000) && Firebase.ready())// Check that 10 seconds has elapsed before, device is authenticated and the firebase service is ready.
    if (Firebase.ready() && firebaseDelayTimer.justFinished())
     {
        firebaseDelayTimer.repeat();
        String datatype = "/Sensor Readings";
        Serial.println("sending data");
        
        Firebase.RTDB.setFloat(&fbdo, databasePath + datatype + "/Room Temp", dht_tempC);
        Firebase.RTDB.setFloat(&fbdo, databasePath + datatype + "/Humidity", dht_humidity);
        Firebase.RTDB.setFloat(&fbdo, databasePath + datatype + "/Water Temp", water_temp_C);
        Firebase.RTDB.setString(&fbdo, databasePath + datatype + "/Pump Status", pump_status);
        Firebase.RTDB.setString(&fbdo, databasePath + datatype + "/Heater Status", heater_status);
        Firebase.RTDB.setFloat(&fbdo, databasePath + datatype + "/pH", ph_value);


        //Firebase.setString(fbdo, databasePath + datatype + "/Last Time", current_time);
       
        //Firebase.setFloat(fbdo, databasePath + datatype + "/TDS", tds_value);
        //Firebase.setString(fbdo, databasePath + datatype + "/Pump Time", pump_time_string);
        //Firebase.setInt(fbdo, databasePath + datatype + "/Root Dampness", moisture_value);

        // Check settings
        
        datatype = "/Settings";
        Firebase.RTDB.setFloat(&fbdo, databasePath + datatype + "/set Water Temp", heat_set);
        Firebase.RTDB.setFloat(&fbdo, databasePath + datatype + "/local ip", local_ip);
        Firebase.RTDB.setFloat(&fbdo, databasePath + datatype + "/set PH", ph_set_level);
        Firebase.RTDB.setInt(&fbdo, databasePath + datatype + "/set PPM", ppm_set_level);
        Firebase.RTDB.setString(&fbdo, databasePath + datatype + "/Device UID", DEVICE_UID);
        Firebase.RTDB.setInt(&fbdo, databasePath + datatype + "/set Pump ON", pump_on_time);
        Firebase.RTDB.setInt(&fbdo, databasePath + datatype + "/set Pump OFF", pump_off_time);
        Firebase.RTDB.setBool(&fbdo, databasePath + datatype + "/Temp Units", temp_in_C);
        

        //Firebase.setInt(fbdo, databasePath + datatype + "/pH Dose Time", ph_dose_time);
        //Firebase.setFloat(fbdo, databasePath + datatype + "/pH Tolorance", ph_tolerance);
        //Firebase.setInt(fbdo, databasePath + datatype + "/Nutrient Dose Time", ppm_dose_seconds);
        //Firebase.setFloat(fbdo, databasePath + datatype + "/Nutrient Tolorance", ppm_tolerance);
        //Firebase.setString(fbdo, databasePath + datatype + "/Pump Time", pump_time_string);
        //Firebase.setInt(fbdo, databasePath + datatype + "/pH Dose Delay", ph_dose_delay);
        //Firebase.setInt(fbdo, databasePath + datatype + "/Nutrient Delay", ppm_delay_minutes);
        //Firebase.setFloat(fbdo, databasePath + datatype + "/set pH", ph_set_level);
      }
}

/*
void sendPumpTimetoFirebase()
  {
    if (millis() - elapsedPumpMillis > 1000 && Firebase.ready())
          {
            elapsedPumpMillis = millis();
            String datatype = "/Sensor Readings";
            Firebase.setString(fbdo, databasePath + datatype + "/Pump Time", pump_time_string);
          }

  }
*/
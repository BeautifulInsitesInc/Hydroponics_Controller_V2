#include <SPIFFS.h>

void spiffInit(){
    if (!SPIFFS.begin()) { //  Initialize SPIFFS
        Serial.println("SPIFFS init failed");
        return;
    }
    Serial.println("SPIFFS init finished");
}

void spiffListFiles(){
    //list contents of SPIFF
    File root = SPIFFS.open("/");
    File listing = root.openNextFile();

    while(listing){
        Serial.print("FILE: ");
        Serial.println(listing.name());
        listing = root.openNextFile();
     }
}
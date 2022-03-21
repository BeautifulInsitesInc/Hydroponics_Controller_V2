#include <TJpg_Decoder.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <FS.h> // for ili9488  // The SPIFFS (FLASH filing system) is used to hold touch screencalibration data
#define FS_NO_GLOBALS

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
#define TFT_GREY 0x5AEB // New colour
bool showsplash; // used in loop to show only on initilization


// ============ DISPLAYING JPEGS ======================
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap){// render each block to the TFT.
  if ( y >= tft.height() ) return 0;// Stop further decoding as image is running off bottom of screen
  tft.pushImage(x, y, w, h, bitmap);  // This function will clip the image block rendering automatically at the TFT boundaries
  return 1;// Return 1 to decode next block
}



void lcdInit(){
  //tft.init();
  tft.setRotation(1);
  tft.begin();
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  // splash screen init
  showsplash = true;
  TJpgDec.setJpgScale(1);// The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setCallback(tft_output);  // The decoder must be given the exact name of the rendering function above
}

void loadFile(const char *name)
{
  tft.fillScreen(TFT_RED);
  uint32_t t = millis();// Time recorded for test purposes
  uint16_t w = 0, h = 0, scale;// Get the width and height in pixels of the jpeg if you wish
  TJpgDec.getFsJpgSize(&w, &h, name); 
  tft.setRotation(w > h ? 1 : 0);
  for (scale = 1; scale <= 8; scale <<= 1) {
    if (w <= tft.width() * scale && h <= tft.height() * scale) break;
  }
  TJpgDec.setJpgScale(scale);
  TJpgDec.drawFsJpg(0, 0, name);// Draw the image, top left at 0,0
  t = millis() - t;  // How much time did rendering take

  char buf[80];
  sprintf(buf, "%s %dx%d 1:%d %u ms", name, w, h, scale, t);
  //tft.setCursor(0, tft.height() - 8);
  //tft.print(buf);
  Serial.println(buf);
  //delay(2000);
}

void lcdSplashScreen(){
  loadFile("/lexi.jpg");
  delay(2000);
  //loadFile("/cg.jpg");
  //delay(1000);
  loadFile("/cgl.jpg");
  delay(5000);

  //tft.fillScreen(TFT_RED);
  //uint32_t t = millis();  // Time recorded for test purposes
  //uint16_t w = 0, h = 0;// Get the width and height in pixels of the jpeg if you wish
  //TJpgDec.getFsJpgSize(&w, &h, "/cg.jpg"); // Note name preceded with "/"
  //Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);
  //TJpgDec.drawFsJpg(0, 0, "/cg.jpg");  // Draw the image, top left at 0,0
  //t = millis() - t;  // How much time did rendering take (ESP8266 80MHz 271ms, 160MHz 157ms, ESP32 SPI 120ms, 8bit parallel 105ms
  //Serial.print(t); Serial.println(" ms");
  //delay(5000);// Wait before drawing again
}





void lcdTest(){
    tft.fillScreen(TFT_WHITE);
    tft.setCursor(0, 0, 2);
    // Set the font colour to be white with a black background, set text size multiplier to 1
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  tft.setTextSize(5);
    // We can now plot text on screen using the "print" class
    tft.println("Hi Lexi!");
    tft.println("Your feet stink!");
}

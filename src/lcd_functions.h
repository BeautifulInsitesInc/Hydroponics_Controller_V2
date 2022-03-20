TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
#define TFT_GREY 0x5AEB // New colour

void lcdInit(){
  tft.init();
  tft.setRotation(1);
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

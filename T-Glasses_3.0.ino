#include <Adafruit_GFX.h>    // Core graphics library
#include <splash.h>
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#include "bitmaps.h"

#define TFT_CS  10
#define TFT_RST 9
#define TFT_DC 8


const uint16_t BLACK = 0x0000;
const uint16_t WHITE = 0xffff;
const uint16_t BLUE = 0x001f;
const uint16_t RED = 0xf800;
const uint16_t YELLOW = 0xffe0;
const uint16_t GREEN = 0x07e0;


Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

bool timerActive = false;
int timerHour = 59;
int timerMinute = 59;
int timerSecond = 59;

unsigned long timerCurrentMillis = 0;
unsigned long stopWatchCurrentMillis = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  display.initR(INITR_MINI160x80);
  display.setRotation(3);
  display.invertDisplay(true);

  display.setCursor(0, 0);
  display.setTextSize(3);
  display.setTextColor(WHITE, BLACK);
  display.fillScreen(BLACK);
  
  display.drawBitmap(0, 0, boot_logo, 160, 80, BLUE);
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  fnTimer();
  
}

void uiHome() {
  
}

void fnTimer() {
  if(millis() - timerCurrentMillis >= 1000) {
    if((timerHour == 0) && (timerMinute == 0) && (timerSecond == 0)){
      
    }
    else if(timerSecond > 0) {
      timerSecond --;
      
    } else {
      timerSecond = 59;
      if(timerMinute > 0) {
        timerMinute --;
        
      } else {
        timerMinute = 59;
        if(timerHour > 0) {
          timerHour --;
        }
      }
    }
    Serial.println(String(timerHour) + ":" + String(timerMinute) + ":" + String(timerSecond));
    timerCurrentMillis = millis();
  }
}

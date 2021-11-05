#include <Adafruit_GFX.h>    // Core graphics library
#include <splash.h>
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <RotaryEncoder.h>
#include <SPI.h>

#include "bitmaps.h"

#define TFT_CS  10
#define TFT_RST 9
#define TFT_DC 8

#define PIN_IN1 5
#define PIN_IN2 4
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
void checkPosition(){
  encoder.tick();
}


const uint16_t BLACK = 0x0000;
const uint16_t WHITE = 0xffff;
const uint16_t BLUE = 0x001f;
const uint16_t RED = 0xf800;
const uint16_t YELLOW = 0xffe0;
const uint16_t GREEN = 0x07e0;
const uint16_t color = WHITE;
Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

boolean timerActive = true;
int timerHour = 0;
int timerMinute = 0;
int timerSecond = 0;

boolean stopWatchActive = false;
int stopWatchHour = 0;
int stopWatchMinute = 0;
int stopWatchSecond = 0; 

boolean encoderButton = false;

unsigned long timerPreviousMillis = 0;
unsigned long stopWatchPreviousMillis = 0;

String currentPage = "menuHome";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(6, INPUT_PULLUP);
  display.initR(INITR_MINI160x80);
  display.setRotation(3);
  display.invertDisplay(true);

  attachInterrupt(digitalPinToInterrupt(PIN_IN1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_IN2), checkPosition, CHANGE);

  display.setCursor(0, 0);
  display.setTextSize(3);
  display.setTextColor(WHITE, BLACK);
  display.fillScreen(BLACK);
  
  display.drawBitmap(0, 0, boot_logo, 160, 80, color);
  delay(2000);
  display.fillScreen(BLACK); 
  menuHome();
}

void loop() {
  // put your main code here, to run repeatedly:
  static int pos = 0;
  encoder.tick();
  int newPos = encoder.getPosition() / 2;
  while (pos != newPos) { //Encoder Rotated
    int dir = (int)(encoder.getDirection());
    if(dir == 1) {
      Serial.println("Right");
      if(currentPage == "menuHome") {
        menuTimer();
      }
      else if(currentPage == "menuTimer") {
        menuStopWatch();
      }
      else if(currentPage == "menuStopWatch") {
        menuNote();
      }
      else if(currentPage == "menuNote") {
        menuSettings();
      }
    }
    else if(dir == -1) {
      if(currentPage == "menuTimer") {
        menuHome();
      }
      else if(currentPage == "menuStopWatch") {
        menuTimer();
      }
      else if(currentPage == "menuNote") {
        menuStopWatch();
      }
      else if(currentPage == "menuSettings") {
        menuNote();
      }
    }
    pos = newPos;
  }

  int encoderButtonValue = digitalRead(6);
  if(encoderButtonValue == LOW && encoderButton == false) {
    encoderButton = true;
    Serial.println("Button pressed");
    
  }
  else if(encoderButtonValue == HIGH && encoderButton == true) {
    encoderButton = false;
  }
}

void menuHome() {
  currentPage = "menuHome";
  display.fillScreen(BLACK);
}

void menuTimer() {
  currentPage = "menuTimer";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, ui_timer, 160, 80, color);
  
  
}

void menuStopWatch() {
  currentPage = "menuStopWatch";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, ui_stopWatch, 160, 80, color);;
}

void menuNote() {
  currentPage = "menuNote";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, ui_note, 160, 80, color);
}

void menuSettings() {
  currentPage = "menuSettings";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, ui_settings, 160, 80, color);
}

void fnTimer() {
  unsigned long currentMillis = millis();
  if(currentMillis - timerPreviousMillis >= 1000) {
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
    Serial.println("Timer " + String(timerHour) + ":" + String(timerMinute) + ":" + String(timerSecond));
    if(currentPage == "home") {
      display.setCursor(5, 5);
      display.setTextSize(1);
      display.setTextColor(color, BLACK);
      display.print(String(timerHour) + ":" + String(timerMinute) + ":" + String(timerSecond));
    }
    timerPreviousMillis += 1000;
  }
}

void fnStopWatch() {
  unsigned long currentMillis = millis();
  if(currentMillis - stopWatchPreviousMillis >= 1000) {
    if(stopWatchSecond < 59) {
      stopWatchSecond ++;
    } else{
      stopWatchSecond = 0;
      if(stopWatchMinute < 59) {
        stopWatchMinute ++;
      } else{
        stopWatchMinute = 0;
        if(stopWatchHour < 99) {
          stopWatchHour ++;
        }
      }
    }
    Serial.println("StopWatch " + String(stopWatchHour) + ":" + String(stopWatchMinute) + ":" + String(stopWatchSecond));
    if(currentPage == "home") {
      display.setCursor(5, 5);
      display.setTextSize(1);
      display.setTextColor(color, BLACK);
      display.print(String(stopWatchHour) + ":" + String(stopWatchMinute) + ":" + String(stopWatchSecond));
    }
    stopWatchPreviousMillis += 1000;
  }
}

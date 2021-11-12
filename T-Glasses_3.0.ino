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
void checkPosition() {
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

String BTinput = "";

boolean timerActive = true;
int timerHour = 0;
int timerMinute = 0;
int timerSecond = 10;

boolean stopWatchActive = false;
int stopWatchHour = 0;
int stopWatchMinute = 0;
int stopWatchSecond = 0;

int timeHour = 0;
int timeMinute = 0;
int timeYear = 0;
int timeMonth = 0;
int timeDay = 0;

boolean encoderButton = false;

unsigned long timePreviousMillis = 0;
unsigned long timerPreviousMillis = 0;
unsigned long stopWatchPreviousMillis = 0;

String currentPage = "menuHome";

void setup() {
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
  static int pos = 0;
  encoder.tick();
  int newPos = encoder.getPosition() / 2;
  while (pos != newPos) { //When rotary encoder position is changed
    int dir = (int)(encoder.getDirection());
    if (dir == 1) { //If the rotary encoder is turned CW
      Serial.println("Right");
      if (currentPage == "menuHome") {
        menuTimer();
      }
      else if (currentPage == "menuTimer") {
        menuStopWatch();
      }
      else if (currentPage == "menuStopWatch") {
        menuNote();
      }
      else if (currentPage == "menuNote") {
        menuSettings();
      }
    }
    else if (dir == -1) { //If the rotary encoder is turned CCW``
      if (currentPage == "menuTimer") {
        menuHome();
        runTime(true);
      }
      else if (currentPage == "menuStopWatch") {
        menuTimer();
      }
      else if (currentPage == "menuNote") {
        menuStopWatch();
      }
      else if (currentPage == "menuSettings") {
        menuNote();
      }
    }
    pos = newPos;
  }

  int encoderButtonValue = digitalRead(6);
  if (encoderButtonValue == LOW && encoderButton == false) {
    encoderButton = true;
    Serial.println("Button pressed");

  }
  else if (encoderButtonValue == HIGH && encoderButton == true) {
    encoderButton = false;
  }

  runTime(false);
  if (timerActive == true) {
    runTimer();
  }
  if (stopWatchActive == true) {
    runStopWatch();
  }



}

void menuHome() {
  currentPage = "menuHome";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, ui_home, 160, 80, color);
  display.drawCircle(30, 39, 26, color);
  if (BTinput == "") {

  }
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

void runTimer() {
  unsigned long currentMillis = millis();
  if (currentMillis - timerPreviousMillis >= 1000) {
    if ((timerHour == 0) && (timerMinute == 0) && (timerSecond == 0)) {
      if(currentPage == "menuHome") {
        display.setCursor(0, 0);
        display.setTextSize(1);
        display.setTextColor(color, BLACK);
        display.print("Timer: OFF     ");
      }
    }
    else if (timerSecond > 0) {
      timerSecond --;
      if(currentPage == "menuHome") {
        display.setCursor(0, 0);
        display.setTextSize(1);
        display.setTextColor(color, BLACK);
        display.print("Timer: " + formatTime(timerHour, timerMinute, timerSecond, true));
      }
    } else {
      timerSecond = 59;
      if (timerMinute > 0) {
        timerMinute --;

      } else {
        timerMinute = 59;
        if (timerHour > 0) {
          timerHour --;
        }
      }
    }
    timerPreviousMillis += 1000;
  }
}

void runStopWatch() {
  unsigned long currentMillis = millis();
  if (currentMillis - stopWatchPreviousMillis >= 1000) {
    if (stopWatchSecond < 59) {
      stopWatchSecond ++;
    } else {
      stopWatchSecond = 0;
      if (stopWatchMinute < 59) {
        stopWatchMinute ++;
      } else {
        stopWatchMinute = 0;
        if (stopWatchHour < 99) {
          stopWatchHour ++;
        }
      }
    }
    Serial.println("StopWatch " + String(stopWatchHour) + ":" + String(stopWatchMinute) + ":" + String(stopWatchSecond));
    if (currentPage == "menuHome") {
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.setTextColor(color, BLACK);
      display.print("Stopwatch: " + formatTime(stopWatchHour, stopWatchMinute, stopWatchSecond, true));
    }
    stopWatchPreviousMillis += 1000;
  }
}

void runTime(boolean refresh) {
  if (refresh == true) {
    display.fillCircle(30, 39, 25, BLACK);
    switch (timeMinute / 5 * 5) {
      case 0:
        display.drawLine(30, 39, 30, 18, color);
        break;
      case 5:
        display.drawLine(30, 39, 40, 21, color);
        break;
      case 10:
        display.drawLine(30, 39, 48, 29, color);
        break;
      case 15:
        display.drawLine(30, 39, 51, 39, color);
        break;
      case 20:
        display.drawLine(30, 39, 48, 49, color);
        break;
      case 25:
        display.drawLine(30, 39, 40, 57, color);
        break;
      case 30:
        display.drawLine(30, 39, 30, 60, color);
        break;
      case 35:
        display.drawLine(30, 39, 20, 57, color);
        break;
      case 40:
        display.drawLine(30, 39, 12, 49, color);
        break;
      case 45:
        display.drawLine(30, 39, 9, 39, color);
        break;
      case 50:
        display.drawLine(30, 39, 12, 29, color);
        break;
      case 55:
        display.drawLine(30, 39, 20, 21, color);
        break;
      case 60:
        display.drawLine(30, 39, 30, 18, color);
        break;
    }
    switch (timeHour) {
      case 1:
        display.drawLine(30, 39, 37, 27, color);
        break;
      case 2:
        display.drawLine(30, 39, 42, 31, color);
        break;
      case 3:
        display.drawLine(30, 39, 43, 39, color);
        break;
      case 4:
        display.drawLine(30, 39, 42, 47, color);
        break;
      case 5:
        display.drawLine(30, 39, 37, 51, color);
        break;
      case 6:
        display.drawLine(30, 39, 30, 52, color);
        break;
      case 7:
        display.drawLine(30, 39, 23, 51, color);
        break;
      case 8:
        display.drawLine(30, 39, 18, 47, color);
        break;
      case 9:
        display.drawLine(30, 39, 17, 39, color);
        break;
      case 10:
        display.drawLine(30, 39, 18, 31, color);
        break;
      case 11:
        display.drawLine(30, 39, 23, 27, color);
        break;
      case 12:
        display.drawLine(30, 39, 30, 26, color);
        break;

    }
    display.setCursor(63, 25);
    display.setTextSize(2);
    display.println(formatTime(timeHour, timeMinute, 0, false));
  }
  else {
    unsigned long currentMillis = millis();
    if (currentMillis - timePreviousMillis >= 60000) {
      if (timeMinute < 59) {
        timeMinute ++;
      } else {
        timeMinute = 0;
        if (timeHour < 12) {
          timeHour ++;
        } else {
          timeHour = 1;
        }
      }
      if (currentPage == "menuHome") {
        display.fillCircle(30, 39, 25, BLACK);
        switch (timeMinute / 5 * 5) {
          case 0:
            display.drawLine(30, 39, 30, 18, color);
            break;
          case 5:
            display.drawLine(30, 39, 40, 21, color);
            break;
          case 10:
            display.drawLine(30, 39, 48, 29, color);
            break;
          case 15:
            display.drawLine(30, 39, 51, 39, color);
            break;
          case 20:
            display.drawLine(30, 39, 48, 49, color);
            break;
          case 25:
            display.drawLine(30, 39, 40, 57, color);
            break;
          case 30:
            display.drawLine(30, 39, 30, 60, color);
            break;
          case 35:
            display.drawLine(30, 39, 20, 57, color);
            break;
          case 40:
            display.drawLine(30, 39, 12, 49, color);
            break;
          case 45:
            display.drawLine(30, 39, 9, 39, color);
            break;
          case 50:
            display.drawLine(30, 39, 12, 29, color);
            break;
          case 55:
            display.drawLine(30, 39, 20, 21, color);
            break;
          case 60:
            display.drawLine(30, 39, 30, 18, color);
            break;
        }
        switch (timeHour) {
          case 1:
            display.drawLine(30, 39, 37, 27, color);
            break;
          case 2:
            display.drawLine(30, 39, 42, 31, color);
            break;
          case 3:
            display.drawLine(30, 39, 43, 39, color);
            break;
          case 4:
            display.drawLine(30, 39, 42, 47, color);
            break;
          case 5:
            display.drawLine(30, 39, 37, 51, color);
            break;
          case 6:
            display.drawLine(30, 39, 30, 52, color);
            break;
          case 7:
            display.drawLine(30, 39, 23, 51, color);
            break;
          case 8:
            display.drawLine(30, 39, 18, 47, color);
            break;
          case 9:
            display.drawLine(30, 39, 17, 39, color);
            break;
          case 10:
            display.drawLine(30, 39, 18, 31, color);
            break;
          case 11:
            display.drawLine(30, 39, 23, 27, color);
            break;
          case 12:
            display.drawLine(30, 39, 30, 26, color);
            break;

        }
        display.setCursor(63, 25);
        display.setTextSize(2);
        display.println(formatTime(timeHour, timeMinute, 0, false));
      }
      timePreviousMillis += 60000;
    }
  }
}

void homeDate() {

}

void homeNote() {

}

void splitData(String string) {
  timeHour = string.substring(0, 2).toInt();
  timeMinute = string.substring(3, 5).toInt();;
  timeYear = string.substring(6, 10).toInt();;
  timeMonth = string.substring(11, 13).toInt();;
  timeDay = string.substring(14, 16).toInt();;
}

String formatTime(int hour, int minute, int second, boolean useSecond) {
  String sHour = String(hour);
  String sMinute = String(minute);
  if (sHour.length() == 1) {
    sHour = "0" + sHour;
  }
  if (sMinute.length() == 1) {
    sMinute = "0" + sMinute;
  }
  if (useSecond == false) {
    return sHour + ":" + sMinute;
  }
  else {
    String sSecond = String(second);
    if (sSecond.length() == 1) {
      sSecond = "0" + sSecond;
    }
    return sHour + ":" + sMinute + ":" + sSecond;
  }
  return "error";
}

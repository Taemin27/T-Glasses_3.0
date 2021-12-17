
//Please download all of these libraried before uploading
//SoftwareSerial and SPI is usually pre-installed
//Make sure that the bitmaps.h file is in the same folder as the main file
#include <Adafruit_GFX.h>
#include <splash.h>
#include <Adafruit_ST7735.h>
#include <RotaryEncoder.h>
#include <SPI.h>
#include <SoftwareSerial.h>
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

SoftwareSerial BT(2, 3);

const uint16_t BLACK = 0x0000;
const uint16_t WHITE = 0xffff;
const uint16_t BLUE = 0x001f;
const uint16_t RED = 0xf800;
const uint16_t YELLOW = 0xffe0;
const uint16_t GREEN = 0x07e0;
const uint16_t color = WHITE;
Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

String BTinput = "";

//Timer Variables
boolean timerActive = false;
int timerHour = 0;
int timerMinute = 0;
int timerSecond = 0;
int timerSelectorValue = 0;
boolean timerSelected = false;

//StopWatch Variables
boolean stopWatchActive = false;
int stopWatchHour = 0;
int stopWatchMinute = 0;
int stopWatchSecond = 0;
int stopWatchSelectorValue = 0;

//Current Time Variables
boolean timeActive = false;
int timeHour = 0;
int timeMinute = 0;
int timeYear = 0;
int timeMonth = 0;
int timeDay = 0;

int encoderButton = 0;

//For time checking without delay()
unsigned long timePreviousMillis = 0;
unsigned long timerPreviousMillis = 0;
unsigned long stopWatchPreviousMillis = 0;
unsigned long buttonPreviousMillis = 0;

String currentPage = "menuHome"; //Labels the current page name
String note = ""; //Stored the note

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  pinMode(6, INPUT_PULLUP);
  display.initR(INITR_MINI160x80);
  display.setRotation(3); //Change the number if needed
  display.invertDisplay(true); //Change this to false if needed

  attachInterrupt(digitalPinToInterrupt(PIN_IN1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_IN2), checkPosition, CHANGE);

  display.setCursor(0, 0);
  display.setTextSize(3);
  display.setTextColor(WHITE, BLACK);
  display.fillScreen(BLACK);

  display.drawBitmap(0, 0, boot_logo, 160, 80, color);
  delay(2000); //Delete or comment this line if you don't want the boot logo
  display.fillScreen(BLACK);
  menuHome();
}

void loop() {
  if (BT.available() > 0) { //Reads any bluetooth data and saves into a string
    BTinput = BT.readString();
    BTinput.trim();
    splitData(BTinput);
    timeActive = true;
  }


  static int pos = 0;
  encoder.tick();
  
  int newPos = encoder.getPosition() / 1; //Change the divisor depending on your encoder model(Typically 1 or 2 will work)

  
  while (pos != newPos) { //When rotary encoder position is changed
    int dir = (int)(encoder.getDirection()); //The direction of rotation
    if (dir == 1) { //If the rotary encoder is turned CW
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

      else if (currentPage == "fnTimer") {
        if (timerSelected == false) {
          timerSelectorValue ++;
          fnTimer();
          timerSelector(false);
        }
        else {
          switch (timerSelectorValue) { //Setting up timer number(increase)
            case 1:
              timerHour ++;
              if (timerHour > 99) {
                timerHour = 99;
              }
              timerSelector(true);
              break;
            case 2:
              timerMinute ++;
              if (timerMinute > 59) {
                timerMinute = 59;
              }
              else if (timerMinute < 0) {
                timerMinute = 0;
              }
              timerSelector(true);
              break;
            case 3:
              timerSecond ++;
              if (timerSecond > 59) {
                timerSecond = 59;
              }
              else if (timerSecond < 0) {
                timerSecond = 0;
              }
              timerSelector(true);
              break;
          }
        }
      }
      else if(currentPage == "fnStopWatch") {
        stopWatchSelectorValue ++;
        fnStopWatch();
        stopWatchSelector();
      }
    }
    else if (dir == -1) { //If the rotary encoder is turned CCW
      if (currentPage == "menuTimer") {
        menuHome();
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

      else if (currentPage == "fnTimer") {
        if (timerSelected == false) {
          timerSelectorValue --;
          fnTimer();
          timerSelector(false);
        }
        else {
          switch (timerSelectorValue) { //Setting up timer number(decrease)
            case 1:
              timerHour --;
              if (timerHour < 0) {
                timerHour = 0;
              }
              timerSelector(true);
              break;
            case 2:
              timerMinute --;
              if (timerMinute < 0) {
                timerMinute = 0;
              }
              timerSelector(true);
              break;
            case 3:
              timerSecond --;
              if (timerSecond < 0) {
                timerSecond = 0;
              }
              timerSelector(true);
              break;
          }
        }
      }
      else if(currentPage == "fnStopWatch") {
        stopWatchSelectorValue --;
        fnStopWatch();
        stopWatchSelector();
      }
    }
    pos = newPos; //Reset the encoder position
  }

  encoderButton = digitalRead(6); //Reads the button value
  if (encoderButton == LOW) {
    unsigned long buttonCurrentMillis = millis();
    if (buttonCurrentMillis - buttonPreviousMillis >= 300) {
      
      if (currentPage == "menuTimer") {
        fnTimer();
        timerSelector(0);
      }
      else if (currentPage == "fnTimer") {
        if (timerSelectorValue == 0) {
          menuTimer();
        }
        else if (timerSelectorValue == 4) {
          timerPreviousMillis = millis();
          timerActive = !timerActive;
        }
        else {
          fnTimer();
          timerSelected = !timerSelected;
          timerSelector(timerSelected);
        }
      }
      else if(currentPage == "menuStopWatch") {
        fnStopWatch();
        stopWatchSelector();
      }
      else if(currentPage == "fnStopWatch") {
        switch(stopWatchSelectorValue) {
          case 0:
            menuStopWatch();
            break;
          case 1:
            stopWatchPreviousMillis = millis();
            stopWatchActive = !stopWatchActive;
            stopWatchSelector();
            break;
          case 2:
            stopWatchHour = 0;
            stopWatchMinute = 0;
            stopWatchSecond = 0;
            fnStopWatch();
            stopWatchSelector();
            break;
        }
      }
      buttonPreviousMillis = buttonCurrentMillis;
    }
  }

  if (timeActive == true) { //Shows the current time only when it's active
    runTime(false);
  }
  if (timerActive == true) { //Shows the running timer only when it's active
    runTimer();
  }
  if (stopWatchActive == true) { //Shows the running stop watch only when it's active
    runStopWatch();
  }



}

void menuHome() { //Home page UI
  currentPage = "menuHome";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, ui_home, 160, 80, color);
  if (note != "") {
    display.setCursor(0, 70);
    display.setTextSize(1);
    display.setTextColor(color);
    display.print("Note: " + note.substring(0, 15) + "...");
  }

  if (timeActive == false) { //When current time isn't synced
    display.setCursor(0, 20);
    display.setTextSize(1);
    display.setTextColor(RED);
    display.println("No clock data");
    display.setTextColor(color);
    display.print("Please connect your       smartphone to sync        with current time");
  }
  else { //When current time is synced
    display.drawCircle(30, 39, 26, color); //Outline of the analog clock
    runTime(true); //Hands and digital clock
  }
}

void menuTimer() { //Timer option UI
  currentPage = "menuTimer";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, ui_timer, 160, 80, color);
}

void menuStopWatch() { //StopWatch option UI
  currentPage = "menuStopWatch";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, ui_stopWatch, 160, 80, color);;
}

void menuNote() { //Note option UI
  currentPage = "menuNote";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, ui_note, 160, 80, color);
}

void menuSettings() { //Settings option UI
  currentPage = "menuSettings";
  display.fillScreen(BLACK);
  display.drawBitmap(0, 0, ui_settings, 160, 80, color);
}

void runTimer() { //Runs timer in background
  unsigned long currentMillis = millis();
  if (currentMillis - timerPreviousMillis >= 1000) {
    if ((timerHour == 0) && (timerMinute == 0) && (timerSecond == 0)) {
      timerActive = false;
    } else {
      if (timerSecond > 0) {
        timerSecond --;
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
    }
    if (currentPage == "menuHome") {
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.setTextColor(color, BLACK);
      display.print("Timer: " + formatTime(timerHour, timerMinute, timerSecond, true));
    }
    else if (currentPage == "fnTimer") {
      display.setTextColor(color, BLACK);
      display.setCursor(32, 20);
      display.setTextSize(2);
      display.print(formatTime(timerHour, timerMinute, timerSecond, true));
      timerSelector(timerSelected);
    }


    timerPreviousMillis += 1000; //Millis sync(even if it falls behind it catched up on the next second)
  }
}

void runStopWatch() { //Runs stop watch in background
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
    if(currentPage == "fnStopWatch") {
      display.setTextColor(color, BLACK);
      display.setCursor(32, 20);
      display.setTextSize(2);
      display.print(formatTime(stopWatchHour, stopWatchMinute, stopWatchSecond, true));
      stopWatchSelector();
    }
    stopWatchPreviousMillis += 1000; //Millis sync(even if it falls behind it catched up on the next second)
  }
}

void runTime(boolean refresh) { //Runs current time in background
  if (refresh == true) { //Only for refreshing
    display.fillCircle(30, 39, 25, BLACK);
    switch (timeMinute / 5 * 5) { //Round down minute to the nearest 5 and show the corresponding hand
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
    switch (timeHour) { //Hour hand
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
    display.setTextColor(color, BLACK);
    display.println(formatTime(timeHour, timeMinute, 0, false)); //Digital clock next to the analog clock
  }
  else {
    unsigned long currentMillis = millis();
    if (currentMillis - timePreviousMillis >= 60000) { //Repeats every 60 seconds
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
      if (currentPage == "menuHome") { //NOT for refreshing. Actually shows the hands and digital clock
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
        display.setTextColor(color, BLACK);
        display.println(formatTime(timeHour, timeMinute, 0, false)); //Digital clock
      }
      timePreviousMillis += 60000; //Millis sync(even if it falls behind it catched up on the next second)
    }
  }
}

void fnTimer() { //Timer settings page
  currentPage = "fnTimer";
  display.fillScreen(BLACK);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(color, BLACK);
  display.print("<<< Set Timer");
  display.setCursor(32, 20);
  display.setTextSize(2);
  display.print(formatTime(timerHour, timerMinute, timerSecond, true));
  display.setCursor(50, 50);
  display.setTextSize(1);
  display.print("Start/Stop");

}
void timerSelector(boolean a) { //Shows selector bar
  if (a == true) { //When the button is pressed(Inverts the number)
    switch (timerSelectorValue) {
      case 0:
        display.setCursor(0, 0);
        display.setTextSize(1);
        display.setTextColor(BLACK, color);
        display.print("<<<");
        break;
      case 1:
        display.setCursor(32, 20);
        display.setTextSize(2);
        display.setTextColor(BLACK, color);
        display.print(formatInt(timerHour));
        break;
      case 2:
        display.setCursor(68, 20);
        display.setTextSize(2);
        display.setTextColor(BLACK, color);
        display.print(formatInt(timerMinute));
        break;
      case 3:
        display.setCursor(104, 20);
        display.setTextSize(2);
        display.setTextColor(BLACK, color);
        display.print(formatInt(timerSecond));
        break;
    }
  }
  else if (a == false) { //When the button is not pressed(Underlines the selected option
    if (timerSelectorValue < 0) {
      timerSelectorValue = 0;
    }
    else if (timerSelectorValue > 4) {
      timerSelectorValue = 4;
    }
    switch (timerSelectorValue) {
      case 0:
        display.drawFastHLine(0, 8, 17, color);
        break;
      case 1:
        display.drawFastHLine(32, 36, 22, color);
        display.drawFastHLine(32, 37, 22, color);
        break;
      case 2:
        display.drawFastHLine(68, 36, 22, color);
        display.drawFastHLine(68, 37, 22, color);
        break;
      case 3:
        display.drawFastHLine(104, 36, 22, color);
        display.drawFastHLine(104, 37, 22, color);
        break;
      case 4:
        display.drawFastHLine(50, 58, 59, color);
    }
  }
}

void fnStopWatch() {
 currentPage = "fnStopWatch";
 display.fillScreen(BLACK);
 display.setCursor(0, 0);
 display.setTextSize(1);
 display.setTextColor(color, BLACK);
 display.print("<<< StopWatch");
 display.setCursor(32, 20);
 display.setTextSize(2);
 display.print(formatTime(stopWatchHour, stopWatchMinute, stopWatchSecond, true));
 display.setCursor(39, 50);
 display.setTextSize(1);
 display.print("Start/Stop  Reset");
}
void stopWatchSelector() {
  if(stopWatchSelectorValue < 0) {
    stopWatchSelectorValue = 0;
  }
  else if(stopWatchSelectorValue > 2) {
    stopWatchSelectorValue = 2;
  }
  switch(stopWatchSelectorValue) {
    case 0:
      display.drawFastHLine(0, 8, 17, color);
      break;
    case 1:
      display.drawFastHLine(39, 58, 59, color);
      break;
    case 2:
      display.drawFastHLine(111, 58, 29, color);
  }
}

void splitData(String string) { //Splits the bluetooth data into five independent variables
  timeHour = string.substring(0, 2).toInt();
  timeMinute = string.substring(3, 5).toInt();;
  timeYear = string.substring(6, 10).toInt();;
  timeMonth = string.substring(11, 13).toInt();;
  timeDay = string.substring(14, 16).toInt();;
}

String formatTime(int hour, int minute, int second, boolean useSecond) { //Turns int into time-formatted String, adding an extra 0 at the front if the length is 1
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

String formatInt(int i) {
  String stringI = String(i);
  if (stringI.length() == 1) {
    stringI = "0" + stringI;
  }
  return stringI;
}

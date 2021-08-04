#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <ezButton.h>
#include "EEPROM.h"

#define FF17 &FreeSans9pt7b
#define FF21 &FreeSansBold9pt7b
#define FF35 &FreeSansBold18pt7b
#define FF47 &FreeSansBold24pt7b

#define ROW1 0,16
#define ROW3 0,62

int addr = 0;
#define EEPROM_SIZE 30

const int buzzer = 25;
const int ADC_PIN = 34;

long batteryPrevMillis = 0;
int batteryInterval = 400;
const int VREF = 1100;

const int sleepInterval = 10; //min
const int SHORT_PRESS_TIME = 500; // x seconds
const int LONG_PRESS_TIME  = 1000; // x seconds
int  blinkingTime = 500; //blinking time during alarm set

int previousTemperature = 0;
int currentTemperature = 0;
int TemperatureThreshold = 0;
int AlarmSetCheck = 0, blinkingchange = 0;
unsigned int pressedTime, releasedTime, blinkingmillis;
int alterCandF = 0;
int bothButtonCheck = 0;

unsigned int sleepTimerPrevMillis = millis();
unsigned int wakeTimerPrevMillis = millis();

int secondCounter = 0, minuteCounter = 0;
int hotstate = 0;

ezButton buttonUp(35); //35 //26
ezButton buttonLow(0); //0 //27

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
TFT_eSPI tft = TFT_eSPI();

bool isButtonlowShort, isButtonlowLong, isButtonBoth, isButtonUpNormal;

int wakeupCheck = 1;

RTC_DATA_ATTR int bootCount = 0;

void setup() {

  Serial.begin(115200);
  Serial.println("Device Started");

  mlx.begin();

  pinMode(buzzer, OUTPUT);

  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM");
  }
  else {
    TemperatureThreshold = eeGetInt(0);
    alterCandF = eeGetInt(10);
    wakeupCheck = eeGetInt(20);
    Serial.print("Memory Temperature Alarm Value: "); Serial.println(TemperatureThreshold);
    Serial.print("Memory C/F selection: "); Serial.print(alterCandF);
    if (alterCandF == 1) Serial.println(" Celcius");
    if (alterCandF == 0) Serial.println("  Farenheit");
  }

  buttonUp.setDebounceTime(50); // set debounce time to 50 milliseconds
  buttonLow.setDebounceTime(50); // set debounce time to 50 milliseconds

  if (buttonUp.isPressed()) { //check for long button press
    pressedTime = millis();
    Serial.println("UP button Pressed for wake");
  }

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.fillRect(0, 0, 240, 24, TFT_MAROON);
  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(ROW1);
  tft.print("   ODT Pocket Temper V2");
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(ROW3);
  tft.print("    Banger");
  tft.setCursor(0, 82);
  tft.print("     Temp:");

  tft.setFreeFont(FF35);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(130 , 78);
  currentTemperature = mlx.readObjectTempF(); //mlx.readObjectTempC();
  tft.print(currentTemperature);
  tft.setCursor(197, 57);
  tft.print(".");
  tft.setCursor(205, 78);
  tft.print("F");

  tft.fillRect(0, 110, 240, 25, TFT_DARKGREY); // tft.fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_RED);
  tft.setCursor(187, 127);
  tft.print("A:");
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(207, 127); // Row 6
  tft.print(TemperatureThreshold);

  tft.fillRect(4, 116, 26, 14, TFT_BLACK);
  tft.fillRect(6, 118, 22, 10, TFT_GREEN);
  tft.fillRect(29, 120, 3, 6, TFT_BLACK);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0); //1 = High, 0 = Low //35 //26

  //wakeupCheck = 0;

  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  Serial.printf("Wakeup was caused by : %d\n", wakeup_reason);
  //if(wakeup_reason == 2) wakeupCheck = 0;

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  if (bootCount == 1) wakeupCheck = 1;

  minuteCounter = 0;

  wakeTimerPrevMillis = millis();

  normalTemperatureScreen();
}
int btnUpState, btnLowState;

void loop() {

  buttonUp.loop(); // MUST call the loop() function first
  buttonLow.loop(); // MUST call the loop() function first

  checkButtonStates();

  sleepTimer();

  buzzerfunction();

  batteryfunction();

  resetTemperatureThresholdFunction();

  normalTemperatureScreen();

  alarmSetSecreen();

  buttonUpFunction();

  buttonLowFunction();

  resetSleep();

}

void sleepTimer() {
  if (millis() - sleepTimerPrevMillis > 1000) {
    sleepTimerPrevMillis = millis();
    secondCounter = secondCounter + 1;
  }
  if (secondCounter == 60) {
    minuteCounter = minuteCounter + 1;
    Serial.print("minutes: ");
    Serial.println(minuteCounter);
    secondCounter = 0;
  }


  if (minuteCounter == sleepInterval) {
    Serial.println();
    Serial.println("Going to sleep now due to timer");
    bootCount = 0;
    delay(100);
    esp_deep_sleep_start();
  }
}


void resetSleep(){
  if(mlx.readObjectTempF() >= 150){
    minuteCounter = 0;
    //Serial.print("Tempertaure: "); Serial.println(mlx.readObjectTempF());
    //Serial.println("  minuteCounter is resetted");
  }
}

void buzzerfunction() {
  int objectFahrenheit = mlx.readObjectTempF();
  if ((objectFahrenheit >= (TemperatureThreshold - 3)) && (objectFahrenheit <= (TemperatureThreshold + 3)) && (TemperatureThreshold != 0)) {
    Serial.println("Buzzer Started");
    Serial.print("objectFahrenheit: "); Serial.println(objectFahrenheit);
    Serial.print("TemperatureThreshold   -3: "); Serial.print(TemperatureThreshold - 3); Serial.print(" +3: "); Serial.println(TemperatureThreshold + 3);
    digitalWrite(buzzer, HIGH);
    delay(400);
    digitalWrite(buzzer, LOW);
    Serial.println("Buzzer Stop");
  }
}

void batteryfunction() {

  if (millis() - batteryPrevMillis > batteryInterval) {

    int adc = analogRead(ADC_PIN);
    float batteryVoltage = ((float)adc / 4095.0) * 2.0 * 3.3 * (VREF / 1000.0);

    tft.fillRect(0, 111, 90, 25, TFT_DARKGREY); //clear screen for battery
    if (batteryVoltage > 4.2) {
      tft.fillRect(4, 116, 26, 14, TFT_BLACK);
      tft.fillRect(6, 118, 22, 10, TFT_BLUE);
      tft.fillRect(29, 120, 3, 6, TFT_BLACK);
      tft.setFreeFont(FF17);
      tft.setTextColor(TFT_BLACK);
      tft.setCursor(12, 127); // Row 6
      tft.print("c");
    }
    else {
      batteryVoltage = constrain(batteryVoltage, 3, 4);
      float batteryPercent = +((batteryVoltage - 3) / 1) * 100;
      if (batteryPercent < 25) {
        tft.fillRect(4, 116, 26, 14, TFT_BLACK);
        tft.fillRect(6, 118, 22, 10, TFT_DARKGREY);
        tft.fillRect(6, 118, 5.5, 10, TFT_RED);
        tft.fillRect(29, 120, 3, 6, TFT_BLACK);
      }
      else if (batteryPercent < 50) {
        tft.fillRect(4, 116, 26, 14, TFT_BLACK);
        tft.fillRect(6, 118, 22, 10, TFT_DARKGREY);
        tft.fillRect(6, 118, 11, 10, TFT_YELLOW);
        tft.fillRect(29, 120, 3, 6, TFT_BLACK);
      }
      else if (batteryPercent < 70) {
        tft.fillRect(4, 116, 26, 14, TFT_BLACK);
        tft.fillRect(6, 118, 22, 10, TFT_DARKGREY);
        tft.fillRect(6, 118, 16.5, 10, TFT_GREEN);
        tft.fillRect(29, 120, 3, 6, TFT_BLACK);
      }
      else {
        tft.fillRect(4, 116, 26, 14, TFT_BLACK);
        tft.fillRect(6, 118, 22, 10, TFT_GREEN);
        tft.fillRect(29, 120, 3, 6, TFT_BLACK);
      }
    }

    batteryPrevMillis = millis();
  }

}


void normalTemperatureScreen() {

  if ((btnUpState == 1) && (btnLowState == 1) && (AlarmSetCheck == 0)) {
    //Serial.println("Current temperature displayed when nothing in process");

    if (alterCandF == 1) currentTemperature = mlx.readObjectTempC();
    if (alterCandF == 0) currentTemperature = mlx.readObjectTempF();
    // currentTemperature = 718;
    if (currentTemperature != previousTemperature) {

      Serial.print("Tempertaure: "); Serial.println(currentTemperature);

      if (currentTemperature <= 715) {
        tft.fillRect(130, 30, 100, 80, TFT_BLACK); //blinking  screen
        tft.fillRect(205, 50, 40, 30, TFT_BLACK); //clear screen to alter between C and F
        tft.setFreeFont(FF35);
        if (currentTemperature < 500) tft.setTextColor(TFT_WHITE);
        if ((currentTemperature >= 500) && (currentTemperature <= 600)) tft.setTextColor(TFT_BLUE);
        if ((currentTemperature >= 601) && (currentTemperature <= 675)) tft.setTextColor(TFT_GREEN);
        if ((currentTemperature >= 676) && (currentTemperature <= 715)) tft.setTextColor(TFT_RED);

        tft.setCursor(130 , 78);
        tft.print(currentTemperature);
        tft.setCursor(197, 57);
        tft.print(".");
        tft.setCursor(205, 78);
        if (alterCandF == 1) {
          tft.print("C");
          Serial.println("  Normal Screen Celcius");
        }
        if (alterCandF == 0) {
          tft.print("F");
          Serial.println("  Normal Screen Farenheit");
        }
        hotstate = 0;
      }

      if ((currentTemperature > 715) && (hotstate == 0)) {
        tft.fillRect(130, 30, 100, 80, TFT_BLACK); //blinking  screen
        tft.fillRect(205, 50, 40, 30, TFT_BLACK); //clear screen to alter between C and F
        Serial.print("Tempertaure: "); Serial.println(currentTemperature);
        tft.setCursor(197, 57);
        tft.print(".");
        tft.setCursor(205, 78);
        if (alterCandF == 1) {
          Serial.println("  Normal Screen Celcius");
        }
        if (alterCandF == 0) {
          Serial.println("  Normal Screen Farenheit");
        }
        tft.setFreeFont(FF35);
        tft.setTextColor(TFT_RED);
        tft.setCursor(130 , 60);
        tft.print("TOO");
        tft.setCursor(130 , 90);
        tft.print("HOT!");
        Serial.println("TOO HOT!");
        hotstate = 1;
      }
    }
    //delay(1000);
    previousTemperature = currentTemperature;
    //previousTemperature = 444;
  }
}

void eeWriteInt(int pos, int val) {
  byte* p = (byte*) &val;
  EEPROM.write(pos, *p);
  EEPROM.write(pos + 1, *(p + 1));
  EEPROM.write(pos + 2, *(p + 2));
  EEPROM.write(pos + 3, *(p + 3));
  EEPROM.commit();
}

int eeGetInt(int pos) {
  int val;
  byte* p = (byte*) &val;
  *p        = EEPROM.read(pos);
  *(p + 1)  = EEPROM.read(pos + 1);
  *(p + 2)  = EEPROM.read(pos + 2);
  *(p + 3)  = EEPROM.read(pos + 3);
  return val;
}


void buttonUpFunction() {

  if (buttonUp.isPressed()) { //check for long button press
    pressedTime = millis();
  }

  if ((buttonUp.isReleased()) && (isButtonUpNormal == true)) {
    // wakeupCheck = wakeupCheck+1;
    //if(wakeupCheck > 1){
    releasedTime = millis();
    Serial.println();
    Serial.println("UP button Pressed");
    //wakeupCheck = wakeupCheck+1;

    unsigned int pressDurationUp = releasedTime - pressedTime;

    if ((wakeupCheck == bootCount) && (pressDurationUp > LONG_PRESS_TIME) && (AlarmSetCheck == 0)) {
      Serial.println();
      Serial.println("Going to sleep now due to long button press");
      delay(100);
      esp_deep_sleep_start();
    }

    if (AlarmSetCheck >= 1) {
      TemperatureThreshold = TemperatureThreshold + 10;
      Serial.print("Temperature Threshold: "); Serial.println(TemperatureThreshold);
      tft.fillRect(130, 30, 100, 80, TFT_BLACK); //blinking  screen
      tft.setTextColor(TFT_WHITE);
      tft.setFreeFont(FF35);
      tft.setCursor(130 , 78);
      tft.println(TemperatureThreshold);
    }
    else if ((wakeupCheck == bootCount) && (millis() - wakeTimerPrevMillis > 2000)) {
      alterCandF = !alterCandF;
      tft.fillRect(130, 30, 100, 80, TFT_BLACK); //blinking  screen
      tft.setFreeFont(FF35);
      tft.setTextColor(TFT_WHITE);
      tft.setCursor(130 , 78);
      if (alterCandF == 1) currentTemperature = mlx.readObjectTempC();
      if (alterCandF == 0) currentTemperature = mlx.readObjectTempF();
      tft.print(currentTemperature);
      tft.setCursor(197, 57);
      tft.print(".");
      tft.fillRect(205, 50, 40, 30, TFT_BLACK); //clear screen to alter between C and F
      tft.setTextColor(TFT_WHITE);
      tft.setCursor(205, 78);
      if (alterCandF == 1) {
        tft.print("C");
        Serial.println("  Temperarure Set to Celcius");
      }
      if (alterCandF == 0) {
        tft.print("F");
        Serial.println("  Temperarure Set to Farenheit");
      }
      eeWriteInt(10, alterCandF);
    }
    eeWriteInt(20, bootCount);
    wakeupCheck = bootCount;
    isButtonUpNormal = false;
    //}
  }
}


void buttonLowFunction() {

  if (buttonLow.isPressed()) { //check for long button press
    pressedTime = millis();
  }

  if ((buttonLow.isReleased()) && (isButtonlowShort == true)) {
    Serial.println();
    Serial.println("LOW button Pressed");
    releasedTime = millis();
    AlarmSetCheck = AlarmSetCheck + 1;

    unsigned int pressDuration = releasedTime - pressedTime;
    Serial.println(pressDuration);

    if ( pressDuration < SHORT_PRESS_TIME ) {
      if (AlarmSetCheck == 1) {
        Serial.println("    Set New Temperature Threshold");
        TemperatureThreshold = 500;
        tft.fillRect(130, 50, 60, 40, TFT_BLACK); //alarm set screen
        tft.setFreeFont(FF35);
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(130 , 78);
        tft.print(TemperatureThreshold);
        blinkingmillis = millis();
      }

      if (AlarmSetCheck > 1) {
        TemperatureThreshold = TemperatureThreshold - 10;
        Serial.print("Temperature Threshold: "); Serial.println(TemperatureThreshold);
        tft.fillRect(130, 30, 100, 80, TFT_BLACK); //blinking  screen
        tft.setTextColor(TFT_WHITE);
        tft.setFreeFont(FF35);
        tft.setCursor(130 , 78);
        tft.println(TemperatureThreshold);
      }
    }

    if (pressDuration > LONG_PRESS_TIME ) {
      Serial.print("Temperature Threshold Set to  "); Serial.println(TemperatureThreshold);
      tft.fillRect(130, 30, 100, 80, TFT_BLACK); //blinking  screen
      AlarmSetCheck = 0;
      normalTemperatureScreen();
      tft.fillRect(200, 112, 100, 23, TFT_DARKGREY); //clear screen for bottom alarm
      tft.setFreeFont(FF21);
      tft.setTextColor(TFT_GREEN);
      tft.setCursor(207, 127); // Row 6
      tft.print(TemperatureThreshold);
      eeWriteInt(0, TemperatureThreshold);
    }

    isButtonlowShort = false;
  }
}


void resetTemperatureThresholdFunction() {

  if ((isButtonBoth == true) && (btnUpState == 1) && (btnLowState == 1) && (AlarmSetCheck == 0)) {

    isButtonBoth = false;
    Serial.println();
    Serial.println("Both buttons were Pressed");
    Serial.println("Temperature reset to 0");
    TemperatureThreshold = 0;
    Serial.print("Temperature Threshold: "); Serial.println(TemperatureThreshold);
    tft.fillRect(200, 112, 100, 23, TFT_DARKGREY); //clear screen for bottom alarm
    tft.setFreeFont(FF21);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(207, 127); // Row 6
    tft.print(TemperatureThreshold);
    eeWriteInt(0, TemperatureThreshold);
  }
}


void alarmSetSecreen() {

  if (AlarmSetCheck >= 1) {

    if ((millis() - blinkingmillis) > blinkingTime) {
      blinkingmillis = millis();
      blinkingchange = !blinkingchange;
    }

    if (blinkingchange == 1) {
      tft.fillRect(130, 30, 100, 80, TFT_BLACK); //blinking  screen
      //Serial.println("Blink Alarm");
    }
    else {
      tft.setTextColor(TFT_WHITE);
      tft.setFreeFont(FF35);
      tft.setCursor(130 , 78);
      tft.println(TemperatureThreshold);
      //Serial.print("Temperature Threshold: "); Serial.println(TemperatureThreshold);
    }
  }

}


void  checkButtonStates() {

  btnUpState = buttonUp.getState();
  btnLowState = buttonLow.getState();

  //  Serial.print("UP button state: "); Serial.println(btnUpState);
  //  Serial.print("LOW button state: "); Serial.println(btnLowState);

  if ((btnUpState == 0) && (btnLowState == 0)) {
    isButtonlowShort = false;
    isButtonlowLong = false;
    isButtonBoth = true;
    isButtonUpNormal = false;
    sleepTimerPrevMillis = millis();
    secondCounter = 0;
    minuteCounter = 0;
    delay(500);
  }

  if ((btnUpState == 1) && (btnLowState == 0)) {
    isButtonlowShort = true;
    isButtonlowLong = true;
    isButtonBoth = false;
    isButtonUpNormal = false;
    sleepTimerPrevMillis = millis();
    secondCounter = 0;
    minuteCounter = 0;
  }

  if ((btnUpState == 0) && (btnLowState == 1)) {
    isButtonlowShort = false;
    isButtonlowLong = false;
    isButtonBoth = false;
    isButtonUpNormal = true;
    sleepTimerPrevMillis = millis();
    secondCounter = 0;
    minuteCounter = 0;
  }
}

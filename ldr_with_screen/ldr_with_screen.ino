/*
 * HelTec Automation(TM) ESP32 Series Dev boards OLED draw Simple Function test code
 * - Some OLED draw Simple Function function test;
 * by LXYZN from HelTec AutoMation, ChengDu, China
 * www.heltec.cn
 *
 * this project also releases on GitHub:
 * https://github.com/HelTecAutomation/Heltec_ESP32
*/

// This example just provide basic function test;
// For more informations, please vist www.heltec.cn or mail to support@heltec.cn

#include "Arduino.h"
#include "heltec.h"
#include "images.h"

#define Analog 7  //Analog output of Light Sensor Module connected to A0 of BBFuino
#define DEMO_DURATION 3000
typedef void (*Demo)(void);

int demoMode = 0;
int counter = 1;

//Global variable
int AnalogValue = 0;  //to store analog value

void setup() {
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
  pinMode(Analog, INPUT);
}

void drawFontFaceDemo() {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_24);
    AnalogValue = analogRead(Analog);
    Heltec.display->drawString(0, 15, String(AnalogValue));
}

Demo demos[] = {drawFontFaceDemo};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;

void loop() {
  // clear the display
  Heltec.display->clear();
  // draw the current demo method
  demos[demoMode]();

  Heltec.display->setTextAlignment(TEXT_ALIGN_RIGHT);
  Heltec.display->drawString(10, 128, String(millis()));
  // write the buffer to the display
  Heltec.display->display();

  if (millis() - timeSinceLastModeSwitch > DEMO_DURATION) {
    demoMode = (demoMode + 1)  % demoLength;
    timeSinceLastModeSwitch = millis();
  }
  counter++;
  delay(10);
}

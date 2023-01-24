#ifndef ARDUINO_INKPLATE10
#error "Wrong board selection for this example, please select Inkplate 10 in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch
#include "SdFat.h"    //Include library for SD card
#include "driver/rtc_io.h"
#include "Arduino.h"

#include "globals.h"
#include "draw.h"
// #include "network.h"

Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;

Draw d;

void connectWifi()
{

  int ConnectCount = 20;

  if (WiFi.status() != WL_CONNECTED)
  {
    while (WiFi.status() != WL_CONNECTED)
    {
      if (ConnectCount++ == 20)
      {
        Serial.println("Connect WiFi");
        WiFi.begin(globals::ssid, globals::password);
        Serial.println("Connecting.");
        ConnectCount = 0;
      }
      Serial.print(".");
      delay(1000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
  display.clearDisplay(); // Clear frame buffer of display

  // TODO should only happen if RTC is not set
  connectWifi();

  configTime(globals::gmtOffset_sec, globals::daylightOffset_sec, globals::ntpServer);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }

  WiFi.disconnect();
  // display.rtcSetTime(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);                               // Send time to RTC
  display.rtcSetDate(timeinfo.tm_wday, timeinfo.tm_mday, timeinfo.tm_mon, timeinfo.tm_year + 1900); // Send date to RTC

  display.rtcGetRtcData();
  d.update(display);

  Serial.println(display.readBattery());
  Serial.println(display.readTemperature(), DEC);

  // DO NOT DELETE
  esp_sleep_enable_timer_wakeup(globals::TIME_TO_SLEEP * globals::uS_TO_S_FACTOR); // Activate wake-up timer
  esp_deep_sleep_start();                                                          // Put ESP32 into deep sleep. Program stops here.

  Serial.println("End setup");
}

void loop()
{
}

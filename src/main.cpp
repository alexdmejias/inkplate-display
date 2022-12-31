#ifndef ARDUINO_INKPLATE10
#error "Wrong board selection for this example, please select Inkplate 10 in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch
#include "SdFat.h"    //Include library for SD card
#include "driver/rtc_io.h"
#include "Arduino.h"
#include "FreeSans18pt7b.h"

Inkplate display(INKPLATE_3BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)
SdFile file;

#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 30       // How long ESP32 will be in deep sleep (in seconds)

char baseImageDir[] = "day-images/%i/%i/%i.png";
int month = 1;
int day = 23;
RTC_DATA_ATTR int imageIndex = 0;
RTC_DATA_ATTR int refreshIndex = 0;

char *getImageAddress(char *imageAddress, int month, int day, int index)
{
  sprintf(imageAddress, baseImageDir, month, day, index);

  return imageAddress;
}

void drawBoxes()
{
  const uint8_t marginLeft = 60;
  const uint8_t marginTop = 142;
  const unsigned int amountOfBoxes = 364;
  const uint8_t columns = 28;
  const uint8_t rows = 13;
  const uint8_t boxSize = 30;
  const uint8_t boxMargin = 8;
  const uint8_t currDay = 182;

  for (uint8_t i = 0; i < rows; i++)
  {
    for (uint8_t j = 0; j < columns; j++)
    {
      int x = marginLeft + (boxMargin * (j + 1)) + (boxSize * j);
      int y = marginTop + (boxMargin * (i + 1)) + (boxSize * i);
      int curr = (i * columns) + j;

      if (curr <= currDay)
      {
        display.fillRoundRect(x, y, boxSize, boxSize, 2, BLACK);
      }
      else
      {
        display.drawRoundRect(x, y, boxSize, boxSize, 2, BLACK);
      }
    }
  }
  display.setCursor(marginLeft, 795 - marginLeft);
  display.setFont(&FreeSans18pt7b);
  display.setTextColor(0, 7);
  display.setTextSize(1);

  String num = String(round((currDay / 365.0) * 100), 0);

  display.print(String("Year: ") + num + String("% Complete"));
}

void drawImage()
{
  SdFile file;
  char *imageAddress;
  imageAddress = new char[60];
  imageAddress = getImageAddress(imageAddress, month, day, imageIndex);

  if (file.open(imageAddress, O_RDONLY))
  {
    Serial.println("file exists");
  }
  else
  {
    Serial.println("file does not exist");
    imageIndex = 0;
    imageAddress = getImageAddress(imageAddress, month, day, imageIndex);
  }

  display.drawImage(imageAddress, 0, 0, 0, 1);
  imageIndex++;
}

void setup()
{
  Serial.begin(115200);
  display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
  display.clearDisplay(); // Clear frame buffer of display

  // Init SD card. Display if SD card is init propery or not.
  if (display.sdCardInit())
  {
    Serial.println("SD Card ok! Reading data...");
    if (refreshIndex % 2 == 0)
    {
      drawImage();
    }
    else
    {
      drawBoxes();
    }
    refreshIndex++;
  }
  else
  {
    display.setCursor(50, 50);
    display.setTextColor(0, 7);
    display.setTextSize(4);
    display.print("error loading sd card");
    Serial.println("error loading sd card");
  }

  display.display();

  // DO NOT DELETE
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Activate wake-up timer -- wake up after 20s here
  esp_deep_sleep_start();                                        // Put ESP32 into deep sleep. Program stops here.

  Serial.println("End setup");
}

void loop()
{
}
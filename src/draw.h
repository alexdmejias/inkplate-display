#pragma once

#include <ArduinoJson.h>
#include "Inkplate.h"
#include "FreeSans18pt7b.h"
#include "globals.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "network.h"

Network network;

class Draw
{
    uint8_t daysInMonths[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

public:
    void update(Inkplate &d)
    {
        // if (d.sdCardInit())
        // {
        //     Serial.println("SD Card ok! Reading data...");
        //     if (globals::refreshIndex % 2 == 0)
        //     {
        //         drawImage(d);
        //     }
        //     else
        //     {
        // drawBoxes(d);
        //     }
        //     globals::refreshIndex++;
        // }
        // else
        // {
        //     d.setCursor(50, 50);
        //     d.setTextColor(0, 7);
        //     d.setTextSize(4);
        //     d.print("error loading sd card");
        //     Serial.println("error loading sd card");
        // }

        drawRedditPosts(d);

        d.display();
    }

private:
    uint8_t getAccumulativeDay(uint8_t month, uint8_t day)
    {
        uint8_t total = day - 1;
        for (uint8_t i = 0; i < month - 1; i++)
        {
            total += daysInMonths[i];
        }

        return total;
    }

    char *getImageAddress(char *imageAddress, int month, int day, int index)
    {
        sprintf(imageAddress, globals::baseImageDir, month, day, index);

        Serial.print("looking up image:");
        Serial.println(imageAddress);

        return imageAddress;
    }

    void drawBoxes(Inkplate &d)
    {
        const uint8_t marginLeft = 30;
        const uint8_t marginTop = 142;
        const int amountOfBoxes = 364;
        const uint8_t columns = 28;
        const uint8_t rows = 13;
        const uint8_t boxSize = 28;
        const uint8_t boxMargin = 8;
        const uint8_t currDay = getAccumulativeDay(d.rtcGetMonth(), d.rtcGetDay());
        const uint8_t month = d.rtcGetMonth();
        // const uint8_t day = d.rtcGetDay() - 1;
        const uint8_t day = d.rtcGetDay();

        // for (uint8_t i = 0; i < rows; i++)
        // {
        //   for (uint8_t j = 0; j < columns; j++)
        //   {
        //     int x = marginLeft + (boxMargin * (j + 1)) + (boxSize * j);
        //     int y = marginTop + (boxMargin * (i + 1)) + (boxSize * i);
        //     int curr = (i * columns) + j;

        //     if (curr <= currDay - 1)
        //     {
        //       d.fillRoundRect(x, y, boxSize, boxSize, 2, BLACK);
        //     }
        //     else
        //     {
        //       d.drawRoundRect(x, y, boxSize, boxSize, 2, BLACK);
        //     }

        //     if (curr == currDay)
        //     {
        //       d.drawThickLine(x, y, x + boxSize, y + boxSize, BLACK, 2);
        //       d.drawThickLine(x, y + boxSize, x + boxSize, y, BLACK, 2);
        //     }
        //   }
        // }

        for (uint8_t i = 0; i < 12; i++)
        {
            for (uint8_t j = 0; j < daysInMonths[i]; j++)
            {
                int x = marginLeft + (boxMargin * (j + 1)) + (boxSize * j);
                int y = marginTop + (boxMargin * (i + 1)) + (boxSize * i);

                if (i < month || i == month && j < day - 1)
                {
                    d.fillRoundRect(x, y, boxSize, boxSize, 2, BLACK);
                }
                else
                {
                    d.drawRoundRect(x, y, boxSize, boxSize, 2, BLACK);
                }

                // draw X on the current day
                // if (i == month && j == day)
                // {
                //     d.drawThickLine(x, y, x + boxSize, y + boxSize, BLACK, 2);
                //     d.drawThickLine(x, y + boxSize, x + boxSize, y, BLACK, 2);
                // }

                // draw date
                if (i == month && j == day - 1)
                {
                    int16_t xDate, yDate;
                    uint16_t wDate, hDate;

                    String dateToPrint = String(day);

                    if (day < 10)
                    {
                        dateToPrint = String("0") + day;
                    }

                    d.setTextSize(2);
                    d.getTextBounds(String(dateToPrint), x + 3, y + 6, &xDate, &yDate, &wDate, &hDate);
                    d.setCursor(xDate, yDate);
                    d.setTextColor(0, 7);
                    d.print(dateToPrint);
                }
            }
        }

        d.setCursor(marginLeft + boxMargin, 795 - marginLeft);
        d.setFont(&FreeSans18pt7b);
        d.setTextColor(0, 7);
        d.setTextSize(1);

        String num = String(floor((currDay / 365.0) * 100), 0);

        d.print(String("Year: ") + num + String("% Complete"));
    }

    void drawImage(Inkplate &d)
    {
        SdFile file;
        uint8_t month = d.rtcGetMonth() + 1;
        uint8_t day = d.rtcGetDay();

        char *imageAddress;
        imageAddress = new char[60];
        imageAddress = getImageAddress(imageAddress, month, day, globals::imageIndex);

        if (file.open(imageAddress, O_RDONLY))
        {
            Serial.println("file exists");
        }
        else
        {
            Serial.println("file does not exist");
            Serial.println();
            d.println("file does not exist");
            globals::imageIndex = 0;
            imageAddress = getImageAddress(imageAddress, month, day, globals::imageIndex);
        }

        d.drawImage(imageAddress, 0, 0, 0, 1);
        globals::imageIndex++;
    }

    void drawRedditPosts(Inkplate &d)
    {
        d.setCursor(0, 50);
        d.setFont(&FreeSans18pt7b);
        d.setTextColor(0, 7);
        int MAX_INPUT_LENGTH = 2000;
        DynamicJsonDocument doc(2000);
        network.connect();

        if (WiFi.status() == WL_CONNECTED)
        {
            HTTPClient http;

            http.getStream().setNoDelay(true);
            http.getStream().setTimeout(1);
            http.begin(reddit_listings_url);

            int httpResponseCode = http.GET();

            if (httpResponseCode > 0)
            {
                // Serial.println("alexalexalex aaaaaaaa");
                DeserializationError error = deserializeJson(doc, http.getStream());
                if (error)
                {
                    // Serial.println("alexalexalex eeeeeee");
                    Serial.print("deserializeJson() failed: ");
                    Serial.println(error.c_str());
                    return;
                }
                // Serial.println("alexalexalex fffffff");
                for (JsonObject item : doc.as<JsonArray>())
                {
                    // const char *title = item["title"];     // "has anyone else noticed an increase in people screaming at ...
                    const char *created = item["created"]; // "2/2/2023", "2/2/2023", "2/2/2023"

                    d.println(item["title"].as<const char *>());
                }

                // d.display();
                // Serial.println("alexalexalex bbbbb");
            }
            else
            {
                // Serial.println("alexalexalex gggggg");
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            }
            // Serial.println("alexalexalex ccccccccc");
            http.end();
        }
        else
        {
            // Serial.println("alexalexalex ddddddd");
            Serial.println("WiFi Disconnected");
        }
    }
};
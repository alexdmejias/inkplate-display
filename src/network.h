#pragma once

#include "Inkplate.h"
#include <WiFi.h>
#include <time.h>
#include "globals.h"

uint8_t hour;
uint8_t minutes;
uint8_t seconds;

// Set date and weekday (NOTE: In weekdays 0 means Sunday, 1 menas Monday, ...)
uint8_t weekday;
uint8_t day;
uint8_t month;
int16_t year; // 2 digits

class Network
{
public:
    void connectWifi(Inkplate &d)
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
        else
        {
            setTime(d);
        }
    }

    bool connect()
    {
        WiFi.begin(ssid_name, ssid_password); // Try to connect to WiFi network
        Serial.println("drawRedditPosts()");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    int setupClientForStream(HTTPClient &http, const char *url)
    {
        http.getStream().setNoDelay(true);
        http.getStream().setTimeout(1);
        http.begin(url);

        return http.GET();
    }

    void setTime(Inkplate &d)
    {
        configTime(globals::gmtOffset_sec, globals::daylightOffset_sec, globals::ntpServer);

        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("Failed to obtain time");
            return;
        }
        Serial.println("Setup local time from NTP");
        Serial.println(&timeinfo, "%H:%M:%S %A, %d/%B/%Y");

        // Setup local time from NTP
        uint8_t hour = timeinfo.tm_hour;   // hours since midnight 0-23
        uint8_t minutes = timeinfo.tm_min; // minutes after the hour 0-59
        uint8_t seconds = timeinfo.tm_sec; // seconds after the minute 0-60

        uint8_t weekday = timeinfo.tm_wday;     // days since Sunday 0-6
        uint8_t day = timeinfo.tm_mday;         // day of the month 1-31
        uint8_t month = timeinfo.tm_mon + 1;    // months since January 0-11
        int16_t year = timeinfo.tm_year + 1900; // years since 1900

        d.rtcSetTime(hour, minutes, seconds);    // Send time to RTC
        d.rtcSetDate(weekday, day, month, year); // Send date to RTC
    }

    void disconnect()
    {
        WiFi.disconnect();
    }
};
#pragma once

#include "Inkplate.h"
#include "secrets.h"

namespace globals
{
    int uS_TO_S_FACTOR = 1000000; // Conversion factor for micro seconds to seconds
    uint8_t TIME_TO_SLEEP = 30;   // How long ESP32 will be in deep sleep (in seconds)
    RTC_DATA_ATTR int imageIndex = 0;
    RTC_DATA_ATTR int refreshIndex = 0;
    char baseImageDir[] = "day-images/%i/%i/%i.png";

    const char *ssid = ssid_name;
    const char *password = ssid_password;
    const char *ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = -18000;
    const int daylightOffset_sec = 3600;
} // namespace globals

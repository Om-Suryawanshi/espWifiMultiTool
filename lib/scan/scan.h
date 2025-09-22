#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <functional> 
#include <ArduinoJson.h>
#include <LittleFS.h>

class Scan
{
public:
    Scan();
    void handleWifiScan(int networksFound);
    void beginScan();
    bool isSafe();

private:
    bool scanInProgress;
    bool writingToFile;
    int scanResult;
    char mac[18];
};
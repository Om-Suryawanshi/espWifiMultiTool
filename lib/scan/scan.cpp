#include <scan.h>

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <functional> 

Scan::Scan()
{

}

void Scan::handleWifiScan(int networksFound)
{
    Serial.printf("%d network(s) found\n", networksFound);
    for (int i = 0; i < networksFound; i++)
    {
        Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
    }
}

void Scan::beginScan()
{
    WiFi.scanNetworksAsync(std::bind(&Scan::handleWifiScan, this, std::placeholders::_1), true);
}
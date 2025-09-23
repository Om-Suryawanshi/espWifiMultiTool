#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <scan.h>
#include <attack.h>

class ServerManager
{
public:
    ServerManager();
    void begin();
    void handle();

private:
    DNSServer dnsServer;
    ESP8266WebServer webServer;
    Scan scan;
    Attack attack;
    char ssid[32] = "nop";
    char pswd[32] = "1234567890";
    bool loadConfig();
};

#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

class ServerManager {
public:
    ServerManager();
    void begin(const char* ssid, const char* password);
    void handle();
private:
    DNSServer dnsServer;
    ESP8266WebServer webServer;
};

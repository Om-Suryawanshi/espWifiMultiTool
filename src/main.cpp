#include <Arduino.h>
#include <LittleFS.h>
#include <webserver.h>
#include <scan.h>

void printStatus(String status)
{
  Serial.println(status);
}

ServerManager server;
String status = "0";

void setup()
{
  Serial.begin(115200);
  if (!LittleFS.begin())
  {
    status = "LittleFS mount failed, formatting...";
    Serial.println("LittleFS mount failed, formatting...");
    LittleFS.format();
  }
    server.begin();
}

void loop()
{
  server.handle();
  // printStatus(status);
}
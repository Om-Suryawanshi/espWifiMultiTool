#include <Arduino.h>
#include <LittleFS.h>
#include <webserver.h>
#include <scan.h>

bool loadConfig();
void listFS();

void printStatus(String status)
{
  Serial.println(status);
}

ServerManager server;
char ssid[32] = "nop";
char pswd[32] = "1234567890";
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
  if(loadConfig())
  {
    status = "ConfigLoaded";
    server.begin(ssid, pswd);
  }
  status = "Something fked up";
  Scan scan;
  scan.beginScan();
}

void loop()
{
  server.handle();
  printStatus(status);
  delay(1000);
}

bool loadConfig()
{
  if (!LittleFS.exists("/config.cfg"))
  {
    status = "config doesnt exits";
    return false;
  }
  File config = LittleFS.open("/config.cfg", "r");
  if (!config)
  {
    status = "config dosent open";
    return false;
  }
  String s = config.readStringUntil('\n');
  String p = config.readStringUntil('\n');
  config.close();
  s.trim();
  p.trim();
  strncpy(ssid, s.c_str(), sizeof(ssid) - 1);
  strncpy(pswd, p.c_str(), sizeof(pswd) - 1);
  return true;
}

void listFS()
{
  Serial.println("== LittleFS root ==");
  Dir dir = LittleFS.openDir("/");
  while (dir.next())
  {
    Serial.printf("%s\t(%u bytes)\n", dir.fileName().c_str(), dir.fileSize());
  }
  Serial.println("==================");
}
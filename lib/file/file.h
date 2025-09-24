#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

class FileManager
{
public:
    FileManager();
    String listAllFiles();
    String getFileContent(String path);
};
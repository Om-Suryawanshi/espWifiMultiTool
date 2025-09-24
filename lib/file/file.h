#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

class FileManager
{
public:
    FileManager();
    String listAllFiles(String path);
    String getFileContent(String path);
};
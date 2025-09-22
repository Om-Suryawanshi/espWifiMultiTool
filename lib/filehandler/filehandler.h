#include <LittleFS.h>
#include <Arduino.h>

class File
{
public:
    File();
    void begin();
    void readFile();
    void writeFile(String filename);
};
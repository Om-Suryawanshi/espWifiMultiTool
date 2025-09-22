#include <scan.h>

Scan::Scan()
{
    writingToFile = false;
    scanInProgress = false;
}

bool Scan::isSafe()
{
    return !writingToFile && !scanInProgress;
}

void Scan::handleWifiScan(int networksFound)
{
    if (!scanInProgress)
    {
        return;
    }

    if (networksFound == -1)
    {
        Serial.println("Scan failed or cancelled.");
        scanInProgress = false;
        WiFi.scanDelete();
        return;
    }

    // Process the scan results
    if (networksFound >= 0)
    {
        scanInProgress = false;
        writingToFile = true;

        DynamicJsonDocument doc(8192); // Use a large buffer size
        JsonArray networks = doc.createNestedArray("networks");

        for (int i = 0; i < networksFound; i++)
        {
            JsonObject network = networks.createNestedObject();

            String ssid = WiFi.SSID(i);
            ssid.replace("\"", ""); // Remove quotes
            ssid.replace("\\", ""); // Remove backslashes
            network["ssid"] = ssid;

            network["rssi"] = WiFi.RSSI(i);

            network["enc"] = WiFi.encryptionType(i);

            uint8_t *bssid = WiFi.BSSID(i);
            sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
                    bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
            network["bssid"] = mac;
            Serial.printf("%d: SSID: %s, RSSI: %d, Encryption: %d, BSSID: %s\n",
                          i + 1,
                          network["ssid"].as<const char *>(),
                          network["rssi"].as<int>(),
                          network["enc"].as<int>(),
                          network["bssid"].as<const char *>());
        }

        // Write the complete JSON document to the file
        File file = LittleFS.open("/api/scan.json", "w");
        if (file)
        {
            serializeJson(doc, file);
            file.close();
            Serial.println("Done Writing to file /api/scan.json");
        }
        else
        {
            Serial.println("Failed to open file /api/scan.json");
        }

        writingToFile = false;
        WiFi.scanDelete();
    }
}

void Scan::beginScan()
{
    if (isSafe())
    {
        scanInProgress = true;
        WiFi.scanNetworksAsync(std::bind(&Scan::handleWifiScan, this, std::placeholders::_1), true);
    }
    else
    {
        Serial.println("Cannot start scan, system is busy");
    }
}
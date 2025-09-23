#include <attack.h>

extern "C"
{
#include "user_interface.h"
}

Attack *Attack::instance = nullptr;

Attack::Attack()
{
    instance = this;
}

// Beacon
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
        for (int ch = 1; ch <= 13; ch++)
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

void Attack::static_sniffer_callback(uint8_t *buff, uint16_t len)
{
    if (instance)
    {
        instance->handleProbePacket(buff, len);
    }
}
void Attack::handleProbePacket(uint8_t *buff, uint16_t len)
{
    bool debug = false;
    wifi_promiscuous_pkt_t *packet = (wifi_promiscuous_pkt_t *)buff;
    if ((packet->hdr.frame_ctrl & 0x00FC) == 0x0040)
    {
        char mac_addr[18];
        sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x",
                packet->hdr.addr2[0], packet->hdr.addr2[1], packet->hdr.addr2[2],
                packet->hdr.addr2[3], packet->hdr.addr2[4], packet->hdr.addr2[5]);

        uint8_t *payload = packet->payload;
        uint8_t ssid_len = payload[1];

        if (ssid_len > 0 && ssid_len < 32 && ssid_len < len)
        {
            char ssid[ssid_len + 1];
            memcpy(ssid, &payload[2], ssid_len);
            ssid[ssid_len] = '\0';
            Serial.printf("PROBE REQUEST from [%s] for SSID: [%s]\n", mac_addr, ssid);
        }
        else
        {
            Serial.printf("PROBE REQUEST from [%s] for SSID: <Broadcast>\n", mac_addr);
        }
    }
    if (debug)
    {
        // --- TEMPORARY DEBUGGING CODE ---
        // We are printing the details of EVERY packet, not just probe requests.
        uint16_t frameCtrl = packet->hdr.frame_ctrl;
        Serial.printf("Packet captured! Length: %d, Frame Control: 0x%04X\n", len, frameCtrl);
    }
}
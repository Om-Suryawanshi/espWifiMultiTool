#ifndef SCAN_H
#define SCAN_H

#include <ESP8266WiFi.h>
#include <vector>
#include <ArduinoJson.h>
#include <LittleFS.h>

// --- CORRECTED: The extern "C" block must be OUTSIDE the class definition ---
extern "C"
{
#include "user_interface.h"

    // SDK packet structures
    typedef struct
    {
        signed rssi : 8;
        unsigned rate : 4;
        unsigned is_group : 1;
        unsigned : 1;
        unsigned sig_mode : 2;
        unsigned legacy_length : 12;
        unsigned damatch0 : 1;
        unsigned damatch1 : 1;
        unsigned bssidmatch0 : 1;
        unsigned bssidmatch1 : 1;
        unsigned MCS : 7;
        unsigned CWB : 1;
        unsigned HT_length : 16;
        unsigned Smoothing : 1;
        unsigned Not_Sounding : 1;
        unsigned : 1;
        unsigned Aggregation : 1;
        unsigned STBC : 2;
        unsigned FEC_CODING : 1;
        unsigned SGI : 1;
        unsigned rxend_state : 8;
        unsigned ampdu_cnt : 8;
        unsigned channel : 4;
        unsigned : 12;
    } wifi_pkt_rx_ctrl_t;
    typedef struct
    {
        unsigned frame_ctrl : 16;
        unsigned duration_id : 16;
        uint8_t addr1[6];
        uint8_t addr2[6];
        uint8_t addr3[6];
        unsigned sequence_ctrl : 16;
    } wifi_mgmt_hdr_t;
    typedef struct
    {
        wifi_pkt_rx_ctrl_t rx_ctrl;
        uint8_t buf[112];
        uint16_t cnt;
        uint16_t len;
    } wifi_promiscuous_pkt_t;
}

class Scan
{
public:
    // Structure to hold AP info
    struct AccessPoint
    {
        String ssid;
        uint8_t bssid[6];
        int channel;
        int rssi;
        int packet_count = 0;
        std::vector<String> clients;
    };

    Scan();
    bool isSafe();
    void beginScan();
    void sniffer_callback(uint8_t *buf, uint16_t len);

private:
    bool writingToFile = false;
    bool scanInProgress = false;
    String mac_to_string(uint8_t *mac);
    int find_ap(uint8_t *bssid);
    void writeScanResults();
    bool loadConfig();
    char ssid[32] = "nop";
    char pswd[32] = "1234567890";
};

#endif
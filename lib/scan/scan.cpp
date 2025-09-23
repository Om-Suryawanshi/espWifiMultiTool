#include <scan.h>

namespace {
    std::vector<Scan::AccessPoint> ap_list;
    Scan *scan_instance = nullptr;
}

extern "C" void sniffer_callback_wrapper(uint8_t *buf, uint16_t len) {
    if (scan_instance) {
        scan_instance->sniffer_callback(buf, len);
    }
}

Scan::Scan() {
    scan_instance = this;
}

bool Scan::isSafe() {
    return !scanInProgress;
}

void Scan::beginScan() {
    if (!isSafe()) {
        Serial.println("Cannot start scan, system is busy");
        return;
    }

    scanInProgress = true;
    ap_list.clear();

    Serial.println("Starting advanced promiscuous scan for ~13 seconds...");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    wifi_set_opmode(STATION_MODE);
    wifi_promiscuous_enable(1);
    wifi_set_promiscuous_rx_cb(sniffer_callback_wrapper);

    // Scan all channels 5 times for good results
    int num_scans = 5;
    for (int i = 0; i < num_scans; i++) {
        for (int channel = 1; channel <= 13; channel++) {
            wifi_set_channel(channel);
            delay(200);
        }
    }

    wifi_promiscuous_enable(0); // Turn off promiscuous mode
    Serial.println("Scan finished. Writing results and restarting AP...");
    writeScanResults();

    WiFi.mode(WIFI_AP);
    loadConfig();
    WiFi.softAP(ssid, pswd);

    scanInProgress = false;
}

void Scan::writeScanResults() {
    writingToFile = true;
    DynamicJsonDocument doc(8192);
    JsonArray networks = doc.createNestedArray("networks");

    for (const auto &ap : ap_list) {
        JsonObject network = networks.createNestedObject();
        network["ssid"] = ap.ssid;
        network["channel"] = ap.channel;
        network["rssi"] = ap.rssi;
        // --- CLEANUP: Removed the unnecessary const_cast ---
        network["bssid"] = mac_to_string(const_cast<uint8_t*>(ap.bssid));
        network["clients"] = ap.clients.size();
        network["enc"] = "?";
    }

    File file = LittleFS.open("/api/scan.json", "w");
    if (file) {
        serializeJson(doc, file);
        file.close();
        Serial.println("Done writing to file /api/scan.json");
    } else {
        Serial.println("Failed to open file for writing");
    }
    writingToFile = false;
}

// The sniffer logic, now as a member of the Scan class
void Scan::sniffer_callback(uint8_t *buf, uint16_t len)
{
    if (buf == NULL)
        return;

    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
    wifi_mgmt_hdr_t *hdr = (wifi_mgmt_hdr_t *)pkt->buf;

    uint8_t *ap_bssid = nullptr;
    uint8_t *client_mac = nullptr;

    int to_ds = (hdr->frame_ctrl & 0x0100) != 0;
    int from_ds = (hdr->frame_ctrl & 0x0200) != 0;

    if (!to_ds && !from_ds)
    {
        ap_bssid = hdr->addr3;
        client_mac = hdr->addr2;
    }
    else if (!to_ds && from_ds)
    {
        ap_bssid = hdr->addr2;
        client_mac = hdr->addr1;
    }
    else if (to_ds && !from_ds)
    {
        ap_bssid = hdr->addr1;
        client_mac = hdr->addr2;
    }

    if (ap_bssid == nullptr || client_mac == nullptr)
        return;

    int ap_index = find_ap(ap_bssid);
    if (ap_index != -1)
    {
        ap_list[ap_index].packet_count++;
        ap_list[ap_index].rssi = pkt->rx_ctrl.rssi;
        String client_mac_str = mac_to_string(client_mac);
        if (client_mac_str != "ff:ff:ff:ff:ff:ff" && client_mac_str != "00:00:00:00:00:00")
        {
            if (std::find(ap_list[ap_index].clients.begin(), ap_list[ap_index].clients.end(), client_mac_str) == ap_list[ap_index].clients.end())
            {
                ap_list[ap_index].clients.push_back(client_mac_str);
            }
        }
    }

    if ((hdr->frame_ctrl & 0x00FC) == 0x0080)
    { // Beacon Frame
        if (find_ap(hdr->addr3) == -1)
        { // Only add if it's new
            uint8_t *payload = pkt->buf + 24 + 12;
            uint8_t ssid_len = payload[1];
            if (ssid_len > 0 && ssid_len < 32)
            {
                char ssid_name[ssid_len + 1];
                memcpy(ssid_name, payload + 2, ssid_len);
                ssid_name[ssid_len] = '\0';

                AccessPoint new_ap;
                new_ap.ssid = String(ssid_name);
                memcpy(new_ap.bssid, hdr->addr3, 6);
                new_ap.channel = wifi_get_channel();
                new_ap.rssi = pkt->rx_ctrl.rssi;
                ap_list.push_back(new_ap);
            }
        }
    }
}

// Helper function implementations
String Scan::mac_to_string(uint8_t *mac)
{
    char mac_str[18];
    sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(mac_str);
}

int Scan::find_ap(uint8_t *bssid)
{
    for (int i = 0; i < ap_list.size(); i++)
    {
        if (memcmp(ap_list[i].bssid, bssid, 6) == 0)
        {
            return i;
        }
    }
    return -1;
}

bool Scan::loadConfig()
{
    if (!LittleFS.exists("/config.cfg"))
    {
        return false;
    }
    File config = LittleFS.open("/config.cfg", "r");
    if (!config)
    {
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
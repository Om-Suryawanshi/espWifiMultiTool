#include <webserver.h>

#define DNS_PORT 53

ServerManager::ServerManager() : webServer(80) {}

void ServerManager::begin()
{
    loadConfig();
    // Start WiFi AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, pswd);
    
    IPAddress apIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(apIP);

    dnsServer.start(DNS_PORT, "wifi.tools", apIP); // http://www.wifitools/scan

    //Index
    webServer.on("/", HTTP_GET, [this]() 
    {
        File file = LittleFS.open("/static/index.html", "r");
        if(!file)
        {
            webServer.send(404, "text/html", "Index not found");
        }
        webServer.streamFile(file, "text/html");
        file.close();
    });

    // File System
    webServer.on("/filesystem", HTTP_GET, [this]()
    {
        File file = LittleFS.open("/static/file.html", "r");
        if(!file)
        {
            webServer.send(500, "text/html", "Index html file not found");
        }
        webServer.streamFile(file, "text/html");
        file.close(); 
    });

    webServer.on("/api/files", HTTP_GET, [this]() 
    {
        String json = fileManger.listAllFiles();
        webServer.send(200, "application/json", json);
    });

    webServer.on("/api/view", HTTP_GET, [this]() 
    {
        if(!webServer.hasArg("path"))
        {
            webServer.send(400, "text/plain", "Bad Request: 'path' arg missing");
            return;
        }
        String path = webServer.arg("path");
        String content = fileManger.getFileContent(path);
        webServer.send(200, "application/json", content);
    });

    // Scan
    webServer.on("/scan", HTTP_GET, [this]()
    {
        File file = LittleFS.open("/static/scan.html", "r");
        if (!file)
        {
            webServer.send(500, "text/html", "Scan html file not found");
        }
        webServer.streamFile(file, "text/html");
        file.close();
    });

    // Scan backend called by /scan
    webServer.on("/api/scan", HTTP_GET, [this]()
    {
        if(scan.isSafe())
        {
            File file = LittleFS.open("/api/scan.json", "r");
            if((!file))
            {
                webServer.send(500, "application/json", "scan.json not found");
            }
            webServer.streamFile(file, "application/json");
            file.close();
        }
        else
        {
            webServer.send(202, "application/json", "{\"status\":\"in_progress\"}");
        } 
    });

    // Scan wifi trigger POST
    webServer.on("/api/scan", HTTP_POST, [this]()
    {
        if(scan.isSafe())
        {
            scan.beginScan();
            webServer.send(202, "application/json", "{\"status\":\"scan_req_initiated\"}");
        }
        else
        {
            webServer.send(202, "application/json", "{\"status\":\"scan_req_processing\"}");
        } 
    });

    webServer.on("/beacon", HTTP_GET, [this]()
    {
        attack.loadSSIDs("ssid_nsfw.txt");
        webServer.send(200, "text/html", "Beacon Started. You will be disconnected");
        delay(100);
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        attack.startBeacon();
        Serial.println("Beacon Started."); 
    });

    webServer.on("/probe", HTTP_GET, [this]()
    {
        webServer.send(200, "text/html", "Probe started. You will be disconnected.");
        delay(100);
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        wifi_set_opmode(STATION_MODE);
        wifi_promiscuous_enable(1);
        wifi_set_promiscuous_rx_cb(Attack::static_sniffer_callback);
        Serial.println("Probe Started.");

        int channel = 1;
        while(true)
        {
            wifi_set_channel(channel);
            channel++;
            if(channel > 13)
            {
                channel = 1;
            }
            delay(100);
        } 
    });

    webServer.on("/deauth", HTTP_GET, [this]()
    {
        File file = LittleFS.open("/static/deauth.html", "r");
        if(!file)
        {
            webServer.send(404,"text/html" ,"deauth html file not found");
        }
        webServer.streamFile(file, "text/html");
        file.close(); 
    });

    // Add this new endpoint to your webserver
    webServer.on("/api/deauth", HTTP_POST, [this]()
    {
        if (webServer.hasArg("plain") == false)
        {
            webServer.send(400, "text/plain", "Body not received");
            return;
        }

        String body = webServer.arg("plain");
        DynamicJsonDocument doc(256);
        deserializeJson(doc, body);

        const char *bssid_str = doc["bssid"];
        int channel = doc["channel"];

        // Convert BSSID string back to uint8_t array
        uint8_t bssid[6];
        sscanf(bssid_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &bssid[0], &bssid[1], &bssid[2], &bssid[3], &bssid[4], &bssid[5]);

        // Use broadcast MAC to deauth all clients of the target AP
        uint8_t client_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        webServer.send(200, "application/json", "{\"status\":\"deauth_started\"}");
        delay(100);

        // Call your deauther attack (which should be in your Attack class)
        attack.startDeauth(bssid, client_mac, channel); 
    });

    webServer.begin();
    Serial.println("Web server started!");
}

void ServerManager::handle()
{
    dnsServer.processNextRequest();
    webServer.handleClient();
}

bool ServerManager::loadConfig()
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
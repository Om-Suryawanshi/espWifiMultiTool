#include <webserver.h>

#define DNS_PORT 53

ServerManager::ServerManager() : webServer(80) {}

void ServerManager::begin(const char *ssid, const char *password)
{
    // Start WiFi AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    IPAddress apIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(apIP);

    dnsServer.start(DNS_PORT, "*", apIP);

    // Index
    webServer.on("/", HTTP_GET, [this]()
    {
        File file = LittleFS.open("/static/index.html", "r");
        if(!file)
        {
            webServer.send(500, "text/html", "Index html file not found");
        }
        webServer.streamFile(file, "text/html");
        file.close();
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

    webServer.on("/startbeacon", HTTP_GET, [this]() 
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

    webServer.begin();
    Serial.println("Web server started!");
}

void ServerManager::handle()
{
    dnsServer.processNextRequest();
    webServer.handleClient();
}

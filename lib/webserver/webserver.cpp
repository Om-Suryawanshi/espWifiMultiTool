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
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        attack.startBeacon();
        webServer.send(200, "text/html", "Beacon Started");
        Serial.println("Beacon Started.");
    });

    webServer.on("/stopbeacon", HTTP_GET, [this]() 
    {
        attack.stopBeacon();
        webServer.send(200, "text/html", "Beacon Stoped");
    });

    webServer.begin();
    Serial.println("Web server started!");
}

void ServerManager::handle()
{
    dnsServer.processNextRequest();
    webServer.handleClient();
}

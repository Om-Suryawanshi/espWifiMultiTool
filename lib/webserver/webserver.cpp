#include <webserver.h>

#define DNS_PORT 53

ServerManager::ServerManager() : webServer(80) {}

void ServerManager::begin(const char* ssid, const char* password) {
    // Start WiFi AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    IPAddress apIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(apIP);

    // Start DNS server: redirect all queries to our ESP
    dnsServer.start(DNS_PORT, "*", apIP);

    // Serve index.html on any request
    webServer.onNotFound([this]() {
        File file = LittleFS.open("/static/index.html", "r");
        if (!file) {
            webServer.send(500, "text/plain", "File not found");
            return;
        }
        webServer.streamFile(file, "text/html");
        file.close();
    });

    webServer.begin();
    Serial.println("Web server started!");
}

void ServerManager::handle() {
    dnsServer.processNextRequest();
    webServer.handleClient();
}

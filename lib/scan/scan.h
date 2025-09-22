#pragma once

class Scan
{
public:
    Scan();
    void handleWifiScan(int networksFound);
    void beginScan();
};
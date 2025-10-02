# espWifiMultiTool (ESP8266)

A lightweight WiFi manager for **ESP8266** that helps manage multiple SSIDs,
auto-reconnect, and simple fallback behavior. Designed to be small and easy to
embed into other projects.

[License: MIT](LICENSE) · [PlatformIO](https://platformio.org)

---

## Features

- Manage multiple SSID / password pairs.  
- Auto-reconnect with fallback attempts.  
- Small, dependency-light design for ESP8266 projects.  
- Simple, documented API headers (see `include/`).

---

## Requirements

- ESP8266 development board (e.g., NodeMCU, Wemos D1 Mini).  
- PlatformIO (recommended) or Arduino IDE.  
- USB cable and serial monitor for debugging.

---

## Installation

1. Clone the repository:

   ```
   git clone https://github.com/Om-Suryawanshi/espWifiMultiTool.git
2. Open the project in PlatformIO (recommended).
3. Choose your ESP8266 board in platformio.ini.
4. Build and upload:
   ```
   pio run --target upload
5. Open the serial monitor:
   ```
   pio device monitor
---

##Contributing
1. Fork the repo.
2. Create a branch for your change: git checkout -b feature/your-feature.
3. Commit with clear messages.
4. Push and open a pull request.
- Please test on actual ESP8266 hardware before submitting PRs and include a short
description of how to reproduce and verify the change.

#pragma once

namespace env {
    const char* WIFI_SSID = "WiFi SSID (2.4 GHz only)";
    const char* WIFI_PASSWORD = "WiFiPassword";

    // Name of the device on the network (used for mDNS)
    // Device will be available at http://<MDNS_SERVER_NAME>.local
    const char* MDNS_SERVER_NAME = "rgb-led";
}

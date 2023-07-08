#pragma once

#include <ESP8266httpUpdate.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include "env.h"

class UpdateClient {
private:
    WiFiClient client;

public:
    UpdateClient(WiFiClient client) : client(client) {
        ESPhttpUpdate.rebootOnUpdate(true);
        ESPhttpUpdate.setLedPin(LED_BUILTIN);
    }

    void install(String updateServerURL, int port, String updatePath) {
        Serial.println("Installing update, version: " + updatePath);
        Serial.println("Update server: " + updateServerURL + ":" + String(port) + "/" + updatePath);
        t_httpUpdate_return ret = ESPhttpUpdate.update(client, updateServerURL, port, updatePath);

        switch (ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("Updated FAILED (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("[update] Update no Update.");
            break;
        case HTTP_UPDATE_OK:
            // may not be called since we reboot the ESP
            Serial.println("[update] Update ok.");
            break;
        }
        Serial.println("Update installed");
    }

    String version() {
        return device::VERSION;
    }
};

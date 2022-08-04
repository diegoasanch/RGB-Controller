#pragma once

#include <ESP8266httpUpdate.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include "env.h"

const String UPDATE_SERVER_URL = "192.168.0.223";
const String UPDATE_PATH = "/static/update/";
const String HTTP_ERROR = "HTTP_ERROR";
const int PORT = 3301;

class UpdateClient {
private:
    String url;
    int port;
    WiFiClient client;


public:
    UpdateClient(WiFiClient client) : client(client) {
        this->url = UPDATE_SERVER_URL;
        this->port = PORT;

        ESPhttpUpdate.rebootOnUpdate(true);
        ESPhttpUpdate.setLedPin(LED_BUILTIN);
    }

    void install(String version) {
        Serial.println("Installing update, version: " + version);
        t_httpUpdate_return ret = ESPhttpUpdate.update(client, this->url, this->port, UPDATE_PATH + version + ".bin");

        switch (ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("Updated FAILED (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("[update] Update no Update.");
            break;
        case HTTP_UPDATE_OK:
            Serial.println("[update] Update ok."); // may not be called since we reboot the ESP
            break;
        }
        Serial.println("Update installed");
    }

    String version() {
        return device::VERSION;
    }
};

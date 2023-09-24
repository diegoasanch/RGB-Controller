#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "../env.h"


//Static IP address configuration
IPAddress staticIP(192, 168, 0, 127); //ESP static ip
IPAddress gateway(192, 168, 0, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(8, 8, 8, 8);  //DNS

const char* deviceName = "deviceName.com";
String ipAddr = "";


void setupWifi() {

    int attempt = 1;
    Serial.println("Configuring to WiFi");
    WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
    if (!WiFi.config(staticIP, subnet, gateway)) {
        Serial.println("Failed to configure WiFi with DHCP");
        delay(1000);
    }
    wifi_set_sleep_type(NONE_SLEEP_T);
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(env::WIFI_SSID, env::WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
        attempt += 1;

        if (attempt > 1000) {
            delay(1000);
            ESP.restart();
        }
    }
    Serial.println("\nWiFi connected");
    ipAddr = WiFi.localIP().toString();
    delay(1000);
}

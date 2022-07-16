#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <uri/UriRegex.h>

#include "RGBLed.hpp"

class Api {
public:
    Api(int port, RGBLed* rgb) : server(port) {
        this->port = port;
        this->rgb = rgb;
    }

    void configure() {
        Serial.println("Configuring API");
        this->configureRoutes();
        Serial.println("Configured Routes");
        server.begin();
        Serial.println("Started server");
        MDNS.begin("esp8266");
        Serial.println("Started MDNS");
    }

    void handleRequests() {
        server.handleClient();
        MDNS.update();
    }
private:
    ESP8266WebServer server;
    int port;
    RGBLed* rgb;

    void configureRoutes() {
        server.on("/", [this]() {this->handleRoot();});
        server.on("/light/on", [this]() {this->turnOn();});
        server.on("/light/off", [this]() {this->turnOff();});
        server.on("/light/brightness", [this]() {this->getBrightness();});
        server.on(UriRegex("^\\/light\\/brightness\\/([0-9]+)$"), [this]() {this->setBrightness();});
        server.on("/light/hexColor", [this]() {this->getHexColor();});
        server.on(UriRegex("^\\/light\\/hexColor\\/([0-9a-fA-F]{6})$"), [this]() {this->setHexColor();});
    }

    void handleRoot() {
        server.send(200, "text/plain", "Hello friend.");
    }

    void getBrightness() {
        String brightness = String(this->rgb->getBrightness());
        server.send(200, "text/plain", brightness);
    }

    void setBrightness() {
        String brightness = server.pathArg(0);
        this->rgb->setBrightness((uint8)brightness.toInt());
        server.send(200, "text/plain", "Brightness set to " + brightness);
    }

    void setHexColor() {
        String hexColor = server.pathArg(0);
        this->rgb->setHexColor(hexColor);
        server.send(200, "text/plain", hexColor);
    }

    void getHexColor() {
        String color = this->rgb->getHexColor();
        server.send(200, "text/plain", color);
    }

    void turnOn() {
        this->rgb->turnOn();
        server.send(200, "text/plain", "1");
    }

    void turnOff() {
        this->rgb->turnOff();
        server.send(200, "text/plain", "0");
    }
};

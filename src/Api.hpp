#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <uri/UriRegex.h>

#include "Weather.hpp"
#include "RGBLed.hpp"

class Api {
public:
    Api(int port, RGBLed* rgb, Weather* weather) : server(port) {
        this->port = port;
        this->rgb = rgb;
        this->weather = weather;
    }

    void configure() {
        Serial.println("Configuring API");

        this->configureRoutes();
        Serial.println("Configured Routes");
        this->configureMiddleware();
        Serial.println("Configured Middleware");

        server.begin();
        Serial.println("Started server");

        if (MDNS.begin("esp8266"))
            Serial.println("Started MDNS");
        else
            Serial.println("Error starting MDNS");
    }

    void handleRequests() {
        server.handleClient();
        MDNS.update();
    }
private:
    ESP8266WebServer server;
    int port;
    RGBLed* rgb;
    Weather* weather;

    void configureRoutes() {
        server.on("/", [this]() {this->handleRoot();});
        // Light
        // -- Toggle
        server.on("/light/on", [this]() {this->turnOn();});
        server.on("/light/off", [this]() {this->turnOff();});
        server.on("/light/toggle", [this]() {this->toggle();});
        // -- Brightness
        server.on("/light/brightness", [this]() {this->getBrightness();});
        server.on(UriRegex("^\\/light\\/brightness\\/([0-9]+)$"), [this]() {this->setBrightness();});
        // -- Color
        server.on("/light/hexColor", [this]() {this->getHexColor();});
        server.on(UriRegex("^\\/light\\/hexColor\\/([0-9a-fA-F]{6})$"), [this]() {this->setHexColor();});

        // Weather
        server.on("/weather/temperature", [this]() {this->getTemperature();});
        server.on("/weather/humidity", [this]() {this->getHumidity();});

        // Utils
        server.on("/stats", [this]() {this->stats();});
    }

    // -------- Light --------
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

    // TODO: Give "light-x" naming
    void turnOn() {
        this->rgb->turnOn();
        server.send(200, "text/plain", "1");
    }

    void turnOff() {
        this->rgb->turnOff();
        server.send(200, "text/plain", "0");
    }

    void toggle() {
        bool status = this->rgb->toggle();
        server.send(200, "text/plain", status ? "1" : "0");
    }

    // -------- Weather --------
    void getTemperature() {
        float temperature = this->weather->getTemperature();
        server.send(200, "text/plain", String(temperature).c_str());
    }

    void getHumidity() {
        float humidity = this->weather->getHumidity();
        server.send(200, "text/plain", String(humidity).c_str());
    }

    // -------- Middleware --------
    void handleNotFound() {
        server.send(404, "text/plain", "Not found");
    }

    // TODO: add development status from ENV
    void stats() {
        server.send(200, "text/json", "{ \"status\": \"OK\", \"uptime\": \"" + String(millis()) + "\" }");
    }

    // service
    void configureMiddleware() {
        server.addHook([](const String& method, const String& url, WiFiClient* client, ESP8266WebServer::ContentTypeFunction contentType) {
            (void)method;       // GET, PUT, ...
            (void)url;          // example: /root/myfile.html
            (void)client;       // the webserver tcp client connection
            // (void)contentType;  // contentType(".html") => "text/html"
            if (Serial) {
                Serial.printf(">[%s] %s - from IP: %s \n", method.c_str(), url.c_str(), client->remoteIP().toString().c_str());
                Serial.println(" time stamp: " + String(millis()));
            }
            return ESP8266WebServer::CLIENT_REQUEST_CAN_CONTINUE;
            });
    }
};

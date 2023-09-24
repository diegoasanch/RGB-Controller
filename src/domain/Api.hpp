#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <uri/UriRegex.h>
#include <uri/UriBraces.h>
#include <ArduinoJson.h>

#include "config.h"
#include "../env.h"

#include "../components/RGBLed.hpp"
#include "./Settings.hpp"
#include "./Update.hpp"
#include "./Weather.hpp"

class Api {
public:
    Api(int port,
        RGBLed& rgb,
        Weather& weather,
        UpdateClient& updateClient,
        Settings& settings
    ) :
        server(port),
        rgb(rgb),
        weather(weather),
        updateClient(updateClient),
        settings(settings) {

        this->port = port;
    }

    void configure() {
        Serial.println("Configuring API");

        this->configureRoutes();
        Serial.println("Configured Routes");
        this->configureMiddleware();
        Serial.println("Configured Middleware");

        server.begin();
        Serial.println("Started server");

        this->configureMDNS();
    }

    void handleRequests() {
        server.handleClient();
        MDNS.update();
    }

private:
    ESP8266WebServer server;
    int port;
    RGBLed& rgb;
    Weather& weather;
    UpdateClient& updateClient;
    Settings& settings;

    void configureMDNS() {
        if (MDNS.begin(env::MDNS_SERVER_NAME))
            Serial.println("Started MDNS at: " + String(env::MDNS_SERVER_NAME) + ".local");
        else
            Serial.println("Error starting MDNS");
    }

    void configureRoutes() {
        server.on("/", [this]() {this->handleRoot();});
        // Light
        // -- Toggle
        server.on("/light", [this]() {this->handleLightStatus();});
        server.on("/light/on", [this]() {this->turnOn();});
        server.on("/light/off", [this]() {this->turnOff();});
        server.on("/light/toggle", [this]() {this->toggle();});
        server.on("/light/onAnimated", [this]() {this->turnOnAnimated();});
        server.on("/light/offAnimated", [this]() {this->turnOffAnimated();});
        server.on("/light/toggleAnimated", [this]() {this->toggleAnimated();});
        // -- Brightness
        server.on("/light/brightness", [this]() {this->getBrightness();});
        server.on(UriRegex("^\\/light\\/brightness\\/([0-9]+)$"), [this]() {this->setBrightness();});
        server.on(UriRegex("^\\/light\\/brightnessAnimated\\/([0-9]+)$"), [this]() {this->setBrightnessAnimated();});
        // -- Color
        server.on("/light/hexColor", [this]() {this->getHexColor();});
        server.on(UriRegex("^\\/light\\/hexColor\\/([0-9a-fA-F]{6})$"), [this]() {this->setHexColor();});
        // -- Refresh rate
        server.on("/light/refreshRateHz", [this]() {this->getRefreshRate();});
        server.on(UriRegex("^\\/light\\/refreshRateHz\\/([0-9]+)$"), [this]() {this->setRefreshRate();});

        // Weather
        server.on("/weather/temperature", [this]() {this->getTemperature();});
        server.on("/weather/humidity", [this]() {this->getHumidity();});

        // -- Settings
        // Update
        // Path args: updateServerURL, port, updatePath
        server.on(UriBraces("/settings/version/update/{}/{}/{}"), [this]() {this->handleUpdateVersion();});
        server.on("/settings/version", [this]() {this->getVersion();});
        server.on("/settings/reboot", [this]() {this->handleReboot();});

        // Utils
        server.on("/stats", [this]() {this->stats();});
    }

    void handleRoot() {
        server.send(200, "text/plain", "Hello world.");
    }

    // -------- Light --------
    void handleLightStatus() {
        server.send(200, "text/plain", this->rgb.getIsOn() ? "1" : "0");
    }
    void getBrightness() {
        String brightness = String(this->rgb.getBrightness());
        server.send(200, "text/plain", brightness);
    }

    void setBrightness() {
        String brightness = server.pathArg(0);
        this->rgb.setTargetBrightness((uint8)brightness.toInt());
        server.send(200, "text/plain", brightness);
    }

    void setBrightnessAnimated() {
        String brightness = server.pathArg(0);
        this->rgb.setBrightnessAnimated((uint8)brightness.toInt());
        server.send(200, "text/plain", brightness);
    }

    void setHexColor() {
        String hexColor = server.pathArg(0);
        this->rgb.setHexColor(hexColor);
        server.send(200, "text/plain", hexColor);
    }

    void getHexColor() {
        String color = this->rgb.getHexColor();
        server.send(200, "text/plain", color);
    }

    // TODO: Give "light-x" naming
    void turnOn() {
        this->rgb.turnOn();
        server.send(200, "text/plain", "1");
    }

    void turnOff() {
        this->rgb.turnOff();
        server.send(200, "text/plain", "0");
    }

    void toggle() {
        bool status = this->rgb.toggle();
        server.send(200, "text/plain", status ? "1" : "0");
    }
    void turnOnAnimated() {
        this->rgb.turnOnAnimated();
        server.send(200, "text/plain", "1");
    }

    void turnOffAnimated() {
        this->rgb.turnOffAnimated();
        server.send(200, "text/plain", "0");
    }

    void toggleAnimated() {
        bool status = this->rgb.toggleAnimated();
        server.send(200, "text/plain", status ? "1" : "0");
    }

    // -------- Weather --------
    void getTemperature() {
        float temperature = this->weather.getTemperature();
        String json = "";
        StaticJsonDocument<200> parser;
        parser["temperature"] = temperature;
        serializeJson(parser, json);
        server.send(200, "text/plain", json);
    }

    void getHumidity() {
        float humidity = this->weather.getHumidity();
        String json = "";
        StaticJsonDocument<200> parser;
        parser["humidity"] = humidity;
        serializeJson(parser, json);
        server.send(200, "text/plain", json);
    }

    // -------- Settings --------

    void handleUpdateVersion() {
        String updateServerUrl = server.pathArg(0);
        String port = server.pathArg(1);
        String updatePath = server.pathArg(2);
        server.send(200, "text/plain", "starting update");
        this->updateClient.install(updateServerUrl, port.toInt(), updatePath);
    }

    void getVersion() {
        String version = this->updateClient.version();
        server.send(200, "text/plain", version);
    }

    void handleReboot() {
        server.send(200, "text/plain", "rebooting");
        this->settings.reboot();
    }

    // -------- Middleware --------
    void handleNotFound() {
        server.send(404, "text/plain", "Not found");
    }

    // TODO: add development status based ENV
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
                Serial.printf("\n>[%s] %s - from IP: %s \n", method.c_str(), url.c_str(), client->remoteIP().toString().c_str());
                Serial.println("-time stamp: " + String(millis()));
            }
            return ESP8266WebServer::CLIENT_REQUEST_CAN_CONTINUE;
            });
    }

    void setRefreshRate() {
        String refreshRate = server.pathArg(0);
        this->rgb.setRefreshRateHz(refreshRate.toInt());
        server.send(200, "text/plain", String(this->rgb.getRefreshRateHz()));
    }

    void getRefreshRate() {
        String refreshRate = String(this->rgb.getRefreshRateHz());
        server.send(200, "text/plain", refreshRate);
    }
};

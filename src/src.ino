#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
// #include <ESP8266mDNS.h>

// #include <Wire.h>
// #include <ESP8266HTTPClient.h>

// #include <uri/UriBraces.h>
// #include <uri/UriRegex.h>


#include "env.h"
#include "config.h"
#include "Screen.hpp"
#include "Weather.hpp"
#include "Led.hpp"
#include "RGBLed.hpp"
#include "Api.hpp"

Weather weatherSensor(refreshRate::WEATHER, pins::WEATHER_DATA);

// ESP8266WebServer server(80);


// Led led(LED_BUILTIN, refreshRate::LED);
RGBLed rgb(pins::RGB_RED, pins::RGB_GREEN, pins::RGB_BLUE, true, refreshRate::LED);
Api api(80, &rgb);

float temperature = 0;
float humidity = 0;
int brightness = 0;
String ipAddr = "";

uint8_t potPin = A0;


//Static IP address configuration
IPAddress staticIP(192, 168, 0, 127); //ESP static ip
IPAddress gateway(192, 168, 0, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(8, 8, 8, 8);  //DNS

const char* deviceName = "deviceName.com";

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    startup();
    setupWifi();
    api.configure();

    // setupServer();
    // if (MDNS.begin("esp8266")) {
    //     Serial.println("MDNS responder started");
    // }


    delay(3000);

}

// void setupServer() {
//     Serial.println("Setting up server");
//     server.on("/", handleRoot);
//     server.on(UriRegex("^\\/params\\/([0-9a-fA-F]{6})$"), handleUrlPathParam);
//     server.on(UriRegex("^\\/brightness\\/set\\/([0-9]+)$"), handleBrightness);
//     server.begin();
//     Serial.println("Server configured");
// }

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
    // led.setBrightness(MAX_BRIGHTNESS);
    // rgb.setBrightness(MAX_BRIGHTNESS);
    ipAddr = WiFi.localIP().toString();
    delay(1000);
}


void loop() {
    // led.updateLed();
    rgb.update();
    api.handleRequests();
    // server.handleClient();
    // MDNS.update();

    if (weatherSensor.shouldUpdate()) {
        temperature = weatherSensor.getTemperature();
        humidity = weatherSensor.getHumidity();
    }
}

void startup() {
    Serial.println("Booting up...");
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    // led.turnOff();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
}

// void handleRoot() {
//     if (led.getBrightness() < 1000)
//         led.setBrightness(1023);
//     else
//         led.setBrightness(1);

//     server.send(200, "text/plain", "hello from esp8266!\r\n");
// }

// void handleUrlPathParam() {
//     String param1 = server.pathArg(0);

//     Serial.printf("param1: %s\n", param1.c_str());
//     server.send(200, "text/plain", "param1: " + param1 + "\r\n");
// }

// void handleBrightness() {
//     String param1 = server.pathArg(0);
//     int brightness = param1.toInt();
//     led.setBrightness(brightness);
//     rgb.setBrightness(brightness);
//     server.send(200, "text/plain", "brightness: " + param1 + "\r\n");
// }

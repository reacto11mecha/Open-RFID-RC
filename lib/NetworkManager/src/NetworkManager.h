#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <MachineCore.h>
#include <WiFi.h>
#include <definitions.h>

class NetworkManager
{
public:
    bool clicked;
    String header;
    WiFiServer server;

    NetworkManager();

    void begin();
    void renderWebServer(WiFiClient *client, bool isOpenRequest);

private:
};

#endif
#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <MachineCore.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

#include <definitions.h>

class NetworkManager
{
public:
    NetworkManager();

    void begin(MachineState &ms, MachineCore &mc, SemaphoreHandle_t &sm, QueueHandle_t &rq, uint32_t &standbyCount);
    void loopWebServer();

private:
};

#endif
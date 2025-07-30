#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <Arduino.h>

#define SOLENOID_PIN 25
#define BUZZER_PIN 14
#define TOUCH_PIN 26

// Buat nandain kondisi alat dari luar
enum MachineState
{
    STANDBY_MODE,
    FINDING_DATA,
    FOUND_STATE,
    NOT_FOUND_STATE
};

// Perintah OLED ngerender sesuatu
enum RenderCommand
{
    CMD_DRAW_STANDBY_SCAN,
    CMD_DRAW_STANDBY_LOGO,
    CMD_DRAW_FINDING,
    CMD_DRAW_FOUND,
    CMD_DRAW_NOT_FOUND,
    CMD_DRAW_OPEN_BY_NON_CARD_READ
};

#endif

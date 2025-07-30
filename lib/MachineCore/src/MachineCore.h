#ifndef MACHINE_CORE_H
#define MACHINE_CORE_H

#include <Arduino.h>
#include <definitions.h>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>

#define UID_LENGTH 4
#define TOTAL_CARDS 3

bool checkUIDMatch(byte *readUID, byte readLength);

class MachineCore
{
public:
    MachineCore();

    void begin();
    bool detectCard();
    bool cardFound();

    void unlockDoor();
    void lockDoor();

    void buzzerShort();
    void buzzerLong();

private:
    PN532 _nfc;
};

#endif
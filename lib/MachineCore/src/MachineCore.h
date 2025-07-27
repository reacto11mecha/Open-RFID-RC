#ifndef MACHINE_CORE_H
#define MACHINE_CORE_H

#include <Arduino.h>
#include <definitions.h>
#include <SPI.h>
#include <MFRC522.h>

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
    void haltRead();

    void unlockDoor();
    void lockDoor();

    void buzzerShort();
    void buzzerLong();

private:
    MFRC522 _mfrc522;
};

#endif
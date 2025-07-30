#include <MachineCore.h>

boolean success;
uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
uint8_t uidLength;

PN532_I2C pn532i2c(Wire);

/**
 * https://github.com/reacto11mecha/kel14-attendance-machine/blob/a08a048ceae5cc06c3949578c1860a6c52d3a5c9/kel14-attendance-machine.ino#L48-L51
 */
byte allowedUID[TOTAL_CARDS][UID_LENGTH] = {
    {0x93, 0x95, 0xFB, 0xEE},
    {0x83, 0x2C, 0x5D, 0xA6},
    {0xD0, 0xC0, 0x27, 0x93}};

bool checkUIDMatch(byte *readUID, byte readLength)
{
    for (byte i = 0; i < TOTAL_CARDS; i++)
    {
        bool match = true;

        for (byte j = 0; j < UID_LENGTH; j++)
        {
            if (readUID[j] != allowedUID[i][j])
            {
                match = false;

                break;
            }
        }

        // Jika ditemukan UID yang cocok
        if (match)
            return true;
    }

    // Tidak ada UID yang cocok
    return false;
}

MachineCore::MachineCore()
    : _nfc(pn532i2c) {}

void MachineCore::begin()
{
    pinMode(SOLENOID_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    _nfc.begin();

    uint32_t versiondata = _nfc.getFirmwareVersion();
    if (!versiondata)
    {
        Serial.print("Didn't find PN53x board");
        while (1)
            ; // halt
    }

    Serial.print("Found chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);
    Serial.print("Firmware ver. ");
    Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print('.');
    Serial.println((versiondata >> 8) & 0xFF, DEC);

    _nfc.setPassiveActivationRetries(0xFF);

    _nfc.SAMConfig();

    Serial.println("PN532 detected!");
}

bool MachineCore::detectCard()
{
    return _nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
}

bool MachineCore::cardFound()
{
    return checkUIDMatch(&uid[0], uidLength);
}

void MachineCore::unlockDoor()
{
    digitalWrite(SOLENOID_PIN, HIGH);
}

void MachineCore::lockDoor()
{
    digitalWrite(SOLENOID_PIN, LOW);
}

void MachineCore::buzzerShort()
{
    digitalWrite(BUZZER_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));

    digitalWrite(BUZZER_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(50));

    digitalWrite(BUZZER_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));

    digitalWrite(BUZZER_PIN, LOW);
}

void MachineCore::buzzerLong()
{
    digitalWrite(BUZZER_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(700));

    digitalWrite(BUZZER_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(500));
}
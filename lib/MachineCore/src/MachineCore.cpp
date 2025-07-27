#include <MachineCore.h>

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
    : _mfrc522(SS_PIN, RST_PIN) {}

void MachineCore::begin()
{
    pinMode(SOLENOID_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    SPI.begin();
    _mfrc522.PCD_Init();
}

bool MachineCore::detectCard()
{
    if (!_mfrc522.PICC_IsNewCardPresent())
        return false;

    if (!_mfrc522.PICC_ReadCardSerial())
        return false;

    // Serial.print("UID Kartu: ");
    // for (byte i = 0; i < _mfrc522.uid.size; i++)
    // {
    //     Serial.print(_mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    //     Serial.print(_mfrc522.uid.uidByte[i], HEX);
    //     Serial.print(" ");
    // }
    // Serial.println();

    return true;
}

bool MachineCore::cardFound()
{
    return checkUIDMatch(_mfrc522.uid.uidByte, _mfrc522.uid.size);
}

void MachineCore::haltRead()
{
    _mfrc522.PICC_HaltA();
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
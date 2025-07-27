#include <Arduino.h>
#include <UserInterface.h>
#include <NetworkManager.h>
#include <MachineCore.h>
#include <definitions.h>

void scanIDCardTask(void *pvParams);
void readTouchSensor(void *pvParams);
void renderTask(void *pvParams);
void webServerTask(void *pvParams);
void resetStandbyModeCount();

TaskHandle_t scanIdCardHandle;
TaskHandle_t renderTaskHandle;
TaskHandle_t readTouchHandle;
TaskHandle_t webServerHandle;

QueueHandle_t renderQueue;

SemaphoreHandle_t stateMutex;

NetworkManager nm;
MachineCore mc;
UInterface oled;

MachineState currentState = STANDBY_MODE;

uint32_t standbyModeStart;

void setup()
{
  Serial.begin(115200);

  oled.begin();

  stateMutex = xSemaphoreCreateMutex();
  renderQueue = xQueueCreate(10, sizeof(RenderCommand));

  pinMode(TOUCH_PIN, INPUT);

  mc.begin();

  mc.unlockDoor();
  vTaskDelay(pdMS_TO_TICKS(250));
  mc.lockDoor();

  nm.begin();

  xTaskCreatePinnedToCore(renderTask, "Render Task", 10000, NULL, 3, &renderTaskHandle, 0);
  xTaskCreatePinnedToCore(webServerTask, "Web Server Task", 10000, NULL, 1, &webServerHandle, 0);
  xTaskCreatePinnedToCore(scanIDCardTask, "Scan ID Card Task", 10000, NULL, 2, &scanIdCardHandle, 0);
  xTaskCreatePinnedToCore(readTouchSensor, "Read Touch Sensor Task", 15000, NULL, 1, &readTouchHandle, 1);
}

void renderTask(void *pvParams)
{
  RenderCommand cmd;
  uint8_t lastCmd = 31;

  while (1)
  {
    if (xQueueReceive(renderQueue, &cmd, portMAX_DELAY))
    {
      if (cmd == lastCmd)
      {
        vTaskDelay(pdMS_TO_TICKS(10));
        continue;
      }

      lastCmd = cmd;

      switch (cmd)
      {
      case CMD_DRAW_STANDBY_SCAN:
      {
        oled.drawStandbyText();
        break;
      }

      case CMD_DRAW_STANDBY_LOGO:
      {
        oled.drawStandbyLogo();
        break;
      }

      case CMD_DRAW_FINDING:
      {
        oled.drawFindingData();
        break;
      }

      case CMD_DRAW_FOUND:
      {
        oled.drawFoundState();
        break;
      }

      case CMD_DRAW_NOT_FOUND:
      {
        oled.drawNotFoundState();
        break;
      }

      case CMD_DRAW_OPEN_BY_NON_CARD_READ:
      {
        oled.drawOpenByNonCardRead();
        break;
      }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void scanIDCardTask(void *pvParams)
{
  const uint32_t cyclePeriod = 10000;
  resetStandbyModeCount();

  while (1)
  {
    MachineState localState;

    if (xSemaphoreTake(stateMutex, portMAX_DELAY) == pdTRUE)
    {
      localState = currentState;
      xSemaphoreGive(stateMutex);
    }

    RenderCommand renderCmd;

    switch (localState)
    {
    case STANDBY_MODE:
    {
      uint32_t elapsed = millis() - standbyModeStart;
      uint32_t cyclePosition = elapsed % cyclePeriod;

      bool renderText = (cyclePosition < 5000);

      renderCmd = renderText ? CMD_DRAW_STANDBY_SCAN : CMD_DRAW_STANDBY_LOGO;
      xQueueSend(renderQueue, &renderCmd, portMAX_DELAY);

      if (mc.detectCard())
      {
        xSemaphoreTake(stateMutex, portMAX_DELAY);
        currentState = FINDING_DATA;
        xSemaphoreGive(stateMutex);
      }
      break;
    }

    case FINDING_DATA:
    {
      renderCmd = CMD_DRAW_FINDING;
      xQueueSend(renderQueue, &renderCmd, portMAX_DELAY);

      bool found = mc.cardFound();
      mc.haltRead();

      xSemaphoreTake(stateMutex, portMAX_DELAY);
      currentState = found ? FOUND_STATE : NOT_FOUND_STATE;
      xSemaphoreGive(stateMutex);

      break;
    }

    case FOUND_STATE:
    {
      renderCmd = CMD_DRAW_FOUND;
      xQueueSend(renderQueue, &renderCmd, portMAX_DELAY);

      mc.unlockDoor();
      mc.buzzerShort();

      uint32_t start = millis();
      while (millis() - start < 4500)
      {
        vTaskDelay(pdMS_TO_TICKS(100));
      }

      mc.lockDoor();

      xSemaphoreTake(stateMutex, portMAX_DELAY);
      resetStandbyModeCount();
      currentState = STANDBY_MODE;
      xSemaphoreGive(stateMutex);

      break;
    }

    case NOT_FOUND_STATE:
    {
      renderCmd = CMD_DRAW_NOT_FOUND;
      xQueueSend(renderQueue, &renderCmd, portMAX_DELAY);

      mc.lockDoor();
      mc.buzzerLong();

      uint32_t start = millis();
      while (millis() - start < 4500)
      {
        vTaskDelay(pdMS_TO_TICKS(100));
      }

      xSemaphoreTake(stateMutex, portMAX_DELAY);
      resetStandbyModeCount();
      currentState = STANDBY_MODE;
      xSemaphoreGive(stateMutex);

      break;
    }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void readTouchSensor(void *pvParams)
{
  while (1)
  {
    if (digitalRead(TOUCH_PIN))
    {
      if (xSemaphoreTake(stateMutex, portMAX_DELAY) == pdTRUE)
      {
        mc.haltRead();
        mc.unlockDoor();
        mc.buzzerShort();

        RenderCommand renderCmd = CMD_DRAW_OPEN_BY_NON_CARD_READ;
        xQueueSend(renderQueue, &renderCmd, portMAX_DELAY);

        uint32_t unlockStart = millis();

        while (millis() - unlockStart < 4500)
        {
          vTaskDelay(pdMS_TO_TICKS(100));
        }

        mc.lockDoor();
        resetStandbyModeCount();
        currentState = STANDBY_MODE;
        xSemaphoreGive(stateMutex);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void webServerTask(void *pvParams)
{
  while (1)
  {
    WiFiClient client = nm.server.available();

    if (client)
    {
      String currentLine = "";
      while (client.connected())
      {
        if (client.available())
        {
          char c = client.read();
          Serial.write(c);
          nm.header += c;

          if (c == '\n')
          {
            if (currentLine.length() == 0)
            {
              bool isOpenRequest = (nm.header.indexOf("GET /open") >= 0);

              // Kirim respons HTTP sekarang juga
              nm.renderWebServer(&client, isOpenRequest);

              if (isOpenRequest && !nm.clicked)
              {
                nm.clicked = true;

                // Hentikan pemrosesan client lebih lanjut
                nm.header = "";
                client.stop();
                Serial.println("Client disconnected after open request");

                // Operasi buka pintu dilakukan DI LUAR loop client
                if (xSemaphoreTake(stateMutex, portMAX_DELAY) == pdTRUE)
                {
                  mc.unlockDoor();
                  mc.buzzerShort();

                  RenderCommand renderCmd = CMD_DRAW_OPEN_BY_NON_CARD_READ;
                  xQueueSend(renderQueue, &renderCmd, portMAX_DELAY);

                  uint32_t unlockStart = millis();

                  while (millis() - unlockStart < 4500)
                  {
                    vTaskDelay(pdMS_TO_TICKS(100));
                  }

                  mc.lockDoor();
                  resetStandbyModeCount();
                  currentState = STANDBY_MODE;
                  xSemaphoreGive(stateMutex);

                  nm.clicked = false;
                }

                break; // Keluar dari loop client
              }
              else
              {
                // Hanya request biasa, lanjutkan normal
                break;
              }
            }
            else
            {
              currentLine = "";
            }
          }
          else if (c != '\r')
          {
            currentLine += c;
          }
        }
      }

      nm.header = "";
      client.stop();
      Serial.println("Client disconnected.");
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void resetStandbyModeCount()
{
  standbyModeStart = millis();
}

void loop()
{
  delay(100);
}

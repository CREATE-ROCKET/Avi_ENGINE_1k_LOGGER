#include <Arduino.h>
#include <SPICREATE.h>
#include <mcp3208.h>

#include "FS.h"
#include "SD_MMC.h"

#define SCK1 33
#define MISO1 25
#define MOSI1 26
#define MCPCS1 27
#define MCPCS2 32
#define LED 19
#define loggingPeriod 1

SPICREATE::SPICreate SPIC1;
MCP mcp3208_0;
MCP mcp3208_1;

bool isSDinitted = false;
File logfile;
xTaskHandle logHandle;
xTaskHandle writeSDHandle;
xTaskHandle initSDHandle;
uint16_t logCounter = 0;

QueueHandle_t xQueue;

void initSD(void *parameters)
{
  // sdio init
  if (!SD_MMC.begin())
  {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE)
  {
    Serial.println("No SD_MMC card attached");
    return;
  }
  Serial.print("SD_MMC Card Type: ");
  if (cardType == CARD_MMC)
  {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  }
  else
  {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
  Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));

  vTaskDelete(initSDHandle);
}

void writeHeader(void *parameters)
{
  Serial.println("Writing file: /hello.txt");

  logfile = SD_MMC.open("/hello.txt", FILE_APPEND);
  if (!logfile)
  {
    Serial.println("filemnterror");
  }
  // write header
  if (logfile.println("data"))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
  }

  logfile.close();

  vTaskDelete(initSDHandle);
}

IRAM_ATTR void logging(void *parameters)
{
  portTickType xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    uint32_t startTime = micros();
    uint16_t adcData[16];
    char bfChar[128] = "";

    for (int i = 0; i < 8; i++)
    {
      adcData[i] = mcp3208_0.Get(i);
      adcData[8 + i] = mcp3208_1.Get(i);
    }

    sprintf(bfChar, "%d", startTime);
    for (int i = 0; i < 16; i++)
    {
      sprintf(bfChar, "%s,%d", bfChar, adcData[i]);
    }
    sprintf(bfChar, "%s\n", bfChar);

    if (uxQueueSpacesAvailable(xQueue) > 0)
    {
      // send queue
      xQueueSend(xQueue, &bfChar, 0);
    }
    else
    {
      // stop logging
      Serial.println("queue filled!!!");
      vTaskDelete(writeSDHandle);
      vTaskDelete(logHandle);
    }

    vTaskDelayUntil(&xLastWakeTime, loggingPeriod / portTICK_PERIOD_MS);
  }
}

IRAM_ATTR void writeSD(void *parameters)
{
  for (;;)
  {
    char data[128];
    if (xQueueReceive(xQueue, &data, 0) == pdTRUE)
    {
      logfile.print(data);
      logCounter++;
      if (logCounter == 1024)
      {
        logCounter = 0;
        logfile.close();
        logfile = SD_MMC.open("/hello.txt", FILE_APPEND);
      }
    }
    else
    {
      delay(1);
    }
  }
}

void setup()
{
  // io init
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  mcp3208_0.begin(&SPIC1, MCPCS1, 10000000);
  mcp3208_1.begin(&SPIC1, MCPCS2, 10000000);

  xQueue = xQueueCreate(128, 128 * sizeof(char));
}

void loop()
{
  // put your main code here, to run repeatedly:
  while (Serial.available())
  {
    char cmd = Serial.read();
    // 'i' cmd init sd card
    if (cmd == 'i')
    {
      if (isSDinitted == false)
      {
        xTaskCreatePinnedToCore(initSD, "initSD", 4096, NULL, 1, &initSDHandle, APP_CPU_NUM);
        delay(1000);
        xTaskCreatePinnedToCore(writeHeader, "writeHeader", 4096, NULL, 1, &initSDHandle, APP_CPU_NUM);
        isSDinitted = true;
      }
    }
    // 'a' cmd logging start
    if (cmd == 'a')
    {
      logfile = SD_MMC.open("/hello.txt", FILE_APPEND);
      if (!logfile)
      {
        Serial.println("filemnterror");
      }

      xTaskCreatePinnedToCore(logging, "logging", 4096, NULL, 1, &logHandle, PRO_CPU_NUM);
      xTaskCreatePinnedToCore(writeSD, "writeSD", 65536, NULL, 1, &writeSDHandle, APP_CPU_NUM);
      Serial.println("logging start");
    }
    if (cmd == 's')
    {
      vTaskDelete(logHandle);
      vTaskDelete(writeSDHandle);
      logfile.close();
      Serial.println("logging stop");
    }
    if (cmd == 'e')
    {
      SD_MMC.remove("/hello.txt");
      Serial.println("remove file");
      isSDinitted = false;
    }
    if (cmd == 'r')
    {
      ESP.restart();
    }
  }
  delay(1000);
}
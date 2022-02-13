#include <Arduino.h>
#include <SPICREATE.h>
#include <MCP3208.h>

#include "FS.h"
#include "SD_MMC.h"

#define SCK1 33
#define MISO1 25
#define MOSI1 26
#define MCPCS 27
#define LED 19
#define loggingPeriod 1

SPICREATE::SPICreate SPIC1;
MCP mcp3208;

File logfile;
xTaskHandle logHandle;

void writeHeader(void)
{
  Serial.println("Writing file: /hello.txt");

  logfile = SD_MMC.open("/hello.txt", FILE_WRITE);
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
}

IRAM_ATTR void logging(void *parameters)
{
  portTickType xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    logfile.print(micros());
    logfile.print(",");
    for (int i = 0; i < 8; i++)
    {
      logfile.print(mcp3208.Get(i));
      logfile.print(",");
    }
    logfile.println();
    vTaskDelayUntil(&xLastWakeTime, loggingPeriod / portTICK_PERIOD_MS);
  }
}

void setup()
{
  // io init
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  mcp3208.begin(&SPIC1, MCPCS, 1000000);

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
}

void loop()
{
  // put your main code here, to run repeatedly:
  while (Serial.available())
  {
    char cmd = Serial.read();
    if (cmd == 'w')
    {
      writeHeader();
    }
    if (cmd == 'a')
    {
      logfile = SD_MMC.open("/hello.txt", FILE_APPEND);
      if (!logfile)
      {
        Serial.println("filemnterror");
      }

      xTaskCreate(logging, "logging", 65536, NULL, 1, &logHandle);
      Serial.println("logging start");
    }
    if (cmd == 's')
    {
      vTaskDelete(logHandle);
      logfile.close();
      Serial.println("logging stop");
    }
    if (cmd == 'e')
    {
      SD_MMC.remove("/hello.txt");
      Serial.println("remove file");
    }
    if (cmd == 'r')
    {
      ESP.restart();
    }
  }
  delay(1000);
}
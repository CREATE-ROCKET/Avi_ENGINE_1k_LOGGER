#include <Arduino.h>
#include <SPICREATE.h>
#include <mcp3208.h>
#include <SDIOLogWrapper.h>

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

SDIOLogWrapper SDIO;

xTaskHandle xlogHandle;

IRAM_ATTR void logging(void *parameters)
{
  portTickType xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    uint16_t adcData[16];
    char bfChar[128] = "";
    uint8_t xQueueWaitCount = SDIO.countWaitingQueue();

    uint32_t startTime = micros();

    for (int i = 0; i < 8; i++)
    {
      adcData[i] = mcp3208_0.Get(i);
      adcData[8 + i] = mcp3208_1.Get(i);
    }

    uint32_t stopTime = micros();

    sprintf(bfChar, "%d,%d,%d,", xQueueWaitCount, startTime, stopTime);
    for (int i = 0; i < 16; i++)
    {
      sprintf(bfChar, "%s%d,", bfChar, adcData[i]);
    }

    sprintf(bfChar, "%s\n", bfChar);

    if (SDIO.appendQueue(bfChar) == 1)
    {
      vTaskDelete(&xlogHandle);
      Serial.println("queue filled!");
      ESP.restart();
    }

    vTaskDelayUntil(&xLastWakeTime, loggingPeriod / portTICK_PERIOD_MS);
  }
}

void setup()
{
  // io init
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  mcp3208_0.begin(&SPIC1, MCPCS1, 6000000);
  mcp3208_1.begin(&SPIC1, MCPCS2, 6000000);

  SDIO.makeQueue(64, 128);
}

void loop()
{
  // put your main code here, to run repeatedly:
  while (Serial.available())
  {
    char cmd = Serial.read();

    if (cmd == 'r')
    {
      ESP.restart();
    }

    if (cmd == 'a')
    {
      Serial.printf("SD init result: %d\n", SDIO.initSD());
      SDIO.openFile();
      SDIO.writeTaskCreate(APP_CPU_NUM);
      xTaskCreateUniversal(logging, "logging", 8192, NULL, 1, &xlogHandle, PRO_CPU_NUM);
    }

    if (cmd == 'g')
    {
      vTaskDelete(xlogHandle);
      delay(1000);
      SDIO.writeTaskDelete();
      SDIO.closeFile();
      SDIO.deinitSD();
    }
  }
  delay(100);
}
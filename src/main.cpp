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
QueueHandle_t xQueue;

xTaskHandle xlogHandle;

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

    if (SDIO.appendQueue(bfChar) == 1)
    {
      vTaskDelete(&xlogHandle);
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
  mcp3208_0.begin(&SPIC1, MCPCS1, 10000000);
  mcp3208_1.begin(&SPIC1, MCPCS2, 10000000);
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
      SDIO.makeQueue(128);

      Serial.printf("SD init result: %d\n", SDIO.initSD());
      SDIO.openFile();
      SDIO.writeTaskCreate(APP_CPU_NUM);
      xTaskCreateUniversal(logging, "logging", 8192, NULL, 1, &xlogHandle, PRO_CPU_NUM);
    }

    if (cmd == 'g')
    {
      vTaskDelete(xlogHandle);
      Serial.printf("logtask killed\n");
      delay(1000);
      SDIO.writeTaskDelete();
      Serial.printf("writetask killed\n");
      SDIO.closeFile();
      Serial.printf("file closed\n");
      SDIO.deinitSD();
      Serial.printf("sd deinitted\n");
    }
  }
  delay(10);
}
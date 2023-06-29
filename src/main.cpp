#include <Arduino.h>
#include <SPICREATE.h>
#include <mcp3208.h>
#include <SDIOLogWrapper.hpp>
#include <SDIOLogWrapper.hpp>

#define SCK1 33
#define MISO1 25
#define MOSI1 26
#define MCPCS1 32
#define MCPCS2 27
#define LED_ERR 19
#define VALVE_BRD_STAT 18
#define LOGGINGINTERVAL 1

#define QUEUESTRINGSIZE 128

SPICREATE::SPICreate SPIC1;
MCP mcp3208_0;
MCP mcp3208_1;

xTaskHandle xlogHandle;
uint8_t isLoggintGoing = 0;

IRAM_ATTR void logging(void *parameters)
{
  portTickType xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    uint16_t adcData[16];
    char *bfChar = new char[QUEUESTRINGSIZE];
    if (bfChar != NULL)
    {
      bfChar[0] = '\0';
      uint16_t xQueueWaitCount = SDIOLogWrapper::countWaitingQueue();

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
      sprintf(bfChar, "%s%d\n", bfChar, digitalRead(VALVE_BRD_STAT));

      if (SDIOLogWrapper::appendQueue(bfChar) == 1)
      {
        delete[] bfChar;
      }
    }

    vTaskDelayUntil(&xLastWakeTime, LOGGINGINTERVAL / portTICK_PERIOD_MS);
  }
}

void setup()
{
  // io init
  Serial.begin(115200);
  pinMode(LED_ERR, OUTPUT);
  pinMode(21, INPUT_PULLDOWN);
  pinMode(VALVE_BRD_STAT, INPUT_PULLUP);
  digitalWrite(LED_ERR, HIGH);
  SPIC1.begin(VSPI, SCK1, MISO1, MOSI1);
  mcp3208_0.begin(&SPIC1, MCPCS1, 6000000);
  mcp3208_1.begin(&SPIC1, MCPCS2, 6000000);

  SDIOLogWrapper::initfileNum();
}

void loop()
{
  // put your main code here, to run repeatedly:

  if ((digitalRead(21) == HIGH) && (isLoggintGoing == 0))
  {
    digitalWrite(LED_ERR, LOW);
    isLoggintGoing = 1;
    SDIOLogWrapper::makeQueue(1024);
    SDIOLogWrapper::setSaveInterval(1024);

    Serial.printf("SD init result: %d\r\n", SDIOLogWrapper::initSD());
    SDIOLogWrapper::openFile();
    SDIOLogWrapper::writeTaskCreate(APP_CPU_NUM);
    xTaskCreateUniversal(logging, "logging", 8192, NULL, configMAX_PRIORITIES, &xlogHandle, PRO_CPU_NUM);
  }

  delay(1000);

  if ((digitalRead(21) == LOW) && (isLoggintGoing == 1))
  {
    digitalWrite(LED_ERR, HIGH);
    vTaskDelete(xlogHandle);
    delay(1000);
    SDIOLogWrapper::writeTaskDelete();
    Serial.println("logTask killed");
    SDIOLogWrapper::deleteQueue();
    SDIOLogWrapper::closeFile();
    SDIOLogWrapper::deinitSD();
    isLoggintGoing = 0;
  }
  delay(1000);
}
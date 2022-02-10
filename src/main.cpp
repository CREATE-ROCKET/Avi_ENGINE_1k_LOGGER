#include <Arduino.h>

#include "FS.h"
#include "SD_MMC.h"

#define LED 19

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

void setup()
{
  // io init
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

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
      int startTime = micros();
      for (int i = 0; i < 65536; i++)
      {
        if (logfile.println("256"))
        {
        }
        else
        {
          Serial.print("Write failed");
        }
      }
      logfile.close();
      Serial.printf("it took %i second\n", micros() - startTime);
    }
    if (cmd == 'e')
    {
      SD_MMC.remove("/hello.txt");
      Serial.println("remove file");
    }
  }
}
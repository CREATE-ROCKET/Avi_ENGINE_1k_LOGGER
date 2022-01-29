#include <Arduino.h>

#include "FS.h"
#include "SD_MMC.h"

void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
  }
}

int appendFile(fs::FS &fs, const char *path, const char *message)
{
  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    return 1;
  }
  if (file.print(message))
  {
    return 0;
  }
  else
  {
    return 2;
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);
  for (int i = 0; i < 10; i++)
  {
    delay(1000);
    Serial.print(i);
  }
  digitalWrite(19, LOW);

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
  ;
  writeFile(SD_MMC, "/hello.txt", "Hello ");
  int ans = appendFile(SD_MMC, "/hello.txt", "World!\n");
  Serial.print("append file status: ");
  Serial.println(ans);
  Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));
}

void loop()
{
  // put your main code here, to run repeatedly:
  while (Serial.available())
  {
    Serial.read();
    uint32_t endtime = 0;

    File file = SD_MMC.open("/hello.txt", FILE_APPEND);
    if (!file)
    {
      Serial.println("filemnterror");
    }

    uint32_t starttime = micros();
    file.print(117);

    endtime = micros();
    Serial.print("it takes ");
    Serial.print(endtime - starttime);
    Serial.println("us");
  }
}
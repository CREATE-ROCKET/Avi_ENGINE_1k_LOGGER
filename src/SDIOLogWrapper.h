#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>

#ifndef SDIOLOGWRAPPER
#define SDIOLOGWRAPPER

#define SDIO_CH_DATA_SIZE 128
#define fileName "/test.txt"

class SDIOLogWrapper
{
private:
    static bool isSDOpend;
    static File logFile;
    static QueueHandle_t xQueue;
    static TaskHandle_t xWriteSDHandle;

public:
    static const int logCounterMax;

public:
    static int initSD();
    static void deinitSD();

    static void openFile();
    static void writeFile(const char mData[]);
    static void closeFile();

    static int makeQueue(int uxQueueLength);
    static int appendQueue(char xData[SDIO_CH_DATA_SIZE]);
    static void deleteQueue();

    static void writeSDfromQueue(void *parameters);
    static void writeTaskCreate(int TaskExecuteCore);
    static void writeTaskDelete();
};
#endif
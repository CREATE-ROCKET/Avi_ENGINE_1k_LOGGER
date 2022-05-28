#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>

#ifndef SDIOLOGWRAPPER
#define SDIOLOGWRAPPER

const int SDIO_CH_DATA_SIZE = 128; // CHANGE this via (logdata size)/(logging 1 clk)

/*
    mXXX: mediation
*/

class SDIOLogWrapper
{
private:
    bool isSDOpend = 0;
    const char *fileName;
    File logFile;
    const int logCounterMax = 1024;

    QueueHandle_t xQueue;

    TaskHandle_t xWriteSDHandle;

public:
    int initSD();
    void deinitSD();

    void openFile(const char mfileName[]);
    void writeFile(const char mData[]);
    void closeFile();

    int makeQueue(int uxQueueLength);
    int appendQueue(char xData[SDIO_CH_DATA_SIZE]);
    void deleteQueue();

    // static void writeSDfromQueue(void *parameters);
    // void writeTaskCreate(int TaskExecuteCore);
    void writeTaskDelete();
};
#endif
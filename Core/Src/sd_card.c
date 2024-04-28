/*
 * sd_card2.c
 *
 *  Created on: Mar 3, 2024
 *      Author: niko
 */

#include "fatfs.h"
#include "FreeRTOS.h"
#include "string.h"
#include "sd_card.h"

#define LOG_FILE "LOG_%u.txt"

// How many writes are allowed to be buffered before we force them to be written to a file
#define WRITES_UNTIL_SYNC 100

#define SD_REQUEST_MAX_MESSAGE_LENGTH 64

typedef enum {
	SDREQUEST_WRITE,
	SDREQUEST_SYNC,
} SDRequestType;

typedef struct {
	SDRequestType type;
	uint32_t length;
	char message[SD_REQUEST_MAX_MESSAGE_LENGTH];
} SDRequest;

#define SD_QUEUE_SIZE sizeof(SDRequest)
#define SD_QUEUE_LENGTH 512
//#define SD_QUEUE_LENGTH 128

static StaticQueue_t xSD_Card_Queue_Static;
QueueHandle_t xSD_Card_Queue;
uint8_t xSD_Card_Queue_Storage[SD_QUEUE_SIZE * SD_QUEUE_LENGTH];

FATFS FatFs; 	//Fatfs handle
FIL logFile; 	//File handle

uint32_t write_count = 0; // how many writes have occured since we've synced them
uint32_t log_index = 0;

uint32_t SDCardIsFilenameFree(char *filename) {
	FILINFO info;
	FRESULT fres = f_stat(filename, &info);

	return fres == FR_NO_FILE;
}



// Write n chars 1 at at time
void SDCardBenchmark(int n) {

	SDCardSync(); // Sync first so we start with a clean slate

	int start = HAL_GetTick();

	for(int i = 0; i < n; i++) {

		SDCardWrite("This is a test\n", 15);

	}

	SDCardSync(); // Sync at and end to force all writes, we're done counting time now

	int end = HAL_GetTick();
	int total_ticks = end - start;

	char total_message[256];

	int length = snprintf(total_message, 256, "\nTotal Ticks: %d\n", total_ticks) + 1;

	SDCardWrite(total_message, 256);

	SDCardSync(); // Force last message to be written

}

// "Do" functions are functions that handle internal stuff of the sd card

FRESULT DoSDCardMount(void) {
	return f_mount(&FatFs, "", 1);
}

FRESULT DoSDCardOpenFile(void) {
	FRESULT fres = FR_NOT_READY;

	char LOG_BUFFER[64] = {0};

	do {
		LOG_BUFFER[0] = '\0';
		snprintf(LOG_BUFFER, 64, LOG_FILE, log_index++);
	}while(!SDCardIsFilenameFree(LOG_BUFFER));

	fres = f_open(&logFile, LOG_BUFFER, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);

	return fres;
}

FRESULT DoSDCardWrite(char *buff, UINT n) {
	UINT bytesWritten;
	FRESULT fres = f_write(&logFile, buff, n, &bytesWritten);

	if(fres == FR_OK) {
		write_count++;
		if(write_count % WRITES_UNTIL_SYNC == 0) { // check if time to sync
			f_sync(&logFile); // sync, if we didn't do this file write wouldn't be pushed to the sd card and we've have to close the file to write them
			write_count = 0;
		}
	}

	return fres;
}

// forces sd to sync, aka write uncommited changes to sd card
FRESULT DoSDSync(void) {
	FRESULT fres = f_sync(&logFile);
	return fres;
}


FRESULT DoSDEject(void) {
	FRESULT fres = f_close(&logFile);

	if(fres == FR_OK) {
		fres = f_mount(NULL, "", 0); // unmount fat fs
	}

	return fres;
}

void SD_Init(void) {
	xSD_Card_Queue = xQueueCreateStatic(SD_QUEUE_LENGTH,
										SD_QUEUE_SIZE,
										xSD_Card_Queue_Storage,
										&xSD_Card_Queue_Static);

	configASSERT(xSD_Card_Queue);

	FRESULT fres = DoSDCardMount();

	if(fres == FR_OK) {
		fres = DoSDCardOpenFile(); // if mounted, open log file
	}

	//return fres;

}

void StartSDCardTask(void const *argument) {

	FRESULT fres = FR_OK;

	for(;;) {
		SDRequest sd_req;
		BaseType_t status = xQueueReceive(xSD_Card_Queue, &sd_req, portMAX_DELAY);

		if(status == pdPASS) {

			switch(sd_req.type) {
			case SDREQUEST_WRITE:
				fres = DoSDCardWrite(sd_req.message, sd_req.length);
			case SDREQUEST_SYNC:
				fres = DoSDSync();
			default:
				break; // maybe some sort of error checking here
			}
		}

		// osDelay(pdMS_TO_TICKS(500));
	}
}

// These functions queue up tasks that call their underlying implementations

// Queue up a write
_Bool SDCardWrite(char *message, uint32_t length) {
	SDRequest req;

	req.type = SDREQUEST_WRITE;

	length = length > SD_REQUEST_MAX_MESSAGE_LENGTH ?
			SD_REQUEST_MAX_MESSAGE_LENGTH : length;

	strncpy(req.message, message, length);
	req.length = length;

	BaseType_t status = xQueueSendToBack(xSD_Card_Queue, &req, 0);

	return status == pdPASS;
}

// Queues up a sync
_Bool SDCardSync(void) {
	SDRequest req;

	req.type = SDREQUEST_SYNC;
	req.length = 0;

	BaseType_t status = xQueueSendToBack(xSD_Card_Queue, &req, 0);

	return status == pdPASS;
}


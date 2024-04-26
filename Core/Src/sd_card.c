/*
 * sd_card2.c
 *
 *  Created on: Mar 3, 2024
 *      Author: niko
 */

#include "fatfs.h"
#include "FreeRTOS.h"
#include "string.h"

#define LOG_FILE "LOG_%u.txt"

// How many writes are allowed to be buffered before we force them to be written to a file
#define WRITES_UNTIL_SYNC 100

#define SD_REQUEST_MAX_MESSAGE_LENGTH 64

typedef struct {
	 uint32_t length;
	 char message[SD_REQUEST_MAX_MESSAGE_LENGTH];
} SDRequest;

#define SD_QUEUE_SIZE sizeof(SDRequest)
#define SD_QUEUE_LENGTH 128

static StaticQueue_t xSD_Card_Queue_Static;
QueueHandle_t xSD_Card_Queue;
uint8_t xSD_Card_Queue_Storage[SD_QUEUE_SIZE * SD_QUEUE_LENGTH];

FATFS FatFs; 	//Fatfs handle
FIL logFile; 	//File handle

uint32_t write_count = 0; // how many writes have occured since we've synced them
uint32_t log_index = 0;

uint32_t sd_is_filename_free(char *filename) {
	FILINFO info;
	FRESULT fres = f_stat(filename, &info);

	return fres == FR_NO_FILE;
}

FRESULT sd_mount(void) {
	return f_mount(&FatFs, "", 1);
}

FRESULT sd_open_log_file(void) {
	FRESULT fres = FR_NOT_READY;

	char LOG_BUFFER[64] = {0};

	do {
		LOG_BUFFER[0] = '\0';
		snprintf(LOG_BUFFER, 64, LOG_FILE, log_index++);
	}while(!sd_is_filename_free(LOG_BUFFER));

	fres = f_open(&logFile, LOG_BUFFER, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);

	return fres;
}

/*
FRESULT sd_init(void) {
	FRESULT fres = sd_mount();

	if(fres == FR_OK) {
		fres = sd_open_log_file();
	}

	return fres;
}
*/

FRESULT sd_log_to_file(char *buff, UINT n) {
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

/*
FRESULT sd_switch_log(void) {
	FRESULT fres = f_close(&logFile);

	if(fres == FR_OK) {
		fres = sd_open_log_file();
	}

	return fres;
}
*/

FRESULT sd_eject(void) {
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

	FRESULT fres = sd_mount();

	if(fres == FR_OK) {
		fres = sd_open_log_file(); // if mounted, open log file
	}

	//return fres;

}

void StartSDCardLogTask(void const *argument) {

	FRESULT fres = FR_OK;

	for(;;) {
		SDRequest sd_req;
		BaseType_t status = xQueueReceive(xSD_Card_Queue, &sd_req, portMAX_DELAY);

		if(status == pdPASS) {
			fres = sd_log_to_file(sd_req.message, sd_req.length);
		}

		osDelay(pdMS_TO_TICKS(500));
	}
}

_Bool SDCardLogWrite(char *message, uint32_t length) {
	SDRequest req;

	length = length > SD_REQUEST_MAX_MESSAGE_LENGTH ?
			SD_REQUEST_MAX_MESSAGE_LENGTH : length;

	strncpy(req.message, message, length);
	req.length = length;

	BaseType_t status = xQueueSendToBack(xSD_Card_Queue, &req, 0);

	return status == pdPASS;
}

/*
 * sd_card2.c
 *
 *  Created on: Mar 3, 2024
 *      Author: niko
 */

#include "fatfs.h"
#include "string.h"

#define LOG_FILE "LOG_%u.txt"

FATFS FatFs; 	//Fatfs handle
FIL logFile; 	//File handle


uint32_t log_index = 0;

FRESULT sd_mount(void) {
	return f_mount(&FatFs, "", 1);
}

FRESULT sd_open_log_file(void) {
	FRESULT fres = FR_NOT_READY;

	char LOG_BUFFER[64] = {0};

	snprintf(LOG_BUFFER, 64, LOG_FILE, log_index++);
	fres = f_open(&logFile, LOG_BUFFER, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);

	return fres;
}

FRESULT sd_log_to_file(char *buff, UINT n) {
	UINT bytesWritten;
	FRESULT fres = f_write(&logFile, buff, n, &bytesWritten);
	return fres;
}


FRESULT sd_switch_log(void) {
	FRESULT fres = f_close(&logFile);

	if(fres == FR_OK) {
		fres = sd_open_log_file();
	}

	return fres;
}


FRESULT sd_eject(void) {
	FRESULT fres = f_close(&logFile);

	if(fres == FR_OK) {
		fres = f_mount(NULL, "", 0); // unmount fat fs
	}

	return fres;
}

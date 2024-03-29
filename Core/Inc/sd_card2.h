/*
 * sd_card2.h
 *
 *  Created on: Mar 3, 2024
 *      Author: niko
 */

#ifndef INC_SD_CARD2_H_
#define INC_SD_CARD2_H_

#include "fatfs.h"

FRESULT sd_init(void);
FRESULT sd_mount(void);
FRESULT sd_open_log_file(void);
FRESULT sd_log_to_file(char*, UINT);
FRESULT sd_switch_log(void);
FRESULT sd_eject(void);

#endif /* INC_SD_CARD2_H_ */

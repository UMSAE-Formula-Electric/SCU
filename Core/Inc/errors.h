
#include "stdint.h"

typedef enum DataType {
	FLOAT = 0,
	INT32 = 1,
	UINT8 = 2,
	UINT32 = 4,
	BOOL = 8,
	NONE = 16,
} DataType;

uint8_t datatype_to_size(const enum DataType dt);

// All these tables must fit inside a CAN message using only 1 Byte
// So there can only be 256 of them from 0 - 255
typedef enum ErrorTable {
	BATTERY_VOLTAGE_LOWISH = 0,
	BATTERY_VOLTAGE_LOW = 1,
	BATTERY_VOLTAGE_LOWER = 2,
	BATTERY_VOLTAGE_LOWEST = 3,

	AMOUNT,
	UNALIGNED = 255		// Special error, means that the ErrorTable & errors map do not line up. NEVER change the value assigned or use this as your ERROR
} ERR_TABLE;

typedef enum WarningTable {
	BATTERY_VOLTAGE_LOWISH = 0,
	BATTERY_VOLTAGE_LOW = 1,
	BATTERY_VOLTAGE_LOWER = 2,
	BATTERY_VOLTAGE_LOWEST = 3,

	AMOUNT,				// Tracks the Amount of values in the table
	UNALIGNED = 255		// Special error, means that the WarningTable & warnings map do not line up. NEVER change the value assigned or use this as your WARNING
} WARN_TABLE;

typedef enum InfoTable {
	BATTERY_VOLTAGE = 0,

	AMOUNT,
	UNALIGNED = 255		// Special error, means that the InfoTable & infos map do not line up. NEVER change the value assigned or use this as your INFO
} INFO_TABLE;

// Logging functions

bool log_can_msg(void *can_data); // SCU specific

// Logging functions
bool log_error(ERR_TABLE error_type, DataType data_type, void* data);
bool log_warning(WARN_TABLE warning_type, DataType data_type, void* data);
bool log_info(INFO_TABLE info_type, DataType data_type, void* data);

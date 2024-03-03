
#include "errors.c"

#include "string.h"


typedef enum LogLevel {
	Error = 0,
	Warning = 1,
	Info = 2
} LogLevel;

uint8_t datatype_to_size(const enum DataType dt) {
	uint8_t size = 0;

	switch (dt) {
	case FLOAT:
	case INT32:
	case UINT32:
		size = 4;
		break;
	case UINT8:
	case BOOL:
		size = 1;
		break;
	case NONE:
		size = 0;
		break;
	}

	return size;
}


//TODO! Add the other log from can message...
bool log_can_msg(void *can_data) {
	if (can_data == NULL) { return false; }

	LogLevel log_level = can_data[0];

	uint8_t log_type = can_data[1]; // Should be guaranteed to be a valid entry from the table index
	DataType data_type = can_data[2];

	void *can_bytes = &can_data[3];

	switch (log_level) {
	case LogLevel.Error:
		return log_error(log_type, data_type, can_bytes);
		break;
	case LogLevel.Warning:
		return log_warning(log_type, data_type, can_bytes);
		break;
	case LogLevel.Info:
		return log_info(log_type, data_type, can_bytes);
		break;
	}
}

bool log_error(ERR_TABLE error_type, DataType data_type, void* data) {
	LogLevel log_level = LogLevel.Error;	// Error

	return true;
}

bool log_warning(WARN_TABLE warning_type, DataType data_type, void* data) {
	return false;
}

bool log_info(INFO_TABLE info_type, DataType data_type, void* data) {
	return false;
}


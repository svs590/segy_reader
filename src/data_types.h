#ifndef __DATA_TYPES
#define __DATA_TYPES

enum class seismic_data_type {
	UNKNOWN,
	EMPTY,
	INT,
	FLOAT,
	DOUBLE,
	CHAR,
	STRING,
	INT64,
	SHORT,
	USHORT,
};

enum class object_type {
	UNKNOWN,
	SEGY_READER,
	
	SEGY_HEADERMAP,
	
	SEGY_TRACEHEADER_FIELD,

	SEGY_TRACE,

	SEGY_TRACEHEADER,

	SEGY_BIN_HEADER

};

#endif

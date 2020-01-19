#pragma once

struct segy_file {
	static const int text_header_size = 3200;
	static const int bin_header_size = 400;
	static const int trace_header_size = 240;
};

enum class segy_coord {
    CDP,
    SRC
};

enum class segy_data_format {
    float32_ibm         = 1,
    int32_2complement,
    int16_2complement,
    float32_obsolete,
    float32,
    float64,
    int24_2complement,
    int8_2complement,
    int64_2complement,
    uint32,
    uint16,
    uint64,
    uint24              = 15,
    uint8
};

short segy_data_format_size(segy_data_format format);
#pragma once

struct segy_file {
	static const int text_header_size = 3200;
	static const int bin_header_size = 400;
	static const int trace_header_size = 240;
};
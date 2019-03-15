#ifndef SEGY_READER
#define SEGY_READER

#include <vector>
#include <string>
#include <map>

#include "utils.h"
#include "segy_bin_header.h"
#include "segy_header_map.h"

#ifdef PYTHON
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
#endif

class segy_line_map {
public:
	std::map<int, std::pair<int, int>>	ilStartEndTrc;
	std::map<int, int>					ilCountTrcs;
	std::map<int, std::pair<int, int>>	xlStartEndTrc;
	std::map<int, int>					xlCountTrcs;

	int il_count;
	int xl_count;
	int il_offset;
	int xl_offset;
	int il_start;
	int xl_start;
};

class segy_reader {
	int samples_count = NOT_INDEX;
	void *obj;

	enum class segy_sorting { iline, xline, unsorted, unknown };
	segy_sorting sorting = segy_sorting::unknown;
	segy_line_map lineMap;

	std::map<int, std::vector<int>> iline_trcs;
	std::map<int, std::vector<int>> xline_trcs;

public:
	segy_reader(const void *obj);
	segy_reader(const segy_reader &obj);
	~segy_reader();
	segy_reader& operator=(const void *obj);
	segy_reader(
		std::string filename_in,
		int nTracesBuffer,
		int segyHeaderMap,
		bool reverseByteOrderData_in,
		bool reverseByteOrderHdr_in,
		bool autoscale_hdrs_in
	);
	void closeFile();

	int sampleByteSize();
	int tracesCount();
	int samplesCount();
	int headersCount();
	float sampleInterval();

	segy_bin_header binHeader();
	std::string charBinHeader();

	segy_header_map traceHeaderMap();

	template <typename T>
	T headerValue(int index);

	void moveToTrace(int firstTraceIndex, int numTracesToRead);
	void moveToTrace(int firstTraceIndex);
	const float *nextTraceRef();
	std::vector<float> getNextTrace();
	std::vector<float> getTrace(int index);

	std::vector<float> iline(int il_num);
	std::vector<float> xline(int xl_num);

	void determineSorting();

#ifdef PYTHON
	py::array_t<float> pyGetNextTrace();
	py::array_t<float> pyIline(int il_num);
	py::array_t<float> pyXline(int xl_num);
#endif

private:
	void getLine(const std::vector<int> &trcs, int trc_buffer, std::vector<float> &line);
};

#endif

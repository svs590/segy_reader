#ifndef SEGY_READER
#define SEGY_READER

#include <vector>
#include <string>

#include "utils.h"
#include "segy_bin_header.h"
#include "segy_header_map.h"

class segy_line_map {
public:
	std::vector<int> ilineStartTraces;
	std::vector<int> xlineStartTraces;
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

	enum class segy_sorting { iline, xline, unsorting, unknown };
	segy_sorting sorting = segy_sorting::unknown;
	segy_line_map lineMap;
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

	void create_mem_map();

private:
	void determineSorting();
	void getLineFromSorted(int startTrace, int count, int offset, std::vector<float> &line);
};

#endif

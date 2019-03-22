#ifndef __SEGY_READER
#define __SEGY_READER

#include <vector>
#include <string>
#include <map>

#include "utils.h"
#include "seismic_data_provider.h"
#include "segy_bin_header.h"
#include "segy_header_map.h"
#include "segy_trace.h"

#ifdef PYTHON
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
#endif

class segy_line {
public:
	segy_line(
		int il_index,
		int xl_index,
		int cdpx_index,
		int cdpy_index,
		int srcx_index,
		int srcy_index
	);

	std::vector<int> traces;

	std::vector<segy_traceheader_field> trace_headers;
	
	std::pair<double, double> start_coord;
	std::pair<double, double> end_coord;

	std::vector<segy_trace> get();
	std::vector<segy_trace> get(
		std::pair<double, double> start_point,
		std::pair<double, double> end_point
	);
	
private:
	// Индексы полей в заголовках трасс
	int il_index;
	int xl_index;
	int cdpx_index;
	int cdpy_index;
	int srcx_index;
	int srcy_index;
};

class segy_cube_geometry {
public:
	int il_count;
	int xl_count;
	int il_offset;
	int xl_offset;
	int il_start;
	int xl_start;
};

class segy_reader : public seismic_data_provider {
	int f_samples_count = NOT_INDEX;
	void *obj;

	enum class segy_sorting { iline, xline, unsorted, unknown };
	segy_sorting sorting = segy_sorting::unknown;
	segy_cube_geometry geometry;

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

	virtual void close();
	virtual int traces_count();
	virtual int samples_count();
	virtual float sample_interval();

	virtual std::string text_header();
	virtual std::shared_ptr<seismic_abstract_header> bin_header();
	virtual std::shared_ptr<seismic_header_map> header_map();
	virtual std::shared_ptr<seismic_trace_header> trace_header(int index);
	virtual std::shared_ptr<seismic_trace> get_trace(int index);

	std::shared_ptr<seismic_trace_header> current_trace_header();

	int sampleByteSize();
	int headersCount();

	template <typename T>
	T headerValue(int index);

	void moveToTrace(int firstTraceIndex, int numTracesToRead);
	void moveToTrace(int firstTraceIndex);
	const float *nextTraceRef();
	std::vector<float> getNextTrace();

	std::vector<float> get_trace_data(int index);

	std::vector<segy_trace> iline(int il_num);
	std::vector<segy_trace> xline(int xl_num);

	virtual void preprocessing();

	virtual object_type type_id() { return object_type::SEGY_READER; }

#ifdef PYTHON
	segy_trace py_get_trace();
#endif

private:
	void getLine(const std::vector<int> &trcs, int trc_buffer, std::vector<segy_trace> &line);
};

#endif

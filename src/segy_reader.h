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
#include "seismic_geometry.h"

#ifdef PYTHON
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
#endif


class segy_reader : public seismic_data_provider {
	int f_samples_count = NOT_INDEX;
	int f_traces_count = NOT_INDEX;
	void *obj;
	bool processed = false;
	bool headers_in_memory = false;
	std::vector<std::shared_ptr<seismic_trace_header>> headers;
	std::shared_ptr<seismic_geometry_info> geometry;
	
public:
	segy_reader(const void *obj);
	segy_reader(const segy_reader &obj);
	~segy_reader();
	segy_reader& operator=(const void *obj);
	segy_reader(
		std::string filename_in,
		int nTracesBuffer,
		header_map_type segyHeaderMap,
		bool reverseByteOrderData_in,
		bool reverseByteOrderHdr_in,
		bool autoscale_hdrs_in
	);
	segy_reader(
		std::string filename_in,
		header_map_type segyHeaderMap,
		bool reverseByteOrderData_in,
		bool reverseByteOrderHdr_in,
		bool autoscale_hdrs_in
	);
	segy_reader(std::string filename_in, header_map_type segyHeaderMap);

	virtual void close();
	virtual int traces_count();
	virtual int samples_count();
	virtual float sample_interval();

	virtual std::string text_header();
	virtual std::shared_ptr<seismic_abstract_header> bin_header();
	virtual std::shared_ptr<seismic_header_map> header_map();
	virtual std::shared_ptr<seismic_trace_header> trace_header(int index);
	virtual std::shared_ptr<seismic_trace> get_trace(int index);

	virtual std::shared_ptr<seismic_geometry_info> get_geometry() { return geometry; }

	virtual std::vector<std::shared_ptr<seismic_trace>> get_traces(seismic_line_info line);
	virtual std::vector<std::shared_ptr<seismic_trace_header>> get_headers(seismic_line_info line);

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

	std::vector<segy_trace> iline(seismic_line_info line);
	std::vector<segy_trace> xline(int xl_num);

	virtual void preprocessing();

	virtual object_type type_id() { return object_type::SEGY_READER; }

private:
	void get_traces(
		const std::vector<int> &trcs, 
		int trc_buffer, 
		std::vector<std::shared_ptr<seismic_trace>> &seismic_line_info
	);
	void get_traces(
		int start_trace,
		int end_trace, 
		std::vector<std::shared_ptr<seismic_trace>> &line
	);
	void get_headers(
		const std::vector<int> &trcs,
		int trc_buffer,
		std::vector<std::shared_ptr<seismic_trace_header>> &seismic_line_info
	);
	void get_headers(
		int start_trace,
		int end_trace,
		std::vector<std::shared_ptr<seismic_trace_header>> &seismic_line_info
	);

	void line_processing(
		std::map<int, seismic_line_info> &line, 
		seismic_line_info::seismic_line_type type,
		std::string name_prefix, 
		int line_no,
		int trace_no, 
		std::pair<float, float> point
	);

	void check_memory_for_headers();
};

#ifdef PYTHON
void py_segy_reader_init(py::module &m,
	py::class_<segy_reader, std::shared_ptr<segy_reader>> &py_segy_reader);
#endif

#endif

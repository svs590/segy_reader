#ifndef __SEGY_READER
#define __SEGY_READER

#include <vector>
#include <string>
#include <map>

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#include <Eigen/Dense>

#include "utils.h"
#include "seismic_data_provider.h"
#include "seismic_header_map.h"
#include "seismic_geometry.h"
#include "segy_bin_header.h"
#include "segy_trace_header.h"
#include "segy_trace.h"

#ifdef PYTHON
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
#endif

class smart_trc_buffer {
    std::vector<std::shared_ptr<segy_trace_header>> f_headers_buffer;
    
    std::vector<byte_t>                             f_raw_buffer;
    std::vector<size_t>                             f_trc_offsets;
    size_t                                          f_capacity;
    size_t                                          f_absolute_trc_beg;
    size_t                                          f_absolute_trc_cur;
    size_t                                          f_buffer_trc_cur;

public:
    smart_trc_buffer();
    void                                            load(std::vector<byte_t> raw_buffer);
    
    void                                            set_capacity(size_t cap, short samples_count, segy_data_format format);
    size_t                                          capacity(size_t cap);
    size_t                                          size();
    bool                                            is_trc_loaded(size_t absolute_index);
    bool                                            is_header_loaded(size_t absolute_index);
    std::shared_ptr<segy_trace_header>              get_header(size_t absolute_index);
    std::shared_ptr<segy_trace>                     get_trace(size_t absolute_index);
};

struct segy_reader_config {
    std::wstring filename;
    header_map_type header_map_type = header_map_type::STANDARD;
    bool ebcdic_header              = true;
    bool little_endian              = false;
    bool reverse_byte_pairs         = false;
};

class segy_reader : public seismic_data_provider {
    segy_reader_config f_config;

    int f_samples_count             = NOT_INDEX;
    int64_t f_traces_count          = NOT_INDEX;
    int64_t f_approx_traces_count   = NOT_INDEX;
    int max_trc_addheaders          = 0;
	void *obj;
	bool processed;
	bool headers_in_memory;

    std::shared_ptr<seismic_header_map> f_header_map;
	std::shared_ptr<segy_bin_header> f_bin_header;
	std::string f_text_header;
	std::vector<std::shared_ptr<seismic_trace_header>> headers;
	std::shared_ptr<seismic_geometry_info> geometry;

	bfs::ifstream f_istream;
    size_t f_filesize;
    size_t f_first_trc_offset;

    std::vector<byte_t> buffer;
    size_t buffer_size;

public:
    ~segy_reader();
	segy_reader(const segy_reader_config &config);
    segy_reader_config get_config() { return f_config; }
    void set_config(const segy_reader_config &config);

	virtual void close();
	virtual int64_t traces_count();
	virtual int samples_count();
	virtual double sample_interval();

	virtual std::string text_header();
	virtual std::shared_ptr<seismic_abstract_header> bin_header();
	virtual std::shared_ptr<seismic_header_map> header_map();
	virtual void set_header_map(std::shared_ptr<seismic_header_map> map);
	virtual std::shared_ptr<seismic_trace_header> trace_header(int index);
	virtual std::shared_ptr<seismic_trace> get_trace(int index);

	virtual std::shared_ptr<seismic_geometry_info> get_geometry() { return geometry; }

	virtual std::vector<std::shared_ptr<seismic_trace>> get_traces(seismic_line_info line);
	virtual std::vector<std::shared_ptr<seismic_trace>>	get_traces(int start, int end);
	virtual std::vector<std::shared_ptr<seismic_trace_header>> get_headers(seismic_line_info line);
	virtual std::vector<std::shared_ptr<seismic_trace_header>> get_headers(int start, int end);

	std::shared_ptr<seismic_trace_header> current_trace_header();

	template <typename T>
	T headerValue(int index);

	const float *nextTraceRef();
	Eigen::VectorXf getNextTrace();

	Eigen::VectorXf get_trace_data(int index);

	virtual void preprocessing();

	virtual object_type type_id() { return object_type::SEGY_READER; }

private:


	void open_file();
	void close_file();
    void init(bool reopen);
    void determine_endian_order();
    void resize_buffer(size_t size);

    void move(int trc_index, int buffer);
    void move(int trc_index);

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

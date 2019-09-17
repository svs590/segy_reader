#pragma once

#include <vector>
#include <string>
#include <map>

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#include <Eigen/Dense>

#include "segy_smart_trc_buffer.h"
#include "utils.h"
#include "seismic_data_provider.h"
#include "seismic_header_map.h"
#include "seismic_geometry.h"
#include "segy_bin_header.h"
#include "segy_trace_header.h"
#include "segy_trace.h"
#include "segy_file.h"


#ifdef PYTHON
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
#endif

struct segy_reader_config {
    std::wstring filename;
    header_map_type f_header_map_type = header_map_type::STANDARD;
    bool ebcdic_header              = true;
    segy_coord coord                = segy_coord::CDP;
};

class segy_reader : public seismic_data_provider {
    segy_reader_config f_config;

    int                                                 f_samples_count       = NOT_INDEX;
    int64_t                                             f_traces_count        = NOT_INDEX;
    int64_t                                             f_approx_traces_count = NOT_INDEX;
    int                                                 max_trc_addheaders    = 0;
	bool                                                processed;
	bool                                                headers_in_memory;

    std::shared_ptr<seismic_header_map>                 f_header_map;
	std::shared_ptr<segy_bin_header>                    f_bin_header;
	std::string                                         f_text_header;
	std::vector<std::shared_ptr<seismic_trace_header>>  f_headers;
	std::shared_ptr<seismic_geometry_info>              f_geometry;

	bfs::ifstream f_istream;
    size_t f_filesize;
    size_t f_first_trc_offset;

    size_t buffer_default_size = 1;
    smart_trc_buffer smart_buffer;

public:
    ~segy_reader();
	segy_reader(const segy_reader_config &config);
    segy_reader_config                                  config() { return f_config; }
    void                                                set_config(const segy_reader_config &config);

	virtual void                                        close();
	virtual int64_t                                     traces_count();
	virtual int                                         samples_count();
	virtual double                                      sample_interval();

	virtual std::string                                 text_header();
	virtual std::shared_ptr<seismic_abstract_header>    bin_header();
	virtual std::shared_ptr<seismic_header_map>         header_map();
	virtual void                                        set_header_map(std::shared_ptr<seismic_header_map> map);
	virtual std::shared_ptr<seismic_trace_header>       header(int index);
	virtual std::shared_ptr<seismic_trace>              trace(int index);

	virtual std::shared_ptr<seismic_geometry_info>      geometry() { return f_geometry; }

	virtual std::vector<std::shared_ptr<seismic_trace>> traces(seismic_line_info line);
	virtual std::vector<std::shared_ptr<seismic_trace>>	traces(int start, int end);

	virtual 
    std::vector<std::shared_ptr<seismic_trace_header>>  headers(seismic_line_info line);

	virtual
    std::vector<std::shared_ptr<seismic_trace_header>>  headers(int start, int end);

	virtual void                                        preprocessing();

	virtual object_type type_id() { return object_type::SEGY_READER; }

private:
	void open_file();
	void close_file();
    void init(bool reopen);
    void resize_buffer(size_t size);
    void store_traces_to_buffer(int index);

    void move(int trc_index);

	void traces(
		const std::vector<int> &trcs, 
		int trc_buffer, 
		std::vector<std::shared_ptr<seismic_trace>> &seismic_line_info
	);
	void traces(
		int start_trace,
		int end_trace, 
		std::vector<std::shared_ptr<seismic_trace>> &line
	);
	void headers(
		const std::vector<int> &trcs,
		int trc_buffer,
		std::vector<std::shared_ptr<seismic_trace_header>> &seismic_line_info
	);
	void headers(
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

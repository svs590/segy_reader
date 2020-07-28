#pragma once

#include <vector>
#include <string>
#include <map>

#include <boost/filesystem.hpp>

#include <Eigen/Dense>

#include "utils.h"
#include "seismic_header_map.h"
#include "seismic_geometry.h"
#include "segy_text_header.h"
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

namespace bfs = boost::filesystem;


class segy_text_header;

struct segy_writer_config {
    std::wstring        filename;
    header_map_type     header_map_type = header_map_type::STANDARD;
    bool                ebcdic_header   = true;
    segy_data_format    data_format     = segy_data_format::automatic;
    seismic_coords_type coord           = seismic_coords_type::CDP;
};

class segy_writer {
    struct processed_info {
        double first_cdp    = std::numeric_limits<double>::max();
        double last_cdp     = std::numeric_limits<double>::lowest();
        double x_min        = std::numeric_limits<double>::max();
        double x_max        = std::numeric_limits<double>::lowest();
        double x_delta;
        double y_min        = std::numeric_limits<double>::max();
        double y_max        = std::numeric_limits<double>::lowest();
        double y_delta;
        double time_min     = std::numeric_limits<double>::lowest();
        double time_max     = std::numeric_limits<double>::max();
        double time_delta;
        double lat_min      = std::numeric_limits<double>::max();
        double lat_max      = std::numeric_limits<double>::lowest();
        double lat_delta;
        double long_min     = std::numeric_limits<double>::max();
        double long_max     = std::numeric_limits<double>::lowest();
        double long_delta;
        double trace_min    = std::numeric_limits<double>::max();
        double trace_max    = std::numeric_limits<double>::lowest();
        double trace_delta;
  
        size_t traces_count = 0;
        size_t samples_count = 0;
        double sample_interval = 0;
        int sorting_code = 1;
    };

    int                                         m_add_text_headers_count    = 0;
    int                                         m_add_trace_header_count    = 0;
    segy_data_format                            m_data_format               = segy_data_format::automatic;
    processed_info                              m_processed_info;
    segy_writer_config                          m_config;
    
    std::shared_ptr<segy_text_header>           m_text_header;
    std::shared_ptr<segy_bin_header>            m_bin_header;
    std::shared_ptr<seismic_header_map>         m_header_map;

    bfs::ofstream                               m_ostream;

    std::vector<byte_t>                         m_write_buffer;

    std::vector<std::shared_ptr<seismic_trace>> m_invalid_traces;

    size_t                                      m_text_header_offset;
    size_t                                      m_bin_header_offset;
    size_t                                      m_first_trc_offset;
    size_t                                      m_last_offset;

    size_t                                      m_buffer_pos;
    
    bool                                        m_invalid_sorting;
    bool                                        m_first_trace_processe = false;

    endian_order                                m_endian_order;

public:
    ~segy_writer();
    segy_writer(const segy_writer_config &config);
    segy_writer                                 config() { return m_config; }
    void                                        set_config(const segy_writer_config &config);


    void                                        close();

    std::shared_ptr<seismic_abstract_header>    text_header();
    void                                        set_text_header(
        std::shared_ptr<seismic_abstract_header> text_header
    );

    std::shared_ptr<seismic_abstract_header>    bin_header();
    void                                        set_bin_header(
        std::shared_ptr<seismic_abstract_header> bin_header
    );

    std::shared_ptr<seismic_header_map>         header_map();
    void                                        set_header_map(
        std::shared_ptr<seismic_header_map> map
    );

    void                                        write_trace(
        std::shared_ptr<seismic_trace> trace
    );
    void                                        write_traces(
        std::vector<std::shared_ptr<seismic_trace>> traces
    );
    void                                        write_line(
        seismic_line_info line,
        std::vector<std::shared_ptr<seismic_trace>> traces
    );

    void                                        ignore_invalid();
    void                                        write_invalid();

private:
    void init();
    void open_file();
    void close_file();
    void write_headers();

    void update_processed_info(std::shared_ptr<seismic_trace> trace);
    void update_processed_info_by_first(std::shared_ptr<seismic_trace> trace);
    void finalize_processed_info();
    void apply_processed_info();

    void prepare_text_header();
    void prepare_bin_header();
    
    void resize_beffers();
    
    void flush_buffer();
};


#ifdef PYTHON
void py_segy_writer_init(
    py::module &m,
    py::class_<segy_writer, std::shared_ptr<segy_writer>> &py_segy_writer
);
#endif
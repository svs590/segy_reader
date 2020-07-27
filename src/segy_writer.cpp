#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <bitset>

#include "segy_writer.h"
#include "segy_bin_header.h"
#include "segy_header_map.h"
#include "utils.h"
#include "data_conversion.h"
#include "segy_file.h"
#include "seismic_exception.h"

#include <Eigen/Dense>

using namespace std;


segy_writer::~segy_writer() {
    close_file();
}

segy_writer::segy_writer(const segy_writer_config &config) {
    set_config(config);
    init();
}

void segy_writer::set_config(const segy_writer_config &config) {
    m_config = config;
}

void segy_writer::init() {
    m_endian_order = native_order();

    size_t text_headers_size = segy_file::text_header_size * (1 + m_add_text_headers_count);
    size_t bin_header_size = segy_file::bin_header_size;

    m_bin_header_offset = text_headers_size;
    m_first_trc_offset  = text_headers_size + bin_header_size;
    m_last_offset       = m_first_trc_offset;

    m_text_header = make_shared<segy_text_header>(segy_text_header());
    m_bin_header = make_shared<segy_bin_header>(segy_bin_header());
    m_header_map = make_shared<segy_header_map>(segy_header_map());

    resize_beffers();

    open_file();

    m_ostream.write(std::vector<char>(m_last_offset, '\0').data(), m_last_offset);
}

void segy_writer::write_trace(shared_ptr<seismic_trace> trace) {
    
}

void segy_writer::write_line(
    seismic_line_info line,
    vector<shared_ptr<seismic_trace>> traces
) {
    m_buffer_pos = 0;

    if (!m_first_trace_processe) {
        update_processed_info_by_first(traces[0]);
        m_first_trace_processe = true;
    }

    for (size_t i = 0; i < traces.size(); ++i) {
        update_processed_info(traces[i]);

        auto raw = traces[i]->raw_data(m_config.data_format, m_endian_order);

        if (m_buffer_pos + raw.size() >= m_write_buffer.size()) {
            flush_buffer(m_buffer_pos);
            m_buffer_pos = 0;
        }

        memcpy(m_write_buffer.data() + m_buffer_pos, raw.data(), raw.size());
        m_buffer_pos += raw.size();
    }
}

void segy_writer::update_processed_info(shared_ptr<seismic_trace> trace) {
    if (trace->header()->coords() != m_config.coord)
        trace->header()->reset_coords(m_config.coord);

    double __float64;
    size_t __uint64;
    short __uint16;

    VARIANT_VALUE_CAST(__float64, trace->header()->get("CPD"));
    m_processed_info.first_cdp = std::min(m_processed_info.first_cdp, __float64);
    m_processed_info.last_cdp = std::max(m_processed_info.last_cdp, __float64);

    VARIANT_VALUE_CAST(__float64, trace->header()->X());
    m_processed_info.x_min = std::min(m_processed_info.x_min, __float64);
    m_processed_info.x_max = std::max(m_processed_info.x_max, __float64);

    VARIANT_VALUE_CAST(__float64, trace->header()->Y());
    m_processed_info.y_min = std::min(m_processed_info.y_min, __float64);
    m_processed_info.y_max = std::max(m_processed_info.y_max, __float64);

    size_t first_nonzero;
    size_t last_nonzero;
    
    std::visit(
        [&first_nonzero, &last_nonzero](auto &&data) {
            first_nonzero = 0;
            last_nonzero = data.size();

            for (size_t i = 0; i < data.size(); ++i)
                if (data(i) == 0)
                    first_nonzero++;
                else
                    break;

            for (size_t i = data.size() - 1; i > 0 ; --i) {
                if (data(i) == 0)
                    last_nonzero--;
                else
                    break;
            }
        },
        trace->data()
    );

    VARIANT_VALUE_CAST(__float64, trace->header()->sample_interval());
    auto f = trace->header()->get("Lag A");
    VARIANT_VALUE_CAST(__uint16, trace->header()->get("Lag A"));

    m_processed_info.time_min = std::max(
        m_processed_info.time_min ,
        -(double)first_nonzero * __float64 / 1000 ///< to millisec
        - (double)__uint16
    );
    m_processed_info.time_max = std::min(
        m_processed_info.time_max,
        -(double)last_nonzero  * __float64 / 1000 ///< to millisec
        - (double)__uint16
    );

    ++m_processed_info.traces_count;
}

void segy_writer::update_processed_info_by_first(shared_ptr<seismic_trace> trace) {
    size_t __uint64_val;
    double double_val;
    VARIANT_VALUE_CAST(__uint64_val, trace->header()->samples_count());
    VARIANT_VALUE_CAST(double_val, trace->header()->sample_interval());

    m_processed_info.samples_count = __uint64_val;
    m_processed_info.sample_interval = double_val;

    if (m_config.data_format == segy_data_format::automatic)
        m_config.data_format = segy_format_from_data(trace->data());
}

void segy_writer::finalize_processed_info() {
    m_processed_info.sorting_code = m_bin_header->sorting_code();

    m_processed_info.trace_min = m_processed_info.time_min;
    m_processed_info.trace_max = m_processed_info.time_max;

    m_processed_info.time_delta = m_processed_info.time_max - m_processed_info.time_min;
    m_processed_info.trace_delta = m_processed_info.trace_max - m_processed_info.trace_min;

    m_processed_info.x_delta = m_processed_info.x_max - m_processed_info.x_min;
    m_processed_info.y_delta = m_processed_info.y_max - m_processed_info.y_min;
}

void segy_writer::apply_processed_info() {
    m_text_header->set_first_cdp(m_processed_info.first_cdp);
    m_text_header->set_last_cdp(m_processed_info.last_cdp);

    m_text_header->set_x_min(m_processed_info.x_min);
    m_text_header->set_x_max(m_processed_info.x_max);
    m_text_header->set_x_delta(m_processed_info.x_delta);

    m_text_header->set_y_min(m_processed_info.y_min);
    m_text_header->set_y_max(m_processed_info.y_max);
    m_text_header->set_y_delta(m_processed_info.y_delta);

    m_text_header->set_time_min(m_processed_info.time_min);
    m_text_header->set_time_max(m_processed_info.time_max);
    m_text_header->set_time_delta(m_processed_info.time_delta);

    m_text_header->set_trace_min(m_processed_info.trace_min);
    m_text_header->set_trace_max(m_processed_info.trace_max);
    m_text_header->set_trace_delta(m_processed_info.trace_delta);
}

void segy_writer::close() {
    close_file();
}

void segy_writer::open_file() {
    m_ostream.open(m_config.filename, std::ios_base::trunc | std::ios_base::binary);
    if (m_ostream.fail())
        SR_THROW(runtime_error, "could not open file to write");
}

void segy_writer::close_file() {
    if (m_ostream.is_open()) {
        flush_buffer(m_buffer_pos);

        finalize_processed_info();
        apply_processed_info();

        write_headers();
        m_ostream.close();
    }
}

void segy_writer::write_headers() {
    prepare_text_header();
    prepare_bin_header();

    // Write text and bin headers
    m_ostream.seekp(m_text_header_offset);
    m_ostream.write(
        reinterpret_cast<char*>(m_text_header->raw_data().data()), 
        segy_file::text_header_size
    );

    m_ostream.seekp(m_bin_header_offset);
    m_ostream.write(
        reinterpret_cast<char*>(m_bin_header->raw_data().data()),
        segy_file::bin_header_size
    );
}

void segy_writer::resize_beffers() {
    size_t mem_avail = get_available_memory();
    
    m_write_buffer.resize((size_t)(0.15 * mem_avail));
}

void segy_writer::prepare_text_header() {
    m_text_header->set_add_text_headers_count(m_add_text_headers_count);
    m_text_header->set_add_trace_header_count(0);   ///< FIXME
    m_text_header->set_product_name("seismo_reader library");

    m_text_header->set_bin_header_loc(
        "bytes " + to_string(m_bin_header_offset) 
        + "-" + to_string(m_bin_header_offset + segy_file::bin_header_size)
    );
    m_text_header->set_bin_header_sample_interval_loc("bytes 17-18");
    m_text_header->set_bin_header_samples_count_loc("bytes 21-22");
    m_text_header->set_bin_header_data_format_loc("bytes 25-26");

    auto il_info = m_header_map->get_field("Inline");
    m_text_header->set_trace_header_iline_loc(
        "bytes " + to_string(std::get<0>(il_info) + 1)
        + "-" + to_string(
            std::get<0>(il_info)
            + std::get<1>(il_info)
        )
    );

    auto crl_info = m_header_map->get_field("Crossline");
    m_text_header->set_trace_header_crossline_loc(
        "bytes " + to_string(std::get<0>(crl_info) + 1)
        + "-" + to_string(
            std::get<0>(crl_info)
            + std::get<1>(crl_info)
        )
    );

    auto cdpx_info = m_header_map->get_field("CDP X");
    m_text_header->set_trace_header_cdpx_loc(
        "bytes " + to_string(std::get<0>(cdpx_info) + 1)
        + "-" + to_string(
            std::get<0>(cdpx_info)
            + std::get<1>(cdpx_info)
        )
    );

    auto cdpy_info = m_header_map->get_field("CDP Y");
    m_text_header->set_trace_header_cdpy_loc(
        "bytes " + to_string(std::get<0>(cdpy_info) + 1)
        + "-" + to_string(
            std::get<0>(cdpy_info)
            + std::get<1>(cdpy_info)
        )
    );

    auto sc_info = m_header_map->get_field("Samples count");
    m_text_header->set_trace_header_samples_count_loc(
        "bytes " + to_string(std::get<0>(sc_info) + 1)
        + "-" + to_string(
            std::get<0>(sc_info)
            + std::get<1>(sc_info)
        )
    );

    auto si_info = m_header_map->get_field("Sample interval");
    m_text_header->set_trace_header_sample_interval_loc(
        "bytes " + to_string(std::get<0>(si_info) + 1)
        + "-" + to_string(
            std::get<0>(si_info)
            + std::get<1>(si_info)
        )
    );

    auto srcx_info = m_header_map->get_field("Src X");
    m_text_header->set_trace_header_srcx_loc(
        "bytes " + to_string(std::get<0>(srcx_info) + 1)
        + "-" + to_string(
            std::get<0>(srcx_info)
            + std::get<1>(srcx_info)
        )
    );

    auto srcy_info = m_header_map->get_field("Src Y");
    m_text_header->set_trace_header_srcy_loc(
        "bytes " + to_string(std::get<0>(srcy_info) + 1)
        + "-" + to_string(
            std::get<0>(srcy_info)
            + std::get<1>(srcy_info)
        )
    );

    auto scf_info = m_header_map->get_field("Scalar coords");
    m_text_header->set_trace_header_scale_factor_loc(
        "bytes " + to_string(std::get<0>(scf_info) + 1)
        + "-" + to_string(
            std::get<0>(scf_info)
            + std::get<1>(scf_info)
        )
    );

    m_text_header->set_date_time(date_time());
}

void segy_writer::prepare_bin_header() {
    m_bin_header->set_samples_count(m_processed_info.samples_count);
    m_bin_header->set_sample_interval(m_processed_info.sample_interval);
    m_bin_header->set_sorting_code(m_processed_info.sorting_code);

    if (m_config.data_format == segy_data_format::automatic)
        SR_THROW(runtime_error, "data format is incorrect after all traces written. "
            "Probably you change writer config after start recording.")
    m_bin_header->set_data_format(m_config.data_format);
    m_bin_header->set_endian(m_endian_order);
}

void segy_writer::flush_buffer(size_t size) {
    m_ostream.write(reinterpret_cast<char*>(m_write_buffer.data()), size);
}

#ifdef PYTHON
void py_segy_writer_init(
    py::module &m,
    py::class_<segy_writer, std::shared_ptr<segy_writer>> &py_segy_writer
) {
    py::class_<segy_writer_config> reader_config(m, "writer_config");
    reader_config.def(py::init<>());
    reader_config.def_property("filename",
        [](segy_writer_config &c) { return c.filename; },
        [](segy_writer_config &c, std::wstring &filename) { c.filename = filename; }
    );
    reader_config.def_property("data_format",
        [](segy_writer_config &c) { return c.data_format; },
        [](segy_writer_config &c, int df) { c.data_format = (segy_data_format)df; }
    );

    py_segy_writer.def(py::init<const segy_writer_config &>());

    py_segy_writer.def("write_line", &segy_writer::write_line);
    py_segy_writer.def("close", &segy_writer::close);
}
#endif
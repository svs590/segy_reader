#pragma once

#include <map>
#include <string>
#include <vector>

#include <boost/preprocessor.hpp>

#ifdef PYTHON
#include <pybind11/pybind11.h>

namespace py = pybind11;
#endif

#include "seismic_abstract_header.h"
#include "data_conversion.h"
#include "segy_file.h"


#define SyBH_FIELD_NAME(tuple)                                                      \
    BOOST_PP_TUPLE_ELEM(5, 0, tuple)

#define SyBH_FIELD_RAW_TYPE(tuple)                                                  \
    BOOST_PP_TUPLE_ELEM(5, 1, tuple)

#define SyBH_FIELD_OUT_TYPE(tuple)                                                  \
    BOOST_PP_TUPLE_ELEM(5, 2, tuple)

#define SyBH_FIELD_OFFSET(tuple)                                                    \
    BOOST_PP_TUPLE_ELEM(5, 3, tuple)

#define SyBH_FIELD_DESCR(tuple)                                                     \
    BOOST_PP_TUPLE_ELEM(5, 4, tuple)

#define DEFINE_SyBH_FIELDS_OP(z, data, el)                                          \
    SyBH_FIELD_OUT_TYPE(el) BOOST_PP_CAT(m_, SyBH_FIELD_NAME(el));

#define DEFINE_SyBH_GET_METHODS_OP(z, data, el)                                     \
    SyBH_FIELD_OUT_TYPE(el) SyBH_FIELD_NAME(el)()                                   \
        { return BOOST_PP_CAT(m_, SyBH_FIELD_NAME(el)); }

#define DEFINE_SyBH_SET_METHODS_OP(z, data, el)                                     \
    void                                                                            \
    BOOST_PP_CAT(set_, SyBH_FIELD_NAME(el))(const SyBH_FIELD_OUT_TYPE(el) &val) {   \
        m_map_need_update = true; BOOST_PP_CAT(m_, SyBH_FIELD_NAME(el)) = val;            \
    }

#define DEFINE_SyBH_METHODS(seq, method_op)                                         \
    BOOST_PP_SEQ_FOR_EACH(method_op, ~, seq)

#define DEFINE_SyBH_FIELDS(seq)                                                     \
    BOOST_PP_SEQ_FOR_EACH(DEFINE_SyBH_FIELDS_OP, ~, seq)


class segy_bin_header : public seismic_abstract_header {

/**
  * Segy bin header fieds tuple like (name, raw_type, type_out, byte_offset, brief description) 
  * where
  *     name            - field name
  *     raw_type        - raw type (see segy doc.)
  *     type_out        - type that use to store value in runtime
  *     byte_offset     - first byte of field (see segy doc.)
  */
#define SyBH_FIELDS                                                     \
    ((                                                                  \
        job_id,                    int32_t,    int32_t,            0,   \
        "Job identification number"                                     \
    ))                                                                  \
    ((                                                                  \
        line_num,                  int32_t,    int32_t,            4,   \
        "Line number. For 3D data, this will contain the in-linenumber" \
    ))                                                                  \
    ((                                                                  \
        reel_num,                  int32_t,    int32_t,            8,   \
        "Reel number"                                                   \
    ))                                                                  \
    ((                                                                  \
        traces_count,              uint16_t,   int32_t,            12,  \
        "Number of data traces per ensemble"                            \
    ))                                                                  \
    ((                                                                  \
        aux_traces_count,          uint16_t,   int32_t,            14,  \
        "Number of auxiliary traces per ensemble"                       \
    ))                                                                  \
    ((                                                                  \
        sample_interval,           uint16_t,   double,             16,  \
        "Sample interval (us / Hz / m / ft)"                            \
    ))                                                                  \
    ((                                                                  \
        sample_interval_orig,      uint16_t,   double,             18,  \
        "Sample interval of original field recording (us / Hz / m / ft)"\
    ))                                                                  \
    ((                                                                  \
        samples_count,             uint16_t,   int32_t,            20,  \
        "Number of samples per data trace"                              \
    ))                                                                  \
    ((                                                                  \
        samples_count_orig,        uint16_t,   int32_t,            22,  \
        "Number of samples per data trace for original field recording" \
    ))                                                                  \
    ((                                                                  \
        data_format,               uint16_t,   segy_data_format,   24,  \
        "Data sample format code:\n\t1 - 4-byte IBM float"              \
        "\n\t2 - 4-byte, two\'s complement integer"                     \
        "\n\t3 - 2-byte, two\'s complement integer"                     \
        "\n\t4 - 4-byte fixed-point with gain (obsolete)"               \
        "\n\t5 - 4-byte IEEE float\n\t6 - 8-byte IEEE float"            \
        "\n\t7 - 3-byte two\'s complement integer"                      \
        "\n\t8 - 1-byte, two\'s complement integer"                     \
        "\n\t9 - 8-byte, two\'s complement integer"                     \
        "\n\t10 - 4-byte, unsigned integer\n\t11 - 2-byte, unsigned integer"\
        "\n\t12 - 8-byte, unsigned integer\n\t15 - 3-byte, unsigned integer"\
        "\n\t16 - 1-byte, unsigned integer"                             \
    ))                                                                  \
                                                                        \
    ((                                                                  \
        ensemble_fold,             uint16_t,   int32_t,            26,  \
        "The expected number of data traces per trace ensemble (e.g. the CMP fold)"\
    ))                                                                  \
    ((                                                                  \
        sorting_code,              uint16_t,   int32_t,            28,  \
        "Trace sorting code:\n\t-1 - Other\n\t0 - Unknown\n\t1 - No sorting"\
        "\n\t2 - CDP ensemble\n\t3 - Single fold continuous profile"    \
        "\n\t4 - Horizontally stacked\n\t5 - Common source point"       \
        "\n\t6 - Common receiver point\n\t7 - Common offset point"      \
        "\n\t8 - Common mid-point\n\t9 - Common conversion point"       \
    ))                                                                  \
    ((                                                                  \
        vert_sum_code,             uint16_t,   int32_t,            30,  \
        "Vertical sum code:\n\t1 - No sum\n\t2 - two sum\n\t..."        \
    ))                                                                  \
    ((                                                                  \
        sweep_fr_start,            uint16_t,   int32_t,            32,  \
        "Sweep frequency at start (Hz)"                                 \
    ))                                                                  \
    ((                                                                  \
        sweep_fr_end,              uint16_t,   int32_t,            34,  \
        "Sweep frequency at end (Hz)"                                   \
    ))                                                                  \
    ((                                                                  \
        sweep_len,                 uint16_t,   int32_t,            36,  \
        "Sweep length (ms)"                                             \
    ))                                                                  \
    ((                                                                  \
        sweep_type,                uint16_t,   int32_t,            38,  \
        "Sweep type code:\n\t1 - Linear\n\t2 - Parabolic\n\t 3 - Exponential"\
        "\n\t4 - Other"                                                 \
    ))                                                                  \
    ((                                                                  \
        sweep_chanel_trcs_count,   uint16_t,   int32_t,            40,  \
        "Trace number of sweep channel"                                 \
    ))                                                                  \
    ((                                                                  \
        sweep_trc_taper_len_start, uint16_t,   int32_t,            42,  \
        "Sweep trace taper length in milliseconds at start if tapered " \
        "(the taper starts at zero time and is effective for this length)"\
    ))                                                                  \
    ((                                                                  \
        sweep_trc_taper_len_end,   uint16_t,   int32_t,            44,  \
        "Sweep trace taper length in milliseconds at end (the ending "  \
        "taper starts at sweep length minus the taper length at end)"   \
    ))                                                                  \
    ((                                                                  \
        taper_type,                uint16_t,   int32_t,            46,  \
        "Taper type:\n\t1 - Linear\n\t2 - Cosine squared\n\t3 = Other"  \
    ))                                                                  \
    ((                                                                  \
        correlated_traces,         uint16_t,   int32_t,            48,  \
        "Correlated data traces:\n\t 1 - No\n\t2 = Yes"                 \
    ))                                                                  \
    ((                                                                  \
        gain_recovered,            uint16_t,   int32_t,            50,  \
        "Binary gain recovered:\n\t1 - Yes\n\t2 = No"                   \
    ))                                                                  \
    ((                                                                  \
        amplitude_rec_method,      uint16_t,   int32_t,            52,  \
        "Amplitude recovery method:\n\t1 - None\n\t2 - Spherical divergence"\
        "\n\t3 - AGC\n\t4 - Other"                                      \
    ))                                                                  \
    ((                                                                  \
        measurement_system,        uint16_t,   int32_t,            54,  \
        "Measurement system (see Segy documentation):\n\t1 - Meters"    \
        "\n\t2 - Feet"                                                  \
    ))                                                                  \
                                                                        \
    ((                                                                  \
        signal_polarity,           uint16_t,   int32_t,            56,  \
        "Impulse signal polarity:\n\t1 - Increase in pressure or upward "\
        "geophone case movement gives negativenumber on trace"          \
        "\n\t2 - Increase in pressure or upward geophone case movement "\
        "gives positivenumber on trace"                                 \
    ))                                                                  \
    ((                                                                  \
        polarity_code,             uint16_t,   int32_t,            58,  \
        "Vibratory polarity code (seismic signal lags pilot signal by):"\
        "\n\t1 - 337.5 to 22.5 deg\n\t2 - 22.5 to 67.5 deg"             \
        "\n\t3 - 67.5 to 112.5 deg\n\t4 - 112.5 to 157.5 deg"           \
        "\n\t5 - 157.5 to 202.5 deg\n\t6 - 202.5 to 247.5 deg"          \
        "\n\t7 - 247.5 to 292.5 deg\n\t8 - 292.5 to 337.5 deg"          \
    ))                                                                  \
                                                                        \
    ((                                                                  \
        extended_traces_count,     int32_t,    int32_t,            60,  \
        "Extended number of data traces per ensemble"                   \
    ))                                                                  \
    ((                                                                  \
        extended_aux_traces_count, int32_t,    int32_t,            64,  \
        "Extended number of auxiliary traces per ensemble"              \
    ))                                                                  \
    ((                                                                  \
        extended_samples_count,    int32_t,    int32_t,            68,  \
        "Extended number of samples per data trace"                     \
    ))                                                                  \
    ((                                                                  \
        extended_sample_interval,  double,     double,             72,  \
        "Extended sample interval, IEEE double precision (64-bit)"      \
    ))                                                                  \
    ((                                                                  \
        extended_sample_interval_orig, double, double,             80,  \
        "Extended sample interval of original field recording, "        \
        "IEEE double precision (64-bit)"                                \
    ))                                                                  \
    ((                                                                  \
        extended_samples_count_orig, int32_t,  int32_t,            88,  \
        "Extended number of samples per data trace in original recording"\
    ))                                                                  \
    ((                                                                  \
        extended_ensemble_fold,    int32_t,    int32_t,            92,  \
        "Extended ensemble fold"                                        \
    ))                                                                  \
                                                                        \
    ((                                                                  \
        is_segy_2,                 uint8_t,    bool,               300, \
        "Major SEG-Y Format Revision Number. A value of 1 indicates "   \
        "SEG-Y 2.0 format, a value ofzero indicates traditional SEG-Y"  \
    ))                                                                  \
    ((                                                                  \
        is_same_for_file,          int16_t,    int32_t,            302, \
        "A value of one indicates that all traces in this SEG-Yfile "   \
        "are guaranteed to have the same sample interval, number of "   \
        "trace headerblocks and trace samples"                          \
    ))                                                                  \
    ((                                                                  \
        extended_text_headers_count, int16_t,  int32_t,            304, \
        "Number of 3200-byte, Extended Textual File Header"             \
    ))                                                                  \
    ((                                                                  \
        max_add_trc_headers_count, int32_t,    int32_t,            306, \
        "Maximum number of additional 240 byte trace headers"           \
    ))                                                                  \
    ((                                                                  \
        time_basis,                uint16_t,   uint32_t,           310, \
        "Time basis code:\n\t1 - Local\n\t2 - GMT\n\t3 - Other (see "   \
        "Extended Textual File Header)\n\t4 - UTC\n\t5 - GPS"           \
    ))                                                                  \
    ((                                                                  \
        stream_traces_count,       uint64_t,   uint64_t,           312, \
        "Number of traces in this file or stream"                       \
    ))                                                                  \
    ((                                                                  \
        first_trace_offset,        uint64_t,   uint64_t,           320, \
        "Byte offset of first trace relative to start of file or "      \
        "stream if known, otherwisezero"                                \
    ))

    static std::map<std::string, std::map<std::string, std::string>>    m_descr;

    bool                                                                m_map_need_update;
    endian_order                                                        m_endian_order = endian_order::big;

    DEFINE_SyBH_FIELDS(SyBH_FIELDS);

public:
    DEFINE_SyBH_METHODS(SyBH_FIELDS, DEFINE_SyBH_GET_METHODS_OP);
    DEFINE_SyBH_METHODS(SyBH_FIELDS, DEFINE_SyBH_SET_METHODS_OP);

	segy_bin_header();
	segy_bin_header(const std::vector<byte_t> &m_raw_data);
    segy_bin_header(const std::map<std::string, seismic_variant_value> &m_raw_data);
    segy_bin_header(segy_bin_header &bin_header);
	~segy_bin_header();

    endian_order endian()               { return m_endian_order;    }
    void set_endian(endian_order order) { m_map_need_update = true; m_endian_order = order; }

    virtual seismic_variant_value get(const std::string &name);
    virtual std::map<std::string, seismic_variant_value> to_map();

    virtual void set(const std::string &name, seismic_variant_value val);
    virtual void from_map(const std::map<std::string, seismic_variant_value> &map);

    virtual std::map<std::string, std::map<std::string, std::string>> fields_descr();

    std::vector<byte_t> raw_data();

private:
	void init_from_raw();
    void write_to_raw();
    void init_map();
	void set_zero();
    void init_endian();
    void write_endian();
    void override_by_extended();
};

#ifdef PYTHON
void py_segy_bin_header_init(
    py::module &m,
	py::class_<segy_bin_header, std::shared_ptr<segy_bin_header>> &py_segy_bin_header
);
#endif

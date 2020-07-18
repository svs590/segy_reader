#include "segy_bin_header.h"
#include "segy_file.h"
#include "data_conversion.h"
#include "utils.h"
#include "seismic_exception.h"

#include <iostream>
#include <iterator>
#include <string>

using namespace std;

segy_bin_header::~segy_bin_header() {

}

segy_bin_header::segy_bin_header() {
	set_zero();
}

segy_bin_header::segy_bin_header(const vector<byte_t> &raw_data) {
    if (raw_data.size() != segy_file::bin_header_size)
        SR_THROW(runtime_error, "segy_bin_header: binary file header has invalid size");

    this->raw_data = raw_data;
    determine_endian();
	initialize();
}

void segy_bin_header::determine_endian() {
    int little_to_native = byte_to_int<endian_order::little>(&raw_data[96]);
    int big_to_native = byte_to_int<endian_order::big>(&raw_data[96]);
    int midlittle_to_native = byte_to_int<endian_order::mid_little>(&raw_data[96]);

    if (little_to_native == 16909060)
        f_endian_order = endian_order::little;
    else if (big_to_native == 16909060)
        f_endian_order = endian_order::big;
    else if (midlittle_to_native == 16909060)
        f_endian_order = endian_order::mid_little;
    else if (big_to_native == 16909060)
        f_endian_order = endian_order::mid_big;
    else {
        int data_format_little = byte_to_ushort<endian_order::little>(&raw_data[24]);
        int data_format_big = byte_to_ushort<endian_order::big>(&raw_data[24]);
        if (data_format_little >= 1 && data_format_little <= 16)
            f_endian_order = endian_order::little;
        else if (data_format_big >= 1 && data_format_big <= 16)
            f_endian_order = endian_order::big;
        else {
            cout << "Warning: segy_bin_header: initialize: can't determine endianness. "
                "Data may be incorrect, try to change endianness by yourself." << endl;
            f_endian_order = endian_order::big;
        }
    }
}

void segy_bin_header::initialize() {
	f_is_segy_2 = (unsigned char)raw_data[300] != 0 ? true : false;

	f_job_id                        = byte_to_int(&raw_data[0], f_endian_order);
	f_line_num                      = byte_to_int(&raw_data[4], f_endian_order);
	f_reel_num                      = byte_to_int(&raw_data[8], f_endian_order);
	f_traces_count                  = (int)byte_to_ushort(&raw_data[12], f_endian_order);
	f_aux_traces_count              = (int)byte_to_ushort(&raw_data[14], f_endian_order);
	f_sample_interval               = (double)byte_to_ushort(&raw_data[16], f_endian_order);
	f_sample_interval_orig          = (double)byte_to_ushort(&raw_data[18], f_endian_order);
	f_samples_count                 = (int)byte_to_ushort(&raw_data[20], f_endian_order);
	f_samples_count_orig            = (int)byte_to_ushort(&raw_data[22], f_endian_order);
	f_data_format                   = (segy_data_format)byte_to_ushort(&raw_data[24], f_endian_order);
    
	f_ensemble_fold                 = (int)byte_to_ushort(&raw_data[26], f_endian_order);
	f_sorting_code                  = (int)byte_to_ushort(&raw_data[28], f_endian_order);
	f_vert_sum_code                 = (int)byte_to_ushort(&raw_data[30], f_endian_order);
	f_sweep_fr_start                = (int)byte_to_ushort(&raw_data[32], f_endian_order);
	f_sweep_fr_end                  = (int)byte_to_ushort(&raw_data[34], f_endian_order);
	f_sweep_len                     = (int)byte_to_ushort(&raw_data[36], f_endian_order);
	f_sweep_type                    = (int)byte_to_ushort(&raw_data[38], f_endian_order);
	f_sweep_chanel_trcs_count       = (int)byte_to_ushort(&raw_data[40], f_endian_order);
	f_sweep_trc_taper_len_start     = (int)byte_to_ushort(&raw_data[42], f_endian_order);
	f_sweep_trc_taper_len_end       = (int)byte_to_ushort(&raw_data[44], f_endian_order);
	f_taper_type                    = (int)byte_to_ushort(&raw_data[46], f_endian_order);
	f_correlated_traces             = (int)byte_to_ushort(&raw_data[48], f_endian_order);
	f_gain_recovered                = (int)byte_to_ushort(&raw_data[50], f_endian_order);
	f_amplitude_rec_method          = (int)byte_to_ushort(&raw_data[52], f_endian_order);
	f_measurement_system            = (int)byte_to_ushort(&raw_data[54], f_endian_order);
    
	f_signal_polarity               = (int)byte_to_ushort(&raw_data[56], f_endian_order);
	f_polarity_code                 = (int)byte_to_ushort(&raw_data[58], f_endian_order);
    
	f_extended_traces_count         = byte_to_int(&raw_data[60], f_endian_order);
	f_extended_aux_traces_count     = byte_to_int(&raw_data[64], f_endian_order);
	f_extended_samples_count        = byte_to_int(&raw_data[68], f_endian_order);
	f_extended_sample_interval      = byte_to_double(&raw_data[72], f_endian_order);
	f_extended_sample_interval_orig = byte_to_double(&raw_data[80], f_endian_order);
	f_extended_samples_count_orig   = byte_to_int(&raw_data[88], f_endian_order);
	f_extended_ensemble_fold        = byte_to_int(&raw_data[92], f_endian_order);

	if (f_extended_traces_count != 0)
		f_traces_count = f_extended_traces_count;
	if (f_extended_aux_traces_count != 0)
		f_aux_traces_count = f_extended_aux_traces_count;
	if (f_extended_samples_count != 0)
		f_samples_count = f_extended_samples_count;
	if (f_extended_sample_interval != 0)
		f_sample_interval = f_extended_sample_interval;
	if (f_extended_sample_interval_orig != 0)
		f_sample_interval_orig = f_extended_sample_interval_orig;
	if (f_extended_ensemble_fold != 0)
		f_ensemble_fold = f_extended_ensemble_fold;

	f_is_same_for_file              = (int)byte_to_short(&raw_data[302], f_endian_order);
	f_extended_text_headers_count   = (int)byte_to_short(&raw_data[304], f_endian_order);
	f_max_add_trc_headers_count     = byte_to_int(&raw_data[306], f_endian_order);
	f_time_basis                    = (int)byte_to_ushort(&raw_data[310], f_endian_order);
	f_stream_traces_count           = byte_to_uint64(&raw_data[312], f_endian_order);
	f_first_trace_offset            = byte_to_uint64(&raw_data[320], f_endian_order);

    init_map();
}

void segy_bin_header::set_zero() {
	raw_data = vector<byte_t>(segy_file::bin_header_size, (byte_t)0);
	initialize();
}

seismic_variant_value segy_bin_header::get(const string &name) {
    if (f_map_need_update)
        init_map();
    auto it = f_fields.find(name);
    if (it != f_fields.cend())
        return it->second;
    else
        SR_THROW(invalid_argument, "invalid field name");
}

void segy_bin_header::set(const std::string &name, seismic_variant_value val) {
    auto it = f_fields.find(name);
    if (it != f_fields.cend())
        set_field(name, val);
    else
        SR_THROW(invalid_argument, "invalid field name");
}

#define SET_FIELD_SWITCH_CASE_IMPL(op, field_name, input_name, field_type, value)   \
    op (input_name == #field_name) {                                                \
        field_type __val;                                                           \
        VARIANT_VALUE_CAST(__val, value);                                           \
        set_##field_name#(__val);                                                   \
    }

#define SET_FIELD_SWITCH_CASE_BEGIN(field_name, input_name, field_type, value)      \
    SET_FIELD_SWITCH_CASE_IMPL(if, field_name, input_name, field_type, value)

#define SET_FIELD_SWITCH_CASE(field_name, input_name, field_type, value)            \
    SET_FIELD_SWITCH_CASE_IMPL(else if, field_name, input_name, field_type, value)


void segy_bin_header::set_field(const std::string &name, seismic_variant_value &val) {
    SET_FIELD_SWITCH_CASE_BEGIN(job_id, name, decltype(f_job_id), val)
    SET_FIELD_SWITCH_CASE(line_num, name, decltype(f_line_num), val)
    SET_FIELD_SWITCH_CASE(reel_num, name, decltype(f_reel_num), val)
    SET_FIELD_SWITCH_CASE(traces_count, name, decltype(f_traces_count), val)
    SET_FIELD_SWITCH_CASE(aux_traces_count, name, decltype(f_aux_traces_count), val)
    SET_FIELD_SWITCH_CASE(sample_interval, name, decltype(f_sample_interval), val)
    SET_FIELD_SWITCH_CASE(sample_interval_orig, name, decltype(f_sample_interval_orig), val)
    SET_FIELD_SWITCH_CASE(samples_count, name, decltype(f_samples_count), val)
}

map<string, seismic_variant_value> segy_bin_header::to_map() {
    if (f_map_need_update)
        init_map();
	return f_fields;
}

void segy_bin_header::init_map() {
    f_fields = {
        { "job_id",                    f_job_id,                   },
        { "line_num",                  f_line_num,                 },
        { "reel_num",                  f_reel_num,                 },
        { "traces_count",              f_traces_count,             },
        { "aux_traces_count",          f_aux_traces_count,         },
        { "sample_interval",           f_sample_interval,          },
        { "sample_interval_orig",      f_sample_interval_orig,     },
        { "samples_count",             f_samples_count,            },
        { "samples_count_orig",        f_samples_count_orig,       },
        { "data_format",               (int)f_data_format,         },
        { "ensemble_fold",             f_ensemble_fold,            },
        { "sorting_code",              f_sorting_code,             },
        { "vert_sum_code",             f_vert_sum_code,            },
        { "sweep_fr_start",            f_sweep_fr_start,           },
        { "sweep_fr_end",              f_sweep_fr_end,             },
        { "sweep_len",                 f_sweep_len,                },
        { "sweep_type",                f_sweep_type,               },
        { "sweep_chanel_trcs_count",   f_sweep_chanel_trcs_count,  },
        { "sweep_trc_taper_len_start", f_sweep_trc_taper_len_start,},
        { "sweep_trc_taper_len_end",   f_sweep_trc_taper_len_end,  },
        { "taper_type",                f_taper_type,               },
        { "correlated_traces",         f_correlated_traces,        },
        { "gain_recovered",            f_gain_recovered,           },
        { "amplitude_rec_method",      f_amplitude_rec_method,     },
        { "measurement_system",        f_measurement_system,       },
        { "signal_polarity",           f_signal_polarity,          },
        { "polarity_code",             f_polarity_code,            },
        { "endian_swap",               (int)f_endian_order,        },
        { "is_segy_2",                 (int)f_is_segy_2,           },
        { "is_same_for_file",          f_is_same_for_file,         },
        { "extended_headers_count",    f_extended_text_headers_count},
        { "max_add_trc_headers_count", f_max_add_trc_headers_count,},
        { "time_basis",                f_time_basis,               },
        { "stream_traces_count",       f_stream_traces_count,      },
        { "first_trace_offset",        f_first_trace_offset,       }
    };

    f_map_need_update = false;
}

#ifdef PYTHON
void py_segy_bin_header_init(py::module &m,
	py::class_<segy_bin_header, shared_ptr<segy_bin_header>> &py_segy_bin_header) {

}
#endif
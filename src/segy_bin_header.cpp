#include "segy_bin_header.h"
#include "segy_defines.h"
#include "data_conversion.h"
#include "utils.h"

#include <iostream>
#include <iterator>
#include <string>

using namespace std;

segy_bin_header::~segy_bin_header() {

}

segy_bin_header::segy_bin_header() {
	set_zero();
}

segy_bin_header::segy_bin_header(const vector<byte_t> &raw_data, endian_swap swap_endian) {
	initialize(raw_data, swap_endian);
}

void segy_bin_header::initialize(const vector<byte_t> &raw_data, endian_swap swap_endian) {
	if (raw_data.size() != segy_file::bin_header_size)
		throw runtime_error("segy_bin_header: binary file header has invalid size");

	f_is_segy_2 = (unsigned char)raw_data[300] != 0 ? true : false;

	int endian_swap = byte_to_int(&raw_data[96]);
	if (endian_swap == 16909060)
		f_endian_swap = endian_swap::none;
	else if (endian_swap == 67305985)
		f_endian_swap = endian_swap::reverse;
	else if (endian_swap == 33620995)
		f_endian_swap = endian_swap::pair;
	else
		f_endian_swap = swap_endian;

	f_job_id                        = byte_to_int(&raw_data[0], f_endian_swap);
	f_line_num                      = byte_to_int(&raw_data[4], f_endian_swap);
	f_reel_num                      = byte_to_int(&raw_data[8], f_endian_swap);
	f_traces_count                  = (int)byte_to_ushort(&raw_data[12], f_endian_swap);
	f_aux_traces_count              = (int)byte_to_ushort(&raw_data[14], f_endian_swap);
	f_sample_interval               = (double)byte_to_ushort(&raw_data[16], f_endian_swap);
	f_sample_interval_orig          = (double)byte_to_ushort(&raw_data[18], f_endian_swap);
	f_samples_count                 = (int)byte_to_ushort(&raw_data[20], f_endian_swap);
	f_samples_count_orig            = (int)byte_to_ushort(&raw_data[22], f_endian_swap);
	f_data_format                   = (segy_data_format)byte_to_ushort(&raw_data[24], f_endian_swap);

	f_ensemble_fold                 = (int)byte_to_ushort(&raw_data[26], f_endian_swap);
	f_sorting_code                  = (int)byte_to_ushort(&raw_data[28], f_endian_swap);
	f_vert_sum_code                 = (int)byte_to_ushort(&raw_data[30], f_endian_swap);
	f_sweep_fr_start                = (int)byte_to_ushort(&raw_data[32], f_endian_swap);
	f_sweep_fr_end                  = (int)byte_to_ushort(&raw_data[34], f_endian_swap);
	f_sweep_len                     = (int)byte_to_ushort(&raw_data[36], f_endian_swap);
	f_sweep_type                    = (int)byte_to_ushort(&raw_data[38], f_endian_swap);
	f_sweep_chanel_trcs_count       = (int)byte_to_ushort(&raw_data[40], f_endian_swap);
	f_sweep_trc_taper_len_start     = (int)byte_to_ushort(&raw_data[42], f_endian_swap);
	f_sweep_trc_taper_len_end       = (int)byte_to_ushort(&raw_data[44], f_endian_swap);
	f_taper_type                    = (int)byte_to_ushort(&raw_data[46], f_endian_swap);
	f_correlated_traces             = (int)byte_to_ushort(&raw_data[48], f_endian_swap);
	f_gain_recovered                = (int)byte_to_ushort(&raw_data[50], f_endian_swap);
	f_amplitude_rec_method          = (int)byte_to_ushort(&raw_data[52], f_endian_swap);
	f_measurement_system            = (int)byte_to_ushort(&raw_data[54], f_endian_swap);

	f_signal_polarity               = (int)byte_to_ushort(&raw_data[56], f_endian_swap);
	f_polarity_code                 = (int)byte_to_ushort(&raw_data[58], f_endian_swap);

	f_extended_traces_count         = byte_to_int(&raw_data[60], f_endian_swap);
	f_extended_aux_traces_count     = byte_to_int(&raw_data[64], f_endian_swap);
	f_extended_samples_count        = byte_to_int(&raw_data[68], f_endian_swap);
	f_extended_sample_interval      = byte_to_double(&raw_data[72], f_endian_swap);
	f_extended_sample_interval_orig = byte_to_double(&raw_data[80], f_endian_swap);
	f_extended_samples_count_orig   = byte_to_int(&raw_data[88], f_endian_swap);
	f_extended_ensemble_fold        = byte_to_int(&raw_data[92], f_endian_swap);

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

	f_is_same_for_file              = (int)byte_to_short(&raw_data[302], f_endian_swap);
	f_extended_headers_count        = (int)byte_to_short(&raw_data[304], f_endian_swap);
	f_max_add_trc_headers_count     = byte_to_int(&raw_data[306], f_endian_swap);
	f_time_basis                    = (int)byte_to_ushort(&raw_data[310], f_endian_swap);
	f_stream_traces_count           = byte_to_uint64(&raw_data[312], f_endian_swap);
	f_first_trace_offset            = byte_to_uint64(&raw_data[320], f_endian_swap);

    init_map();
}

void segy_bin_header::set_zero() {
	vector<byte_t> raw_data(segy_file::bin_header_size, (byte_t)0);
	initialize(raw_data, endian_swap::none);
}

seismic_variant_value segy_bin_header::get(const string &name) {
    if (f_map_need_update)
        init_map();
    auto it = f_fields.find(name);
    if (it != f_fields.cend())
        return it->second;
    else
        throw new invalid_argument("segy_bin_header: get: invalid field name");
}

map<string, seismic_variant_value> segy_bin_header::to_map() {
    if (f_map_need_update)
        init_map();
	return f_fields;
}

void segy_bin_header::init_map() {
    f_fields = {
    { "f_job_id",                   {f_job_id,                      seismic_data_type::INT} },
    { "f_line_num",                 {f_line_num,                    seismic_data_type::INT} },
    { "f_reel_num",                 {f_reel_num,                    seismic_data_type::INT} },
    { "f_traces_count",             {f_traces_count,                seismic_data_type::INT} },
    { "f_aux_traces_count",         {f_aux_traces_count,            seismic_data_type::INT} },
    { "f_sample_interval",          {f_sample_interval,             seismic_data_type::DOUBLE} },
    { "f_sample_interval_orig",     {f_sample_interval_orig,        seismic_data_type::DOUBLE} },
    { "f_samples_count",            {f_samples_count,               seismic_data_type::INT} },
    { "f_samples_count_orig",       {f_samples_count_orig,          seismic_data_type::INT} },
    { "f_data_format",              {(int)f_data_format,            seismic_data_type::INT} },
    { "f_ensemble_fold",            {f_ensemble_fold,               seismic_data_type::INT} },
    { "f_sorting_code",             {f_sorting_code,                seismic_data_type::INT} },
    { "f_vert_sum_code",            {f_vert_sum_code,               seismic_data_type::INT} },
    { "f_sweep_fr_start",           {f_sweep_fr_start,              seismic_data_type::INT} },
    { "f_sweep_fr_end",             {f_sweep_fr_end,                seismic_data_type::INT} },
    { "f_sweep_len",                {f_sweep_len,                   seismic_data_type::INT} },
    { "f_sweep_type",               {f_sweep_type,                  seismic_data_type::INT} },
    { "f_sweep_chanel_trcs_count",  {f_sweep_chanel_trcs_count,     seismic_data_type::INT} },
    { "f_sweep_trc_taper_len_start",{f_sweep_trc_taper_len_start,   seismic_data_type::INT} },
    { "f_sweep_trc_taper_len_end",  {f_sweep_trc_taper_len_end,     seismic_data_type::INT} },
    { "f_taper_type",               {f_taper_type,                  seismic_data_type::INT} },
    { "f_correlated_traces",        {f_correlated_traces,           seismic_data_type::INT} },
    { "f_gain_recovered",           {f_gain_recovered,              seismic_data_type::INT} },
    { "f_amplitude_rec_method",     {f_amplitude_rec_method,        seismic_data_type::INT} },
    { "f_measurement_system",       {f_measurement_system,          seismic_data_type::INT} },
    { "f_signal_polarity",          {f_signal_polarity,             seismic_data_type::INT} },
    { "f_polarity_code",            {f_polarity_code,               seismic_data_type::INT} },
    { "f_endian_swap",              {(int)f_endian_swap,            seismic_data_type::INT} },
    { "f_is_segy_2",                {(int)f_is_segy_2,              seismic_data_type::INT} },
    { "f_is_same_for_file",         {f_is_same_for_file,            seismic_data_type::INT} },
    { "f_extended_headers_count",   {f_extended_headers_count,      seismic_data_type::INT} },
    { "f_max_add_trc_headers_count",{f_max_add_trc_headers_count,   seismic_data_type::INT} },
    { "f_time_basis",               {f_time_basis,                  seismic_data_type::INT} },
    { "f_stream_traces_count",      {f_stream_traces_count,         seismic_data_type::UINT64} },
    { "f_first_trace_offset",       {f_first_trace_offset,          seismic_data_type::UINT64} }
    };

    f_map_need_update = false;
}

#ifdef PYTHON
void py_segy_bin_header_init(py::module &m,
	py::class_<segy_bin_header, shared_ptr<segy_bin_header>> &py_segy_bin_header) {

}
#endif
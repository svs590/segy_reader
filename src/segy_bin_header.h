#ifndef __SEGY_BIN_HEADER
#define __SEGY_BIN_HEADER

#include <map>
#include <string>
#include <vector>

#ifdef PYTHON
#include <pybind11/pybind11.h>

namespace py = pybind11;
#endif

#include "seismic_abstract_header.h"
#include "data_conversion.h"

class segy_bin_header : public seismic_abstract_header {
	std::map<std::string, int> fields;
public:
	segy_bin_header();
	segy_bin_header(const std::vector<byte_t> &raw_data, endian_swap swap_endian);
	~segy_bin_header();

	int job_id()								{ return f_job_id; }
	int line_num()								{ return f_line_num; }
	int reel_num()								{ return f_reel_num; }
	int traces_count()							{ return f_traces_count; }
	int aux_traces_count()						{ return f_aux_traces_count; }
	int sample_interval()						{ return f_sample_interval; }
	int sample_interval_orig()					{ return f_sample_interval_orig; }
	int samples_count()							{ return f_samples_count; }
	int samples_count_orig()					{ return f_samples_count_orig; }
	int data_format()							{ return f_data_format; }
	int ensemble_fold()							{ return f_ensemble_fold; }
	int sorting_code()							{ return f_sorting_code; }
	int vert_sum_code()							{ return f_vert_sum_code; }
	int sweep_fr_start()						{ return f_sweep_fr_start; }
	int sweep_fr_end()							{ return f_sweep_fr_end; }
	int sweep_len()								{ return f_sweep_len; }
	int sweep_type()							{ return f_sweep_type; }
	int sweep_chanel_trcs_count()				{ return f_sweep_chanel_trcs_count; }
	int sweep_trc_taper_len_start()				{ return f_sweep_trc_taper_len_start; }
	int sweep_trc_taper_len_end()				{ return f_sweep_trc_taper_len_end; }
	int taper_type()							{ return f_taper_type; }
	int correlated_traces()						{ return f_correlated_traces; }
	int gain_recovered()						{ return f_gain_recovered; }
	int amplitude_rec_method()					{ return f_amplitude_rec_method; }
	int measurement_system()					{ return f_measurement_system; }
	int signal_polarity()						{ return f_signal_polarity; }
	int polarity_code()							{ return f_polarity_code; }
	bool is_segy_2()							{ return f_is_segy_2; }
	bool is_same_for_file()						{ return f_is_same_for_file; }
	int extended_headers_count()				{ return f_extended_headers_count; }
	int max_add_trc_headers_count()				{ return f_max_add_trc_headers_count; }
	int time_basis()							{ return f_time_basis; }
	int64_t stream_traces_count()				{ return f_stream_traces_count; }
	int64_t first_trace_offset()				{ return f_first_trace_offset; }
	endian_swap endian()						{ return f_endian_swap; }

	void set_job_id(int val)					{ f_job_id = val; }
	void set_line_num(int val)					{ f_line_num = val; }
	void set_reel_num(int val)					{ f_reel_num = val; }
	void set_traces_count(int val)				{ f_traces_count = val; }
	void set_aux_traces_count(int val)			{ f_aux_traces_count = val; }
	void set_sample_interval(int val)			{ f_sample_interval = val; }
	void set_sample_interval_orig(int val)		{ f_sample_interval_orig = val; }
	void set_samples_count(int val)				{ f_samples_count = val; }
	void set_samples_count_orig(int val)		{ f_samples_count_orig = val; }
	void set_data_format(int val)				{ f_data_format = val; }
	void set_ensemble_fold(int val)				{ f_ensemble_fold = val; }
	void set_sorting_code(int val)				{ f_sorting_code = val; }
	void set_vert_sum_code(int val)				{ f_vert_sum_code = val; }
	void set_sweep_fr_start(int val)			{ f_sweep_fr_start = val; }
	void set_sweep_fr_end(int val)				{ f_sweep_fr_end = val; }
	void set_sweep_len(int val)					{ f_sweep_len = val; }
	void set_sweep_type(int val)				{ f_sweep_type = val; }
	void set_sweep_chanel_trcs_count(int val)	{ f_sweep_chanel_trcs_count = val; }
	void set_sweep_trc_taper_len_start(int val)	{ f_sweep_trc_taper_len_start = val; }
	void set_sweep_trc_taper_len_end(int val)	{ f_sweep_trc_taper_len_end = val; }
	void set_taper_type(int val)				{ f_taper_type = val; }
	void set_correlated_traces(int val)			{ f_correlated_traces = val; }
	void set_gain_recovered(int val)			{ f_gain_recovered = val; }
	void set_amplitude_rec_method(int val)		{ f_amplitude_rec_method = val; }
	void set_measurement_system(int val)		{ f_measurement_system = val; }
	void set_signal_polarity(int val)			{ f_signal_polarity = val; }
	void set_polarity_code(int val)				{ f_polarity_code = val; }
	void set_segy_2(bool val)					{ f_is_segy_2 = val; }
	void set_same_for_file(bool val)			{ f_is_same_for_file = val; }
	void extended_headers_count(int val)		{ f_extended_headers_count = val; }
	void max_add_trc_headers_count(int val)		{ f_max_add_trc_headers_count = val; }
	void time_basis(int val)					{ f_time_basis = val; }
	void stream_traces_count(int64_t val)		{ f_stream_traces_count = val; }
	void first_trace_offset(int64_t val)		{ f_first_trace_offset = val; }
	void set_endian(endian_swap val)			{ f_endian_swap = val; }


	//virtual std::pair<std::string, int> get(int index);
	//virtual int index_of(const std::string &name);
	virtual std::map<std::string, int64_t> to_map();

private:
	void initialize(const std::vector<byte_t> &raw_data, endian_swap swap_endian);
	void set_zero();

	int f_job_id;						// 4 bytes
	int f_line_num;						// 4 bytes
	int f_reel_num;						// 4 bytes
	int f_traces_count;					// 2 bytes
	int f_aux_traces_count;				// 2 bytes
	double f_sample_interval;			// 2 bytes
	double f_sample_interval_orig;		// 2 bytes
	int f_samples_count;				// 2 bytes
	int f_samples_count_orig;			// 2 bytes
	int f_data_format;					// 2 bytes
	int f_ensemble_fold;				// 2 bytes
	int f_sorting_code;					// 2 bytes
	int f_vert_sum_code;				// 2 bytes
	int f_sweep_fr_start;				// 2 bytes
	int f_sweep_fr_end;					// 2 bytes
	int f_sweep_len;					// 2 bytes
	int f_sweep_type;					// 2 bytes
	int f_sweep_chanel_trcs_count;		// 2 bytes
	int f_sweep_trc_taper_len_start;	// 2 bytes
	int f_sweep_trc_taper_len_end;		// 2 bytes
	int f_taper_type;					// 2 bytes
	int f_correlated_traces;			// 2 bytes
	int f_gain_recovered;				// 2 bytes
	int f_amplitude_rec_method;			// 2 bytes
	int f_measurement_system;			// 2 bytes
	int f_signal_polarity;				// 2 bytes
	int f_polarity_code;				// 2 bytes

	// If nonzero, rewriting original fields
	int f_extended_traces_count;		// 4 bytes
	int f_extended_aux_traces_count;	// 4 bytes
	int f_extended_samples_count;		// 4 bytes
	double f_extended_sample_interval;	// 8 bytes, IEEE double
	double f_extended_sample_interval_orig;// 8 bytes, IEEE double
	int f_extended_samples_count_orig;	// 4 bytes
	int f_extended_ensemble_fold;		// 4 bytes

	endian_swap f_endian_swap;			// 4 bytes, 67305985 do not swap, 33620995 do swap

	bool f_is_segy_2;					// 1 byte
	int f_is_same_for_file;				// 2 bytes
	int f_extended_headers_count;		// 2 bytes, -1 - variable count
	int f_max_add_trc_headers_count;	// 4 bytes
	int f_time_basis;					// 2 bytes
	uint64_t f_stream_traces_count;		// 8 bytes
	uint64_t f_first_trace_offset;		// 8 bytes, if nonzero overrides f_extended_headers_count
	
};

#ifdef PYTHON
void py_segy_bin_header_init(py::module &m,
	py::class_<segy_bin_header, std::shared_ptr<segy_bin_header>> &py_segy_bin_header);
#endif

#endif

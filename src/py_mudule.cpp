#ifdef PYTHON

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "seismic_data_provider.h"
#include "segy_reader.h"
#include "segy_bin_header.h"
#include "segy_header_map.h"


using namespace pybind11::literals;
namespace py = pybind11;

using namespace std;


PYBIND11_MODULE(segy_reader, m)
{
	m.doc() = "pybind11 segy_reader plugin for C API interface OpenSeaSeis library";

	// Объявление абстрактных классов и их наследников
	py::class_<seismic_data_provider, std::shared_ptr<seismic_data_provider>>
		data_provider(m, "data_provider");
	py::class_<segy_reader, std::shared_ptr<segy_reader>>
		py_segy_reader(m, "segy_reader", data_provider);
	py::class_<seismic_abstract_header, std::shared_ptr<seismic_abstract_header>>
		py_abstract_header(m, "abstract_header");
	py::class_<segy_bin_header, std::shared_ptr<segy_bin_header>>
		py_segy_bin_header(m, "segy_bin_header", py_abstract_header);
	py::class_<seismic_traceheader_field, std::shared_ptr<seismic_traceheader_field>>
		py_seismic_traceheader_field(m, "traceheader_field");
	py::class_<seismic_trace_header, std::shared_ptr<seismic_trace_header>>
		py_seismic_trace_header(m, "trace_header");
	py::class_<segy_trace_header, std::shared_ptr<segy_trace_header>>
		py_segy_trace_header(m, "segy_trace_header", py_seismic_trace_header);
	py::class_<seismic_trace, std::shared_ptr<seismic_trace>>
		py_seismic_trace(m, "trace");
	py::class_<seismic_header_map, shared_ptr<seismic_header_map>>
		py_header_map(m, "header_map");
	py::class_<segy_header_map, shared_ptr<segy_header_map>>
		py_segy_header_map(m, "segy_header_map", py_header_map);

	// Инициализация абстрактных классов и их наследников
	py_seismic_data_provider_init(m, data_provider);
	py_segy_reader_init(m, py_segy_reader);
	py_segy_abstract_header_init(m, py_abstract_header);
	py_seismic_trace_header_init(m, py_seismic_trace_header);
	py_seismic_trace_header_init(m, py_seismic_trace);
	py_seismic_header_map_init(m, py_header_map);
	py_segy_header_map_init(m, py_segy_header_map);
	
	// Прочие классы
	py_seismic_geometry_info_init(m);
	py_seismic_line_info_init(m);
	py_seismic_data_types_init(m);
	py_seismic_traceheader_field_init(m, py_seismic_traceheader_field);

	//py::class_<segy_reader, std::shared_ptr<segy_reader>>
	//			reader(m, "segy_reader", py_data_provider);
	//auto &py_segy_reader = py_segy_reader_init(m, py_data_provider);

	/*
	py::class_<segy_bin_header>(m, "segy_bin_header")
		.def_property_readonly("job_id", &segy_bin_header::jobID)
		.def_property_readonly("line_num", &segy_bin_header::lineNum)
		.def_property_readonly("reel_num", &segy_bin_header::reelNum)
		.def_property_readonly("num_traces", &segy_bin_header::numTraces)
		.def_property_readonly("num_aux_traces", &segy_bin_header::numAuxTraces)
		.def_property_readonly("sample_interval_us", &segy_bin_header::sampleIntUS)
		.def_property_readonly("sample_interval_orig_us", &segy_bin_header::sampleIntOrigUS)
		.def_property_readonly("num_samples", &segy_bin_header::numSamples)
		.def_property_readonly("num_samples_orig", &segy_bin_header::numSamplesOrig)
		.def_property_readonly("data_sample_format", &segy_bin_header::dataSampleFormat)
		.def_property_readonly("fold", &segy_bin_header::fold)
		.def_property_readonly("sort_code", &segy_bin_header::sortCode)
		.def_property_readonly("vert_sum_code", &segy_bin_header::vertSumCode)
		.def_property_readonly("sweep_freq_start", &segy_bin_header::sweepFreqStart)
		.def_property_readonly("sweep_freq_end", &segy_bin_header::sweepFreqEnd)
		.def_property_readonly("sweep_code", &segy_bin_header::sweepCode)
		.def_property_readonly("taper_type", &segy_bin_header::taperType)
		.def_property_readonly("correlated_traces", &segy_bin_header::correlatedTraces)
		.def_property_readonly("gain_recovered", &segy_bin_header::gainRecovered)
		.def_property_readonly("amp_recovery_method", &segy_bin_header::ampRecoveryMethod)
		.def_property_readonly("unit_system", &segy_bin_header::unitSystem)
		.def_property_readonly("polarity", &segy_bin_header::polarity)
		.def_property_readonly("vib_polarity_code", &segy_bin_header::vibPolarityCode)
		.def_property_readonly("revision_num", &segy_bin_header::revisionNum)
		.def_property_readonly("fixed_trace_length", &segy_bin_header::fixedTraceLengthFlag)
		.def_property_readonly("num_extended_blocks", &segy_bin_header::numExtendedBlocks)
		.def("to_dict", &segy_bin_header::to_map);
		*/
}

#endif
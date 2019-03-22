#ifdef PYTHON

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>


#include "segy_reader.h"
#include "segy_trace.h"
#include "segy_trace_header.h"
#include "utils.h"

using namespace pybind11::literals;
namespace py = pybind11;

using namespace std;


PYBIND11_MODULE(segy_reader, m)
{
	m.doc() = "pybind11 segy_reader plugin for C API interface OpenSeaSeis library";

	py::class_<segy_reader>(m, "segy_reader")
		.def(py::init<std::string, int, int, bool, bool, bool>(),
			py::arg("filename"), py::arg("trace_buffer"), py::arg("header_map_type"),
			py::arg("reverse_byte_order_data"), py::arg("reverse_byte_order_header"), py::arg("autoscale_hdrs"))
		.def("close_file", &segy_reader::closeFile)
		.def_property_readonly("sample_byte_size", &segy_reader::sampleByteSize)
		.def_property_readonly("traces_count", &segy_reader::tracesCount)
		.def_property_readonly("samples_count", &segy_reader::samplesCount)
		.def_property_readonly("headers_count", &segy_reader::headersCount)
		.def_property_readonly("sample_interval", &segy_reader::sampleInterval)
		.def_property_readonly("bin_header_str", [](segy_reader &obj) {
				return utf8_to_unicode(obj.charBinHeader());
			})
		.def_property_readonly("header_map", &segy_reader::traceHeaderMap)
		.def_property_readonly("bin_header", &segy_reader::binHeader)
		.def_property_readonly("header_val", &segy_reader::headerValue<float>)
		.def("sort", &segy_reader::preprocessing)
		.def("move", (void (segy_reader::*)(int,int))&segy_reader::moveToTrace)
		.def("get_trace", &segy_reader::py_get_trace)
		.def("iline", &segy_reader::iline)
		.def("xline", &segy_reader::xline);

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

	py::class_<segy_header_map>(m, "segy_header_map")
		.def("add",
		(void(segy_header_map::*)(
				std::string, int, int, cseis_geolib::type_t, std::string
				)
			)&segy_header_map::addHeader
		)
		.def("remove", (void(segy_header_map::*)(int))&segy_header_map::remove)
		.def("remove", (void(segy_header_map::*)(string))&segy_header_map::remove)
		.def("remove_all", &segy_header_map::removeAll)
		.def("index_of", &segy_header_map::getIndexOf)
		.def("contains", &segy_header_map::contains)
		.def("index_of", &segy_header_map::getIndexOf)
		.def("header_info", (segy_traceheader_field(segy_header_map::*)(int))&segy_header_map::headerInfo)
		.def("header_info", (segy_traceheader_field(segy_header_map::*)(string))&segy_header_map::headerInfo)
		.def("count", &segy_header_map::count)
		.def("map_id", &segy_header_map::mapId)
		.def("to_dict", [](segy_header_map &obj) {
			map<string, py::dict> res;
			for (int i = 0; i < obj.count(); ++i) {
				auto info = obj.headerInfo(i);
				py::dict dict_info(
					"position"_a	= info.getByteLoc(),
					"size"_a		= info.getByteSize(),
					"description"_a	= info.getDescription()
				);
				res[info.getName()] = dict_info;
			}
			return res;
		});

	py::class_<segy_trace_header>(m, "segy_trace_header")
		.def(py::init<>())
		.def_property_readonly("count", &segy_trace_header::count,
			"Count of fields in current header"
		)
		.def("description", &segy_trace_header::description,
			py::arg("field_index"),
			"Field description"
		)
		.def("name", &segy_trace_header::name,
			py::arg("field_index"),
			"Field name"
			)
		.def("index_of", &segy_trace_header::index_of,
			py::arg("field_name"),
			"Index of field with name=field_name"
		)
		.def("type", &segy_trace_header::type,
			py::arg("field_index"),
			"Value type for field"
		)
		.def("get_i", &segy_trace_header::get<int>,
			py::arg("field_index"),
			"Get int value of field"
		)
		.def("get_d", &segy_trace_header::get<double>,
			py::arg("field_index"),
			"Get double value of field"
		)
		.def("to_dict", &segy_trace_header::to_dict);

	py::class_<segy_trace>(m, "segy_trace")
		.def(py::init<const segy_trace_header &, const std::vector<float> &>(),
			py::arg("trace_header"), py::arg("trace_data")
		)
		.def(py::init<const segy_trace &>(),
			py::arg("trace")
		)
		.def_property("data", &segy_trace::get_data, &segy_trace::set_data,
			"Trace data array"
		)
		.def_property("header", &segy_trace::get_header, &segy_trace::set_header,
			"Trace header"
		);
}

#endif
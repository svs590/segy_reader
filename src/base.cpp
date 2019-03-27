#include <any>

#include "seismic_data_provider.h"

using namespace std;

#ifdef PYTHON
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

py::object seismic_trace_header::py_get(int index) {
	py::object value;
	auto any_pair = get(index);
	switch (any_pair.second)
	{
	case seismic_data_type::INT:
		value = py::int_(any_cast<int>(any_pair.first));
		break;
	case seismic_data_type::FLOAT:
		value = py::float_(any_cast<float>(any_pair.first));
		break;
	case seismic_data_type::DOUBLE:
		value = py::float_(any_cast<double>(any_pair.first));
		break;
	default:
		// TODO!!!
		break;
	}
	return value;
}

map<string, py::object> seismic_trace_header::to_dict() {
	map<string, py::object> res;
		for (int i = 0; i < count(); ++i) {
			auto n = name(i);
			auto t = type(i);
			py::object v = py_get(i);	
			res[n] = v;
		}
		return res;
}

map<string, pair<any, seismic_data_type>> seismic_trace_header::to_map() {
	map<string, pair<any, seismic_data_type>> res;
	for (int i = 0; i < count(); ++i) {
		std::string f_name = name(i);
		res[f_name] = get(i);
	}
	return res;
}

void py_seismic_data_provider_init(py::module &m,
	py::class_<seismic_data_provider, std::shared_ptr<seismic_data_provider>> &data_provider) {
	
	data_provider.def("text_header", &seismic_data_provider::text_header,
		"Returns textural file header");
	data_provider.def("bin_header", &seismic_data_provider::bin_header,
		"Returns binary file header");
	data_provider.def("header_map", &seismic_data_provider::header_map,
		"Returns header mapping wich uses to parse trace headers");
	data_provider.def("trace_header", &seismic_data_provider::trace_header,
		py::arg("trace_index"),
		"Returns trace header by trace index");
	data_provider.def("get_trace", &seismic_data_provider::get_trace,
		py::arg("trace_index"),
		"Returns trace by trace index");
	data_provider.def("preprocessing", &seismic_data_provider::preprocessing,
		"Determine how file sorted and map some necessary information. "
		"This method will be called implicitly before seismic_line or geometry downloading");
	data_provider.def("get_geometry", &seismic_data_provider::get_geometry,
		"Returns all seismic line information");
	data_provider.def("get_traces", &seismic_data_provider::get_traces,
		py::arg("line_info"),
		"Returns all traces in seismic line"
	);
	data_provider.def("get_headers", &seismic_data_provider::get_headers,
		py::arg("line_info"),
		"Returns trace headers in seismic line"
	);
	data_provider.def("close", &seismic_data_provider::close,
		"Close file");
	data_provider.def("traces_count", &seismic_data_provider::traces_count,
		"Returns traces count in file");
	data_provider.def("samples_count", &seismic_data_provider::samples_count,
		"Returns samples count in trace");
	data_provider.def("sample_interval", &seismic_data_provider::sample_interval,
		"Returns sampling interval (usually in ms)");
}

void py_seismic_header_map_init(py::module &m) {

	py::enum_<header_map_type>(m, "header_map_type")
		.value("CUSTOM",	header_map_type::CUSTOM)
		.value("STANDARD",	header_map_type::STANDARD)
		.value("OBC",		header_map_type::OBC)
		.value("SEND",		header_map_type::SEND)
		.value("ARMSS",		header_map_type::ARMSS)
		.value("PSEGY",		header_map_type::PSEGY)
		.value("NODE_OLD",	header_map_type::NODE_OLD)
		.value("NODE",		header_map_type::NODE)
		.value("SU",		header_map_type::SU)
		.value("SU_ONLY",	header_map_type::SU_ONLY)
		.value("SU_BOTH",	header_map_type::SU_BOTH)
		.export_values();

	py::class_<seismic_header_map, std::shared_ptr<seismic_header_map>>
		header_map(m, "header_map");

	header_map.def("set_field", &seismic_header_map::add_field,
		py::arg("field_name"),
		py::arg("byte_location"),
		py::arg("byte_size"),
		py::arg("data_type"),
		py::arg("description") = "",
		"Add field in trace header map"
		);
	header_map.def("set_field", &seismic_header_map::set_field,
		py::arg("traceheader_field"),
		"Replace existance header field or add new if there is not "
		"field with the same name"
	);
	header_map.def("remove", (void(seismic_header_map::*)(int))&seismic_header_map::remove,
		py::arg("field_index"),
		"Remove header field by index"
		);
	header_map.def("remove", (void(seismic_header_map::*)(const std::string &))&seismic_header_map::remove,
		py::arg("field_name"),
		"Remove header field by name"
	);
	header_map.def("clear", &seismic_header_map::clear,
		"Remove all headers field from map");
	header_map.def("index_of", &seismic_header_map::index_of,
		py::arg("field_name"),
		"Returns field index by name"
		);
	header_map.def("contains", &seismic_header_map::contains,
		py::arg("field_name"),
		"Returns field index by name if map contains field, return -1 otherwise"
	);
	header_map.def("get_field", 
		(std::shared_ptr<seismic_traceheader_field>(seismic_header_map::*)(int)const)&seismic_header_map::get_field,
		py::arg("field_index"),
		"Returns field by index"
	);
	header_map.def("get_field",
		(std::shared_ptr<seismic_traceheader_field>(seismic_header_map::*)(const std::string &)const)&seismic_header_map::get_field,
		py::arg("field_name"),
		"Returns field by name"
	);
	header_map.def_property_readonly("count", &seismic_header_map::count,
		"Returns fields count"
	);
	header_map.def_property_readonly("type", &seismic_header_map::type,
		"Returns map type"
	);
	header_map.def("to_dict", &seismic_header_map::to_map);
}

void py_seismic_geometry_info_init(py::module &m) {
	py::class_<seismic_geometry_info, std::shared_ptr<seismic_geometry_info>>
		py_geometry_info(m, "geometry_info");

	py_geometry_info.def(py::init<std::vector<seismic_line_info>>(),
		py::arg("line_info_array")
	);
	py_geometry_info.def("get_lines", &seismic_geometry_info::get_lines,
		"Returns array of line information");
}

void py_seismic_line_info_init(py::module &m) {
	py::enum_<seismic_line_info::seismic_line_type>(m, "line_type")
		.value("iline",	seismic_line_info::seismic_line_type::iline)
		.value("xline",	seismic_line_info::seismic_line_type::xline)
		.value("none",	seismic_line_info::seismic_line_type::none)
		.export_values();

	py::class_<seismic_line_info, std::shared_ptr<seismic_line_info>>
		line_info(m, "line_info");
	line_info.def(py::init<
			std::string, 
			std::pair<float, float>, 
			std::pair<float, float>,
			int,
			seismic_line_info::seismic_line_type
		>(),
		py::arg("line_name"), 
		py::arg("start_point"),
		py::arg("end_point"),
		py::arg("line_no"),
		py::arg("type")
	);
	line_info.def_property_readonly("start",	&seismic_line_info::get_start);
	line_info.def_property_readonly("end",		&seismic_line_info::get_end);
	line_info.def_property_readonly("name",		&seismic_line_info::get_name);
}

void py_segy_abstract_header_init(py::module &m,
	py::class_<seismic_abstract_header, std::shared_ptr<seismic_abstract_header>> &py_abstract_header) {
	
	py_abstract_header.def("get_field", &seismic_abstract_header::get,
		py::arg("index"),
		"Returns pair (field name, value) by index");
	py_abstract_header.def("index_of", &seismic_abstract_header::index_of,
		py::arg("field_name"),
		"Returns field index by name if header contains field, return -1 otherwise");
	py_abstract_header.def("to_dict", &seismic_abstract_header::to_map,
		"Returns dict kind of {name : (byte_position, byte_size, data_type, description)}");
}

void py_seismic_data_types_init(py::module &m) {
	py::enum_<seismic_data_type>(m, "seismic_data_type")
		.value("UNKNOWN",	seismic_data_type::UNKNOWN)
		.value("EMPTY",		seismic_data_type::EMPTY)
		.value("INT",		seismic_data_type::INT)
		.value("FLOAT",		seismic_data_type::FLOAT)
		.value("DOUBLE",	seismic_data_type::DOUBLE)
		.value("CHAR",		seismic_data_type::CHAR)
		.value("STRING",	seismic_data_type::STRING)
		.value("INT64",		seismic_data_type::INT64)
		.value("SHORT",		seismic_data_type::SHORT)
		.value("USHORT",	seismic_data_type::USHORT)
		.export_values();
}

void py_seismic_traceheader_field_init(py::module &m,
	py::class_<seismic_traceheader_field, std::shared_ptr<seismic_traceheader_field>> &py_traceheader_field) {

	py_traceheader_field.def("byte_loc", &seismic_traceheader_field::byte_loc,
		"Returns field byte location");
	py_traceheader_field.def("byte_size", &seismic_traceheader_field::byte_size,
		"Returns field byte size");
	py_traceheader_field.def("name", &seismic_traceheader_field::name,
		"Returns field name");
	py_traceheader_field.def("description", &seismic_traceheader_field::description,
		"Returns field description");
	py_traceheader_field.def("type", &seismic_traceheader_field::type_out,
		"Returns field data_type");
	py_traceheader_field.def("set_byte_loc", &seismic_traceheader_field::set_byte_loc,
		"Set field byte location");
	py_traceheader_field.def("set_byte_size", &seismic_traceheader_field::set_byte_size,
		"Set field byte size");
	py_traceheader_field.def("set_type", &seismic_traceheader_field::set_type_out,
		"Set field data_type");
}

void py_seismic_trace_header_init(py::module &m,
	py::class_<seismic_trace_header, std::shared_ptr<seismic_trace_header>> &py_seismic_trace_header) {

	py_seismic_trace_header.def("count", &seismic_trace_header::count,
		"Returns fields count in seismic header");
	py_seismic_trace_header.def("description", &seismic_trace_header::description,
		py::arg("field_index"),
		"Returns field description"
	);
	py_seismic_trace_header.def("name", &seismic_trace_header::name,
		py::arg("field_index"),
		"Returns field name"
	);
	py_seismic_trace_header.def("index_of", &seismic_trace_header::index_of,
		py::arg("field_name"),
		"Returns field index by name"
	);
	py_seismic_trace_header.def("type", &seismic_trace_header::type,
		py::arg("field_index"),
		"Returns field data_type"
	);
	py_seismic_trace_header.def("value", &seismic_trace_header::py_get,
		py::arg("field_index"),
		"Returns field data_type"
	);
	py_seismic_trace_header.def("to_dict", &seismic_trace_header::to_dict);
}

void py_seismic_trace_header_init(py::module &m,
	py::class_<seismic_trace, std::shared_ptr<seismic_trace>> &py_trace) {

	py_trace.def("get_header", &seismic_trace::get_header);
	py_trace.def("set_header", &seismic_trace::set_header);
	py_trace.def("get_data", &seismic_trace::get_data);
	py_trace.def("set_data", &seismic_trace::set_data);
}

#endif
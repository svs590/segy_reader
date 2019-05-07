#include <any>

#include "seismic_data_provider.h"

using namespace std;

#ifdef PYTHON
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/eigen.h>


void py_seismic_data_provider_init(py::module &m,
	py::class_<seismic_data_provider, shared_ptr<seismic_data_provider>> &data_provider) {
	
	data_provider.def("text_header", [](seismic_data_provider &obj) {
			return utf8_to_unicode(obj.text_header());
		},
		"Returns textural file header"
	);
	data_provider.def("bin_header", &seismic_data_provider::bin_header,
		"Returns binary file header");
	data_provider.def("header_map", &seismic_data_provider::header_map,
		"Returns copy of header map wich uses to parse trace headers");
	data_provider.def("set_header_map", &seismic_data_provider::set_header_map,
		py::arg("header_map"));
	data_provider.def("header", &seismic_data_provider::header,
		py::arg("trace_index"),
		"Returns trace header by trace index");
	data_provider.def("trace", &seismic_data_provider::trace,
		py::arg("trace_index"),
		"Returns trace by trace index");
	data_provider.def("preprocessing", &seismic_data_provider::preprocessing,
		"Determine how file sorted and map some necessary information. "
		"This method will be called implicitly before seismic_line or "
		"f_geometry downloading");
	data_provider.def("geometry", &seismic_data_provider::geometry,
		"Returns all seismic line information");
	data_provider.def("traces", 
		(vector<shared_ptr<seismic_trace>>(seismic_data_provider::*)(seismic_line_info))
		&seismic_data_provider::traces,
		py::arg("line_info"),
		"Returns all traces in seismic line"
	);
	data_provider.def("headers", 
		(vector<shared_ptr<seismic_trace_header>>(seismic_data_provider::*)(seismic_line_info))
		&seismic_data_provider::headers,
		py::arg("line_info"),
		"Returns trace headers in seismic line"
	);
	data_provider.def("traces",
		(vector<shared_ptr<seismic_trace>>(seismic_data_provider::*)(int, int))
		&seismic_data_provider::traces,
		py::arg("start_trace"),
		py::arg("end_trace"),
		"Returns traces from start_trace to end_trace"
	);
	data_provider.def("headers",
		(vector<shared_ptr<seismic_trace_header>>(seismic_data_provider::*)(int, int))
		&seismic_data_provider::headers,
		py::arg("start_trace"),
		py::arg("end_trace"),
		"Returns trace headers from start_trace to end_trace"
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

void py_seismic_header_map_init(py::module &m,
	py::class_<seismic_header_map, shared_ptr<seismic_header_map>> &py_header_map) {

	py::enum_<header_map_type>(m, "header_map_type")
		.value("CUSTOM",	header_map_type::CUSTOM)
		.value("STANDARD",	header_map_type::STANDARD)
		.export_values();

	py_header_map.def("set_field", &seismic_header_map::set_field,
		py::arg("field_name"),
		py::arg("byte_location"),
		py::arg("byte_size"),
		py::arg("data_type"),
		py::arg("description") = "",
		"Add field in trace header map"
		);
	py_header_map.def("remove", &seismic_header_map::remove,
		py::arg("field_name"),
		"Remove header field by name"
	);
	py_header_map.def("clear", &seismic_header_map::clear,
		"Remove all headers field from map");
	py_header_map.def("contains", &seismic_header_map::contains,
		py::arg("field_name"),
		"Returns field index by name if map contains field, return -1 otherwise"
	);
	py_header_map.def("get_field",
		(std::pair<std::string, seismic_header_map::header_field_info>
        (seismic_header_map::*)(int)const)&seismic_header_map::get_field,
		py::arg("field_index"),
		"Returns field by index"
	);
	py_header_map.def("get_field",
		(seismic_header_map::header_field_info
			(seismic_header_map::*)(const string &)const)&seismic_header_map::get_field,
		py::arg("field_name"),
		"Returns field by name"
	);
	py_header_map.def_property_readonly("count", &seismic_header_map::count,
		"Returns fields count"
	);
	py_header_map.def_property_readonly("type", &seismic_header_map::type,
		"Returns map type"
	);
    py_header_map.def("to_dict", [](const seismic_header_map &map) {
        auto tmp = map.to_map();
        std::map<string, tuple<int, int, seismic_data_type, string>> res;
        for (auto &it : tmp)
            res[it.first] = it.second;
        return res;
    });
    py_header_map.def("from_dict", [] (
            seismic_header_map &header_map,
            std::map<string, tuple<int, int, seismic_data_type, string>> map
        ) {
        seismic_header_map::map_storage storage;
        for (auto &it : map)
            storage.push_back({ it.first, it.second });
        header_map.from_map(storage);
    });
}

void py_seismic_geometry_info_init(py::module &m) {
	py::class_<seismic_geometry_info, shared_ptr<seismic_geometry_info>>
		py_geometry_info(m, "geometry_info");

    py::enum_<seismic_geometry_info::seismic_geometry_type>(m, "geometry_type")
        .value("slice_2D", seismic_geometry_info::seismic_geometry_type::g2D)
        .value("volume_3D", seismic_geometry_info::seismic_geometry_type::g3D)
        .value("unknown", seismic_geometry_info::seismic_geometry_type::unknown)
        .export_values();

	py_geometry_info.def(
        py::init<
            const vector<seismic_line_info>&, 
            const vector<seismic_line_info>&, 
            seismic_geometry_info::seismic_geometry_type
        >(),
		py::arg("iline_info_array"),
        py::arg("crossline_info_array"),
        py::arg("geometry_type")
	);
	py_geometry_info.def("lines", 
        (std::vector<seismic_line_info>
            (seismic_geometry_info::*)())&seismic_geometry_info::lines,
		"Returns array of line information"
    );
    py_geometry_info.def("lines",
        (std::vector<seismic_line_info>
            (seismic_geometry_info::*)(seismic_line_info::seismic_line_type)
        )&seismic_geometry_info::lines,
        "Returns array of line information"
    );
    py_geometry_info.def("type",
        &seismic_geometry_info::geometry_type,
        "Returns geometry type: 2D, 3D or unknown"
    );
}

void py_seismic_line_info_init(py::module &m) {
	py::enum_<seismic_line_info::seismic_line_type>(m, "line_type")
		.value("iline",	seismic_line_info::seismic_line_type::iline)
		.value("xline",	seismic_line_info::seismic_line_type::xline)
		.value("none",	seismic_line_info::seismic_line_type::none)
		.export_values();

	py::class_<seismic_line_info, shared_ptr<seismic_line_info>>
		line_info(m, "line_info");
	line_info.def(py::init<
			string, 
			pair<float, float>, 
			pair<float, float>,
			int,
			seismic_line_info::seismic_line_type
		>(),
		py::arg("line_name"), 
		py::arg("start_point"),
		py::arg("end_point"),
		py::arg("line_no"),
		py::arg("type")
	);
	line_info.def_property_readonly("start_point",	&seismic_line_info::start_point);
	line_info.def_property_readonly("end_point",	&seismic_line_info::end_point);
	line_info.def_property_readonly("name",		    &seismic_line_info::name);
}

void py_segy_abstract_header_init(py::module &m,
	py::class_<seismic_abstract_header, 
		shared_ptr<seismic_abstract_header>> &py_abstract_header) {
	
	py_abstract_header.def("get_field", &seismic_abstract_header::get,
		py::arg("name"),
		"Returns pair (field name, value) by name");
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

void py_seismic_trace_header_init(py::module &m,
	py::class_<seismic_trace_header, 
		shared_ptr<seismic_trace_header>> &py_seismic_trace_header) {

	py_seismic_trace_header.def("count", &seismic_trace_header::count,
		"Returns fields count in seismic header");

	py_seismic_trace_header.def("index_of", &seismic_trace_header::contains,
		py::arg("field_name"),
		"Returns field index by name"
	);
	py_seismic_trace_header.def("type", &seismic_trace_header::type,
		py::arg("field_index"),
		"Returns field data_type"
	);
	py_seismic_trace_header.def("value", &seismic_trace_header::get,
		py::arg("field_name"),
		"Returns field data_type"
	);
    py_seismic_trace_header.def("iline", &seismic_trace_header::iline);
    py_seismic_trace_header.def("crossline", &seismic_trace_header::crossline);
    py_seismic_trace_header.def("X", &seismic_trace_header::X);
    py_seismic_trace_header.def("Y", &seismic_trace_header::Y);
	py_seismic_trace_header.def("to_dict", &seismic_trace_header::to_map);
}

void py_seismic_trace_init(py::module &m,
	py::class_<seismic_trace, shared_ptr<seismic_trace>> &py_trace) {

	py_trace.def("header", &seismic_trace::header);
	py_trace.def("set_header", &seismic_trace::set_header);
	py_trace.def("data", &seismic_trace::data);
	py_trace.def("set_data", &seismic_trace::set_data);

    py_trace.def("iline", &seismic_trace::iline);
    py_trace.def("crossline", &seismic_trace::crossline);
    py_trace.def("X", &seismic_trace::X);
    py_trace.def("Y", &seismic_trace::Y);
}

#endif
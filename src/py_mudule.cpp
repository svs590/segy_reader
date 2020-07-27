#ifdef PYTHON

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "seismic_data_provider.h"
#include "segy_reader.h"
#include "segy_writer.h"
#include "segy_text_header.h"
#include "segy_bin_header.h"
#include "segy_header_map.h"


using namespace pybind11::literals;
namespace py = pybind11;

using namespace std;


PYBIND11_MODULE(seismo_reader, m)
{
	m.doc() = "Python segy_reader plugin";

	// Объявление абстрактных классов и их наследников
	py::class_<seismic_data_provider, std::shared_ptr<seismic_data_provider>>
		data_provider(m, "data_provider");
	py::class_<segy_reader, std::shared_ptr<segy_reader>>
		py_segy_reader(m, "segy_reader", data_provider);
    py::class_<segy_writer, std::shared_ptr<segy_writer>>
        py_segy_writer(m, "segy_writer");
	py::class_<seismic_abstract_header, std::shared_ptr<seismic_abstract_header>>
		py_abstract_header(m, "abstract_header");
    py::class_<segy_text_header, std::shared_ptr<segy_text_header>>
        py_segy_text_header(m, "segy_text_header", py_abstract_header);
	py::class_<segy_bin_header, std::shared_ptr<segy_bin_header>>
		py_segy_bin_header(m, "segy_bin_header", py_abstract_header);
	py::class_<seismic_trace_header, std::shared_ptr<seismic_trace_header>>
		py_seismic_trace_header(m, "header");
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
    py_segy_writer_init(m, py_segy_writer);
	py_segy_abstract_header_init(m, py_abstract_header);
    py_segy_text_header_init(m, py_segy_text_header);
    py_segy_bin_header_init(m, py_segy_bin_header);
	py_seismic_trace_header_init(m, py_seismic_trace_header);
	py_seismic_trace_init(m, py_seismic_trace);
	py_seismic_header_map_init(m, py_header_map);
	py_segy_header_map_init(m, py_segy_header_map);
	py_segy_trace_header_init(m, py_segy_trace_header);

	
	// Прочие классы
	py_seismic_geometry_info_init(m);
	py_seismic_line_info_init(m);
	py_seismic_data_types_init(m);

}

#endif
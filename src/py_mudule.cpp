#ifdef PYTHON

#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "segy_reader.h"

using namespace pybind11::literals;
namespace py = pybind11;

PYBIND11_MODULE(segy_reader, m)
{
	m.doc() = "pybind11 segy_reader plugin for C API interface OpenSeaSeis library";

	py::class_<segy_reader>(m, "segy_reader")
		.def(py::init<std::string, int, int, bool, bool, bool>(),
			py::arg("filename"), py::arg("trace_buffer"), py::arg("header_map_type"),
			py::arg("reverse_byte_order_data"), py::arg("reverse_byte_order_header"), py::arg("autoscale_hdrs"))
		.def_property_readonly("close_file", &segy_reader::closeFile)
		.def_property_readonly("sample_byte_size", &segy_reader::sampleByteSize)
		.def_property_readonly("traces_count", &segy_reader::tracesCount)
		.def_property_readonly("samples_count", &segy_reader::samplesCount)
		.def_property_readonly("headers_count", &segy_reader::headersCount)
		.def_property_readonly("sample_interval", &segy_reader::sampleInterval)
		.def_property_readonly("bin_header_str", &segy_reader::charBinHeader)
		.def_property_readonly("bin_header", &segy_reader::traceHeaderMap)
		.def_property_readonly("header_map", &segy_reader::binHeader)
		.def_property_readonly("header_val", &segy_reader::headerValue<float>)
		.def("move", (void(int,int))&segy_reader::moveToTrace)
		;

	py::class_<Ellipsoid<float>>(m, "Ellipsoid")
		.def(py::init<double, double, double, double, double, double>(),
			py::arg("R1"), py::arg("R2"), py::arg("R3"),
			py::arg("Azimut") = 0, py::arg("Dip") = 0, py::arg("Rotation") = 0)
		.def_readwrite("R1", &Ellipsoid<float>::R1)
		.def_readwrite("R2", &Ellipsoid<float>::R2)
		.def_readwrite("R3", &Ellipsoid<float>::R3)
		.def_readwrite("Direction1", &Ellipsoid<float>::Direction1)
		.def_readwrite("Direction2", &Ellipsoid<float>::Direction2)
		.def_readwrite("Direction3", &Ellipsoid<float>::Direction3);

	m.def("run", &run);
}

#endif
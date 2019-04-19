#pragma once

#include <string>
#include <map>
#include <variant>

#include "data_types.h"

#ifdef PYTHON
#include <pybind11/pybind11.h>

namespace py = pybind11;
#endif


class seismic_abstract_header {
public:
	virtual seismic_variant_value get(const std::string &name) = 0;
	virtual std::map<std::string, seismic_variant_value> to_map() = 0;
};

#ifdef PYTHON
void py_segy_abstract_header_init(py::module &m,
	py::class_<seismic_abstract_header, std::shared_ptr<seismic_abstract_header>> &py_abstract_header);
#endif

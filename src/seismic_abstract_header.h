#ifndef __SEISMIC_ABSTRACT_HEADER
#define __SEISMIC_ABSTRACT_HEADER

#include <string>
#include <map>

#ifdef PYTHON
#include <pybind11/pybind11.h>

namespace py = pybind11;
#endif


class seismic_abstract_header {
public:
	virtual std::pair<std::string, int> get(int index) = 0;
	virtual int index_of(const std::string &name) = 0;
	virtual std::map<std::string, int> to_map() = 0;
};

#ifdef PYTHON
void py_segy_abstract_header_init(py::module &m,
	py::class_<seismic_abstract_header, std::shared_ptr<seismic_abstract_header>> &py_abstract_header);
#endif

#endif

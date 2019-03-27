#ifndef __SEISMIC_TRACE_HEADER
#define __SEISMIC_TRACE_HEADER

#include <string>
#include <any>
#include <map>

#ifdef PYTHON
#include <pybind11/pybind11.h>
#endif

#include "obj_base.h"
#include "data_types.h"


class seismic_trace_header : public obj_base {
public:
	virtual int						count() const = 0;
	virtual std::string				description(int index) const = 0;
	virtual std::string				name(int index) const = 0;
	virtual int						index_of(const std::string &name) const = 0;
	virtual seismic_data_type		type(int index) const = 0;

	virtual std::pair<std::any, seismic_data_type> get(int index) const = 0;
	virtual void set_field(int index, std::pair<std::any, seismic_data_type>) = 0;

	//Not implemented
	virtual int						byte_loc(int index) const = 0;
	virtual int						byte_pos(int index) const = 0;

	std::map<std::string, std::pair<std::any, seismic_data_type>> to_map();

#ifdef PYTHON
	std::map<std::string, py::object> to_dict();
	py::object py_get(int index);
#endif
};

#ifdef PYTHON
void py_seismic_trace_header_init(py::module &m,
	py::class_<seismic_trace_header, std::shared_ptr<seismic_trace_header>> &py_segy_reader);
#endif

#endif

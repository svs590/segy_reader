#ifndef __SEISMIC_TRACE_HEADER
#define __SEISMIC_TRACE_HEADER

#include <string>
#include <any>

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
	virtual void set(int index, std::pair<std::any, seismic_data_type>) = 0;

	//Not implemented
	virtual int						byte_loc(int index) const = 0;
	virtual int						byte_pos(int index) const = 0;

#ifdef PYTHON
	virtual std::map<std::string, py::object> to_dict();
#endif
};

#endif

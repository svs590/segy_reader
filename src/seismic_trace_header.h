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

	virtual std::any get(int index) const = 0;
	virtual std::any set(int index) = 0;

	//Not implemented
	virtual int						byte_loc(int index) const = 0;
	virtual int						byte_pos(int index) const = 0;
	
protected:
	virtual int			get_int(int index) const = 0;
	virtual float		get_float(int index) const = 0;
	virtual double		get_double(int index) const = 0;
	virtual char		get_char(int index) const = 0;
	virtual std::string get_str(int index) const = 0;

#ifdef PYTHON
	virtual std::map<std::string, py::object> to_dict();
#endif
};

#endif

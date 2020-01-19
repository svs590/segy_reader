#ifndef __SEISMIC_HEADER_INFO
#define __SEISMIC_HEADER_INFO

#include <string>

#ifdef PYTHON
#include <pybind11/pybind11.h>

namespace py = pybind11;
#endif

#include "obj_base.h"
#include "data_types.h"


class seismic_traceheader_field : public obj_base {
public:
	virtual int byte_loc() = 0;
	virtual int byte_size() = 0;
	virtual std::string name() = 0;
	virtual std::string description() = 0;
	virtual seismic_data_type type_in() = 0;
	virtual seismic_data_type type_out() = 0;

	virtual void set_byte_loc(int) = 0;
	virtual void set_byte_size(int byteSize) = 0;
	virtual void set_type_in(seismic_data_type type) = 0;
	virtual void set_type_out(seismic_data_type type) = 0;
};

#ifdef PYTHON
void py_seismic_traceheader_field_init(py::module &m,
	py::class_<seismic_traceheader_field, std::shared_ptr<seismic_traceheader_field>> &py_traceheader_field);
#endif

#endif

#ifndef __SEGY_TRACE_HEADER
#define __SEGY_TRACE_HEADER

#include <string>
#include <exception>
#include <memory>
#include <map>

#include "geolib_defines.h"

#include "seismic_trace_header.h"

#ifdef PYTHON
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
#endif

class segy_trace_header : public seismic_trace_header {
	std::shared_ptr<void> obj;
public:
	segy_trace_header() {}
	segy_trace_header(const void *const native_header);
	segy_trace_header(const segy_trace_header &header);
	~segy_trace_header() {}
	segy_trace_header& operator=(const void *obj);
	segy_trace_header& operator=(const segy_trace_header &obj);

	virtual int						count() const;
	virtual std::string				description(int index) const;
	virtual std::string				name(int index) const;
	virtual int						index_of(const std::string &name) const;
	virtual seismic_data_type		type(int index) const;

	//Not implemented
	virtual int						byte_loc(int index) const {
		throw std::runtime_error("Method not implemented"); 
	}
	virtual int						byte_pos(int index) const {
		throw std::runtime_error("Method not implemented"); 
	}

	virtual std::pair<std::any, seismic_data_type> get(int index) const;
	virtual void set_field(int index, std::pair<std::any, seismic_data_type>);

	virtual object_type type_id() { return object_type::SEGY_TRACEHEADER; }

#ifdef PYTHON
	segy_trace_header(py::dict trace_header) { throw std::runtime_error("Method not implemented"); }
#endif
};

#endif

#ifndef __SEISMIC_TRACE
#define __SEISMIC_TRACE

#include <memory>
#include <vector>

#include <Eigen/Dense>

#include "obj_base.h"
#include "seismic_trace_header.h"

#ifdef PYTHON
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/eigen.h>

namespace py = pybind11;
#endif

class seismic_trace : public obj_base {
protected:
	std::shared_ptr<seismic_trace_header> f_header;

public:
	virtual std::shared_ptr<seismic_trace_header> header() = 0;
	virtual void set_header(std::shared_ptr<seismic_trace_header> header) = 0;
	virtual seismic_variant_vector &data() = 0;
	virtual void set_data(const seismic_variant_vector &data) = 0;

    virtual seismic_variant_value   X() = 0;
    virtual seismic_variant_value   Y() = 0;
    virtual seismic_variant_value   iline() = 0;
    virtual seismic_variant_value   crossline() = 0;
};

#ifdef PYTHON
void py_seismic_trace_init(py::module &m,
	py::class_<seismic_trace, std::shared_ptr<seismic_trace>> &py_trace);
#endif

#endif

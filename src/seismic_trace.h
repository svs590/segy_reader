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
	std::shared_ptr<seismic_trace_header> header;
	Eigen::VectorXf data;
	int num_samples;

public:
	virtual std::shared_ptr<seismic_trace_header> get_header() = 0;
	virtual void set_header(std::shared_ptr<seismic_trace_header> header) = 0;
	virtual Eigen::VectorXf &get_data() = 0;
	virtual void set_data(const Eigen::VectorXf &data) = 0;
};

#ifdef PYTHON
void py_seismic_trace_header_init(py::module &m,
	py::class_<seismic_trace, std::shared_ptr<seismic_trace>> &py_trace);
#endif

#endif

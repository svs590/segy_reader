#ifndef __SEISMIC_DP
#define __SEISMIC_DP

#include <string>
#include <memory>

#include "obj_base.h"
#include "segy_trace.h"
#include "seismic_header_map.h"
#include "seismic_abstract_header.h"
#include "seismic_trace_header.h"
#include "seismic_geometry.h"

#ifdef PYTHON
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
#endif

class seismic_data_provider : public obj_base {
public:
	seismic_data_provider() { }

	virtual std::string                                         text_header() = 0;
	virtual std::shared_ptr<seismic_abstract_header>            bin_header() = 0;
	virtual std::shared_ptr<seismic_header_map>                 header_map() = 0;
	virtual void                                                set_header_map(std::shared_ptr<seismic_header_map> map) = 0;

	virtual std::shared_ptr<seismic_trace_header>               trace_header(int index) = 0;
	virtual std::shared_ptr<seismic_trace>                      get_trace(int index) = 0;

	virtual void                                                preprocessing() = 0;

	virtual std::shared_ptr<seismic_geometry_info>              get_geometry() = 0;

	virtual std::vector<std::shared_ptr<seismic_trace>>         get_traces(seismic_line_info line) = 0;
	virtual std::vector<std::shared_ptr<seismic_trace>>         get_traces(int start, int end) = 0;
	virtual std::vector<std::shared_ptr<seismic_trace_header>>  get_headers(seismic_line_info line) = 0;
	virtual std::vector<std::shared_ptr<seismic_trace_header>>  get_headers(int start, int end) = 0;
	// get_traces(points) (interpolation)

	virtual void                                                close() = 0;
	virtual int64_t                                             traces_count() = 0;
	virtual int                                                 samples_count() = 0;
	virtual double                                              sample_interval() = 0;
};

#ifdef PYTHON
void py_seismic_data_provider_init(py::module &m, 
	py::class_<seismic_data_provider, std::shared_ptr<seismic_data_provider>> &data_provider);
#endif

#endif

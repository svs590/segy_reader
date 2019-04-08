#pragma once

#include <string>
#include <exception>
#include <memory>
#include <map>

#include "data_types.h"
#include "data_conversion.h"

#include "seismic_trace_header.h"
#include "seismic_header_map.h"

#ifdef PYTHON
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
#endif


class segy_trace_header : public seismic_trace_header {
	std::shared_ptr<seismic_header_map> map;
    endian_order f_endian_order = endian_order::little;
    std::vector<byte_t> f_raw_data;
    bool f_req_field_init = false;
public:
	segy_trace_header(std::shared_ptr<seismic_header_map> map);
    segy_trace_header(std::shared_ptr<seismic_header_map> map, 
        byte_t *raw_data, endian_order swap);
	segy_trace_header(const segy_trace_header &header);
	~segy_trace_header() {}
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

	virtual std::pair<std::any, seismic_data_type> get(const std::string &name) const;
	virtual void set_field(int index, std::pair<std::any, seismic_data_type>);

	virtual object_type type_id() { return object_type::SEGY_TRACEHEADER; }

    int                                     iline();
    int                                     crossline();
    std::pair<std::any, seismic_data_type>  CDP_X();
    std::pair<std::any, seismic_data_type>  CDP_Y();
    std::pair<std::any, seismic_data_type>  CDP();
    std::pair<std::any, seismic_data_type>  Src_X();
    std::pair<std::any, seismic_data_type>  Src_Y();
    int                                     samples_count();
    double                                  sample_interval();

private:
    void parse_required();

    int                                     f_iline;
    int                                     f_crossline;
    std::pair<std::any, seismic_data_type>  f_CDP_X;
    std::pair<std::any, seismic_data_type>  f_CDP_Y;
    std::pair<std::any, seismic_data_type>  f_CDP;
    std::pair<std::any, seismic_data_type>  f_Src_X;
    std::pair<std::any, seismic_data_type>  f_Src_Y;
    int                                     f_samples_count;
    double                                  f_sample_interval;

#ifdef PYTHON
	segy_trace_header(py::dict trace_header) { throw std::runtime_error("Method not implemented"); }
#endif
};

#ifdef PYTHON
void py_segy_trace_header_init(py::module &m,
	py::class_<segy_trace_header,
		std::shared_ptr<segy_trace_header>> &py_segy_trace_header);
#endif
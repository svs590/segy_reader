#pragma once

#include <string>
#include <exception>
#include <memory>
#include <map>
#include <array>

#include "data_types.h"
#include "data_conversion.h"

#include "seismic_trace_header.h"
#include "seismic_header_map.h"
#include "segy_file.h"

#ifdef PYTHON
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
#endif


class segy_trace_header : public seismic_trace_header {
	std::shared_ptr<seismic_header_map> f_map;
    endian_order                        f_endian_order;
    std::vector<byte_t>                 f_raw_data;
    seismic_coords_type                 f_coord = seismic_coords_type::CDP;
    bool                                f_req_field_init = false;

public:
	segy_trace_header(std::shared_ptr<seismic_header_map> map);
    segy_trace_header(
        std::shared_ptr<seismic_header_map> map, 
        byte_t *raw_data, 
        endian_order swap,
        seismic_coords_type coord = seismic_coords_type::CDP
    );
	segy_trace_header(const segy_trace_header &header);
	~segy_trace_header() {}
	segy_trace_header& operator=(const segy_trace_header &obj);

	virtual size_t					count() const;
	virtual int						contains(const std::string &name) const;
	virtual seismic_data_type		type(const std::string &name) const;

	virtual seismic_variant_value   get(const std::string &name) const;
	virtual void                    set(const std::string &name, seismic_variant_value val);

	virtual object_type type_id() { return object_type::SEGY_TRACEHEADER; }

    virtual std::map<std::string, seismic_variant_value> to_map();
    virtual void from_map(const std::map<std::string, seismic_variant_value> &map);
    virtual bool is_valid();

    virtual seismic_variant_value           iline();
    virtual seismic_variant_value           crossline();
    seismic_variant_value                   CDP_X();
    seismic_variant_value                   CDP_Y();
    seismic_variant_value                   CDP();
    seismic_variant_value                   Src_X();
    seismic_variant_value                   Src_Y();
    seismic_variant_value                   coord_scalar();
    virtual seismic_variant_value           samples_count();
    virtual seismic_variant_value           sample_interval();
    virtual seismic_variant_value           X();
    virtual seismic_variant_value           Y();

    virtual std::vector<byte_t>             raw_data();

    virtual endian_order                    endian();

//protected:
    seismic_coords_type coords();

    void reset_header_map(std::shared_ptr<seismic_header_map> map);
    void reset_endian_order(endian_order order);
    void reset_coords(seismic_coords_type coord);

private:
    void parse_required();

    seismic_variant_value                   f_iline;
    seismic_variant_value                   f_crossline;
    seismic_variant_value                   f_CDP_X;
    seismic_variant_value                   f_CDP_Y;
    seismic_variant_value                   f_CDP;
    seismic_variant_value                   f_Src_X;
    seismic_variant_value                   f_Src_Y;
    seismic_variant_value                   f_coord_scalar;
    seismic_variant_value                   f_samples_count;
    seismic_variant_value                   f_sample_interval;

#ifdef PYTHON
	segy_trace_header(py::dict header) { throw std::runtime_error("Method not implemented"); }
#endif
};

#ifdef PYTHON
void py_segy_trace_header_init(py::module &m,
	py::class_<segy_trace_header,
		std::shared_ptr<segy_trace_header>> &py_segy_trace_header);
#endif
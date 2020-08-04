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

	segy_trace_header&              operator=(const segy_trace_header &obj);

	virtual size_t					count() const;
	virtual int						contains(const std::string &name) const;
	virtual seismic_data_type		type(const std::string &name) const;

	virtual seismic_variant_value   get(const std::string &name) const;
	virtual void                    set(const std::string &name, seismic_variant_value val);

	virtual object_type             type_id() { 
        return object_type::SEGY_TRACEHEADER; 
    }

    virtual 
    std::map<
        std::string, 
        seismic_variant_value
    >                               to_map();

    virtual void                    from_map(
        const std::map<std::string, seismic_variant_value> &map
    );

    virtual bool                    is_valid();

    virtual seismic_variant_value   iline();
    virtual seismic_variant_value   crossline();
    seismic_variant_value           CDP_X();
    seismic_variant_value           CDP_Y();
    seismic_variant_value           CDP();
    seismic_variant_value           Src_X();
    seismic_variant_value           Src_Y();
    seismic_variant_value           coord_scalar();
    virtual seismic_variant_value   samples_count();
    virtual seismic_variant_value   sample_interval();
    virtual seismic_variant_value   X();
    virtual seismic_variant_value   Y();

    virtual std::vector<byte_t>     raw_data();

    virtual endian_order            endian();
    void                            set_endian(endian_order order);

    seismic_coords_type             coords();
    void                            set_coords(seismic_coords_type coord);

    void                            reset_header_map(
        std::shared_ptr<seismic_header_map> map
    );

private:
    void                            parse_required();

    bool                            m_req_field_init = false;

    seismic_variant_value           m_iline;
    seismic_variant_value           m_crossline;
    seismic_variant_value           m_CDP_X;
    seismic_variant_value           m_CDP_Y;
    seismic_variant_value           m_CDP;
    seismic_variant_value           m_Src_X;
    seismic_variant_value           m_Src_Y;
    seismic_variant_value           m_coord_scalar;
    seismic_variant_value           m_samples_count;
    seismic_variant_value           m_sample_interval;

#ifdef PYTHON
	segy_trace_header(py::dict header) { 
        throw std::runtime_error("Method not implemented"); 
    }
#endif
};

#ifdef PYTHON
void py_segy_trace_header_init(
    py::module &m,
	py::class_<
        segy_trace_header, std::shared_ptr<segy_trace_header>
    > &py_segy_trace_header
);
#endif
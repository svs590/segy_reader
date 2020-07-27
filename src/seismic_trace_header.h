#ifndef __SEISMIC_TRACE_HEADER
#define __SEISMIC_TRACE_HEADER

#include <string>
#include <any>
#include <map>

#ifdef PYTHON
#include <pybind11/pybind11.h>
#endif

#include "obj_base.h"
#include "data_types.h"
#include "seismic_header_map.h"

enum class seismic_coords_type { CDP, SRC };

class seismic_trace_header : public obj_base {
public:
	virtual size_t                  count() const = 0;
	virtual int                     contains(const std::string &name) const = 0;
	virtual seismic_data_type       type(const std::string &name) const = 0;

	virtual seismic_variant_value   get(const std::string &name) const = 0;
	virtual void                    set(const std::string &name, seismic_variant_value val) = 0;

    virtual std::map<std::string, seismic_variant_value> to_map() = 0;
    virtual void from_map(const std::map<std::string, seismic_variant_value> &map) = 0;
    virtual bool is_valid() = 0;

    virtual seismic_variant_value   X() = 0;
    virtual seismic_variant_value   Y() = 0;
    virtual seismic_variant_value   iline() = 0;
    virtual seismic_variant_value   crossline() = 0;
    virtual seismic_variant_value   samples_count() = 0;
    virtual seismic_variant_value   sample_interval() = 0;

    virtual std::vector<byte_t>     raw_data() = 0;

    virtual endian_order            endian() = 0;

//protected:
    virtual seismic_coords_type coords() = 0;

    virtual void reset_header_map(std::shared_ptr<seismic_header_map> map) = 0;
    virtual void reset_endian_order(endian_order order) = 0;
    virtual void reset_coords(seismic_coords_type coord) = 0;

#ifdef PYTHON
	
#endif
};

#ifdef PYTHON
void py_seismic_trace_header_init(py::module &m,
	py::class_<seismic_trace_header, std::shared_ptr<seismic_trace_header>> &py_segy_reader);
#endif

#endif

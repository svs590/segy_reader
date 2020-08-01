#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <tuple>
#include <map>

#include "data_types.h"

#include "seismic_header_field.h"
#include "obj_base.h"

#ifdef PYTHON
#include <pybind11/pybind11.h>

namespace py = pybind11;
#endif

enum class header_map_type {
	CUSTOM,
	STANDARD
};

class seismic_header_map : public obj_base {
public:
    // Tuple kind of { data_type, byte_loc, byte_size, is_required }
    using header_field_t    = std::tuple<seismic_data_type, int, int, bool>;
    
    /// NOTE: vector searching with small size is faster than getting std::map elem
    using storage_t         = std::vector<std::pair<std::string, header_field_t>>;
    using map_storage_t     = std::map<std::string, header_field_t>;
    using descr_storage_t   = std::vector<std::pair<std::string, std::string>>;

protected:
	header_map_type                         m_type;
    storage_t                               m_fields;
    descr_storage_t                         m_descr;

public:
	virtual void set(
		std::string         name,
        header_field_t      val
	) = 0;

    virtual void set(
        std::string         name,
        header_field_t      val,
        std::string         desc
    ) = 0;

    virtual void set(
        std::string         name,
        int                 byte_loc,
        int                 byte_size,
        seismic_data_type   type
    ) = 0;

    virtual void set(
        std::string         name,
        int                 byte_loc,
        int                 byte_size,
        seismic_data_type   type,
        std::string         desc
    ) = 0;

	virtual void                            remove(const std::string &name) = 0;
	virtual void                            clear() = 0;
	virtual bool                            contains(const std::string &name) const = 0;
	virtual header_field_t                  get(const std::string &name) const = 0;
    virtual std::pair<std::string, header_field_t>
                                            get(int index) const = 0;
	virtual size_t                          count() const = 0;
	virtual header_map_type                 type() const = 0;

	// Map kind of {name : (data_type, byte_position, byte_size)}
	virtual map_storage_t                   to_map() const = 0;
	virtual void                            from_map(const map_storage_t &m) = 0;
    
    virtual 
    std::map<std::string, std::string>      fields_descr() = 0;

    virtual bool operator==(std::shared_ptr<seismic_header_map>) = 0;
};

#ifdef PYTHON
void py_seismic_header_map_init(
    py::module &m,
	py::class_<seismic_header_map, std::shared_ptr<seismic_header_map>> &py_header_map
);
#endif

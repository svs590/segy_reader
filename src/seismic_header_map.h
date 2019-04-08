#ifndef __SEISMIC_HEADER_MAP
#define __SEISMIC_HEADER_MAP

#include <string>
#include <memory>
#include <unordered_map>
#include <tuple>

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
protected:
	header_map_type map_type;
public:
    using header_field_info = std::tuple<int, int, seismic_data_type, std::string>;
    using map_storage = std::vector<std::pair<std::string, header_field_info>>;

	virtual void set_field(
		std::string name,
		int byte_loc,
		int byte_size,
		seismic_data_type type,
		std::string desc
	) = 0;

	virtual void                                    remove(const std::string &name) = 0;
	virtual void                                    clear() = 0;
	virtual int                                     contains(const std::string &name) const = 0;
	virtual header_field_info                       get_field(const std::string &name) const = 0;
    virtual std::pair<std::string, header_field_info>
                                                    get_field(int index) const = 0;
	virtual size_t                                  count() const = 0;
	virtual header_map_type                         type() const = 0;

	// Словарь вида {name : (byte_position, byte_size, data_type, description)}
	virtual map_storage                             to_map() const = 0;
	virtual void                                    set(const map_storage &m) = 0;
};

#ifdef PYTHON
void py_seismic_header_map_init(py::module &m,
	py::class_<seismic_header_map, std::shared_ptr<seismic_header_map>> &py_header_map);
#endif

#endif

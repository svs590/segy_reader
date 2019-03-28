#ifndef __SEISMIC_HEADER_MAP
#define __SEISMIC_HEADER_MAP

#include <string>
#include <memory>
#include <map>
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
	STANDARD,
	OBC,
	SEND,
	ARMSS,
	PSEGY,
	NODE_OLD,
	NODE,
	SU,
	SU_ONLY,
	SU_BOTH
};

class seismic_header_map : public obj_base {
protected:
	header_map_type map_type;
public:
	virtual void add_field(
		std::string name,
		int byte_loc,
		int byte_size,
		seismic_data_type type,
		std::string desc
	) = 0;

	virtual void									set_field(std::shared_ptr<seismic_traceheader_field> header) = 0;
	virtual void									remove(int index) = 0;
	virtual void									remove(const std::string &name) = 0;
	virtual void									clear() = 0;
	virtual int										index_of(const std::string &name) const = 0;
	virtual int										contains(const std::string &name) const = 0;
	virtual std::shared_ptr<seismic_traceheader_field>	get_field(int index) const = 0;
	virtual std::shared_ptr<seismic_traceheader_field>	get_field(const std::string &name) const = 0;
	virtual int										count() const = 0;
	virtual header_map_type							type() const = 0;

	// Словарь вида {name : (byte_position, byte_size, data_type, description)}
	virtual std::map<std::string, std::tuple<int, int, seismic_data_type, std::string>> to_map() const = 0;
	virtual void set(
		const std::map<std::string, std::tuple<int, int, seismic_data_type, std::string>> &m) = 0;
};

#ifdef PYTHON
void py_seismic_header_map_init(py::module &m,
	py::class_<seismic_header_map, std::shared_ptr<seismic_header_map>> &py_header_map);
#endif

#endif

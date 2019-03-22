#ifndef __SEISMIC_HEADER_MAP
#define __SEISMIC_HEADER_MAP

#include <string>
#include <memory>

#include "data_types.h"

#include "seismic_header_field.h"
#include "obj_base.h"

enum class header_map_type {
	STARNDART
};

class seismic_header_map : public obj_base {
public:
	virtual void add_field(
		std::string name,
		int byte_loc,
		int byte_size,
		seismic_data_type type,
		std::string desc
	) = 0;

	virtual void									set(std::shared_ptr<seismic_traceheader_field> header) = 0;
	virtual void									remove(int index) = 0;
	virtual void									remove(const std::string &name) = 0;
	virtual void									clear() = 0;
	virtual int										index_of(const std::string &name) = 0;
	virtual int										contains(const std::string &name) = 0;
	virtual std::shared_ptr<seismic_traceheader_field>	get_field(int index) = 0;
	virtual std::shared_ptr<seismic_traceheader_field>	get_field(const std::string &name) = 0;
	virtual int										count() const = 0;
	virtual header_map_type							type() const = 0;
};

#endif

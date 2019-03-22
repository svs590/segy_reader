#ifndef __SEGY_HEADER_MAP
#define __SEGY_HEADER_MAP

#include <string>

#include "segy_header_info.h"
#include "seismic_header_map.h"

#include "geolib_defines.h"

class segy_header_map : public seismic_header_map {
	void *obj;
public:
	segy_header_map(const void *obj);
	segy_header_map& operator=(const void *obj);

	virtual void set(std::shared_ptr<seismic_traceheader_field> header);
	virtual void add_field(
		std::string name,
		int byte_loc,
		int byte_size,
		seismic_data_type type,
		std::string desc
	);
	virtual void remove(int index);
	virtual void remove(const std::string &name);
	virtual void clear();
	virtual int index_of(const std::string &name);
	virtual int contains(const std::string &name);
	virtual std::shared_ptr<seismic_traceheader_field> get_field(int index);
	virtual std::shared_ptr<seismic_traceheader_field> get_field(const std::string &name);
	virtual int count() const;
	virtual header_map_type type() const ;

	virtual object_type type_id() { return object_type::SEGY_HEADERMAP; }
};

#endif

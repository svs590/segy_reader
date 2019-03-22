#ifndef __SEGY_HEADER_INFO
#define __SEGY_HEADER_INFO

#include <string>

#include "seismic_header_field.h"
#include "obj_base.h"
#include "segy_header_map.h"

#include "geolib_defines.h"

class segy_traceheader_field : public seismic_traceheader_field {
	void *obj;
public:
	segy_traceheader_field(const void *obj);
	segy_traceheader_field& operator=(const void *obj);

	virtual int byte_loc();
	virtual int byte_size();
	virtual std::string name();
	virtual std::string description();
	virtual seismic_data_type type_in();
	virtual seismic_data_type type_out();

	virtual void set_byte_loc(int );
	virtual void set_byte_size(int byteSize);
	virtual void set_type_in(seismic_data_type type);
	virtual void set_type_out(seismic_data_type type);

	virtual object_type type_id() { return object_type::SEGY_TRACEHEADER_FIELD; }

	friend class segy_header_map;
};

#endif

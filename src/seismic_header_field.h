#ifndef __SEISMIC_HEADER_INFO
#define __SEISMIC_HEADER_INFO

#include <string>

#include "obj_base.h"
#include "data_types.h"

class seismic_traceheader_field : public obj_base {
public:
	virtual int byte_loc() = 0;
	virtual int byte_size() = 0;
	virtual std::string name() = 0;
	virtual std::string description() = 0;
	virtual seismic_data_type type_in() = 0;
	virtual seismic_data_type type_out() = 0;

	virtual void set_byte_loc(int) = 0;
	virtual void set_byte_size(int byteSize) = 0;
	virtual void set_type_in(seismic_data_type type) = 0;
	virtual void set_type_out(seismic_data_type type) = 0;
};

#endif

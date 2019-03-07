#ifndef SEGY_HEADER_INFO
#define SEGY_HEADER_INFO

#include <string>

#include "geolib_defines.h"

class segy_header_info {
	void *obj;
public:
	segy_header_info(const void *obj);
	segy_header_info& operator=(const void *obj);

	int getByteLoc();
	int getByteSize();
	std::string getName();
	std::string getDescription();
	cseis_geolib::type_t getInType();
	cseis_geolib::type_t getOutType();

	void setByteLoc(int byteLoc);
	void setByteSize(int byteSize);
	void setInType(cseis_geolib::type_t type);
	void setOutType(cseis_geolib::type_t type);
};

#endif

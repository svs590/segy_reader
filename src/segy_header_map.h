#ifndef SEGY_HEADER_MAP
#define SEGY_HEADER_MAP

#include <string>

#include "segy_header_info.h"

#include "geolib_defines.h"

class segy_header_map {
	void *obj;
public:
	segy_header_map(const void *obj);
	segy_header_map& operator=(const void *obj);

	void addHeader(const segy_header_map &header);
	void addHeader(
		std::string getName, 
		int getByteLoc, 
		int getByteSize, 
		cseis_geolib::type_t inType, 
		std::string desc
	);
	void remove(int index);
	void remove(std::string getName);
	void removeAll();
	int getIndexOf(std::string getName);
	int contains(std::string getName);
	segy_header_info headerInfo(int index);
	segy_header_info headerInfo(std::string getName);
	int count();
	int mapId();
};

#endif

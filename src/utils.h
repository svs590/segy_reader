#ifndef __UTILS
#define __UTILS

#include <string>
#include <map>
#include <any>

#include "geolib_defines.h"

#include "data_types.h"

#define NOT_INDEX -1

#define DLLIMPORT extern "C" __declspec(dllimport)

std::wstring utf8_to_unicode(const std::string& internal);

class geolib_type_converter {
public:
	template <typename _From, typename _To> 
	static _To convert(_From type);
};

class smatr_any_cast {
	static const std::map<int, seismic_data_type> typemap;
public:
	void cast(std::any a, void *buffer);
};

#endif

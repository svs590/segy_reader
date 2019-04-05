#ifndef __UTILS
#define __UTILS

#include <string>
#include <map>
#include <any>

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#include "data_types.h"

#define NOT_INDEX -1

#define DLLIMPORT extern "C" __declspec(dllimport)

std::wstring utf8_to_unicode(const std::string& internal);

class geolib_type_converter {
public:
	template <typename _From, typename _To> 
	static _To convert(_From type);
};

long long get_available_memory();

void seekg_relative(int64_t rel_pos, bfs::ifstream &stream);

bool is_little_endian();

#endif

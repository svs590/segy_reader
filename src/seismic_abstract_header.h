#ifndef __SEISMIC_ABSTRACT_HEADER
#define __SEISMIC_ABSTRACT_HEADER

#include <string>
#include <map>

class seismic_abstract_header {
public:
	virtual std::pair<std::string, int> get(int index) = 0;
	virtual int index_of(const std::string &name) = 0;
	virtual std::map<std::string, int> to_map() = 0;
};

#endif

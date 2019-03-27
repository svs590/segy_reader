#ifndef __SEGY_HEADER_MAP
#define __SEGY_HEADER_MAP

#include <string>
#include <list>

#include "segy_header_info.h"
#include "seismic_header_map.h"

#include "geolib_defines.h"

class segy_header_map : public seismic_header_map {
	void *obj;
	std::list<std::string> constant_fields_3d = {
		"bin_x",
		"bin_y",
		"row",
		"col"
	};
public:
	segy_header_map(const void *obj);
	segy_header_map& operator=(const void *obj);

	virtual void set_field(std::shared_ptr<seismic_traceheader_field> header);
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
	virtual int index_of(const std::string &name) const;
	virtual int contains(const std::string &name) const;
	virtual std::shared_ptr<seismic_traceheader_field> get_field(int index) const;
	virtual std::shared_ptr<seismic_traceheader_field> get_field(const std::string &name) const;
	virtual int count() const;
	virtual header_map_type type() const ;

	virtual std::map<std::string, std::tuple<int, int, seismic_data_type, std::string>> to_map() const;
	virtual void set(
		const std::map<std::string, std::tuple<int, int, seismic_data_type, std::string>> &m);

	virtual object_type type_id() { return object_type::SEGY_HEADERMAP; }

private:
	bool delete_check(int index);
	bool delete_check(const std::string name);
};

#endif

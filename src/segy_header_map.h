#pragma once

#include <memory>
#include <string>
#include <list>
#include <unordered_map>

#include "segy_header_info.h"
#include "seismic_header_map.h"
#include "segy_reader.h"
#include "segy_trace_header.h"

#include "geolib_defines.h"

class segy_header_map : public seismic_header_map {
    static const std::unordered_map<std::string,
        std::tuple<int, int, seismic_data_type, std::string>> map_standard_req;
    static const std::unordered_map<std::string,
        std::tuple<int, int, seismic_data_type, std::string>> map_standard;

    std::unordered_map<std::string,
        std::tuple<int, int, seismic_data_type, std::string>> f_map;

    header_map_type f_type;
public:
    segy_header_map();
	segy_header_map(header_map_type type);
	segy_header_map(const segy_header_map &map);
	segy_header_map(std::shared_ptr<segy_header_map> map);
    segy_header_map(std::shared_ptr<seismic_header_map> map);

    virtual void set_field(
        std::string name,
        int byte_loc,
        int byte_size,
        seismic_data_type type,
        std::string desc
    );

    virtual void                                    remove(const std::string &name);
    virtual void                                    clear();
    virtual bool                                    contains(const std::string &name) const;
    virtual std::tuple<int, int, seismic_data_type, std::string>
                                                    get_field(const std::string &name) const;
    virtual size_t                                  count() const;
    virtual header_map_type                         type() const;

	virtual std::unordered_map<std::string, std::tuple<int, int, seismic_data_type, std::string>> to_map() const;
	virtual void set(
		const std::unordered_map<std::string, std::tuple<int, int, seismic_data_type, std::string>> &m);

	virtual object_type type_id() { return object_type::SEGY_HEADERMAP; }

	friend class segy_reader;
	friend class segy_trace_header;
};

#ifdef PYTHON
void py_segy_header_map_init(py::module &m,
	py::class_<segy_header_map, std::shared_ptr<segy_header_map>> &py_segy_header_map);
#endif

#pragma once

#include <memory>
#include <string>
#include <list>
#include <unordered_map>
#include <vector>

#include "seismic_header_map.h"
#include "segy_reader.h"
#include "segy_trace_header.h"

class segy_header_map : public seismic_header_map {
    static const map_storage map_standard_req;
    static const map_storage map_standard;

    map_storage f_map;

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
    virtual int                                     contains(const std::string &name) const;
    virtual header_field_info                       get_field(const std::string &name) const;
    virtual std::pair<std::string, header_field_info>                       
                                                    get_field(int index) const;
    virtual size_t                                  count() const;
    virtual header_map_type                         type() const;

	virtual map_storage                             to_map() const;
	virtual void                                    from_map(const map_storage &m);

	virtual object_type type_id() { return object_type::SEGY_HEADERMAP; }

	friend class segy_reader;
	friend class segy_trace_header;

private:
    int                                             contains(const std::string &name, const segy_header_map::map_storage &map) const;
};

#ifdef PYTHON
void py_segy_header_map_init(py::module &m,
	py::class_<segy_header_map, std::shared_ptr<segy_header_map>> &py_segy_header_map);
#endif

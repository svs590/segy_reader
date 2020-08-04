#include <map>
#include <string>
#include <iostream>

#include "segy_trace_header.h"
#include "utils.h"
#include "segy_header_map.h"
#include "data_types.h"
#include "seismic_exception.h"

using namespace std;


segy_trace_header::segy_trace_header(shared_ptr<seismic_header_map> map) {
    m_raw_data.resize(segy_file::trace_header_size, 0);

    m_endian_order = native_order();

	this->m_map = map;
}

segy_trace_header::segy_trace_header(const segy_trace_header &header) {
    m_map                   = header.m_map;
    m_raw_data              = header.m_raw_data;
    m_coord                 = header.m_coord;
    m_endian_order          = header.m_endian_order;

    if (header.m_req_field_init) {
        m_iline             = header.m_iline;
        m_crossline         = header.m_crossline;
        m_CDP_X             = header.m_CDP_X;
        m_CDP_Y             = header.m_CDP_Y;
        m_CDP               = header.m_CDP;
        m_Src_X             = header.m_Src_X;
        m_Src_Y             = header.m_Src_Y;
        m_samples_count     = header.m_samples_count;
        m_sample_interval   = header.m_sample_interval;
        m_coord_scalar      = header.m_coord_scalar;
    }

    m_req_field_init        = header.m_req_field_init;
}

segy_trace_header& segy_trace_header::operator=(const segy_trace_header &obj) {
    m_map                   = obj.m_map;
    m_endian_order          = obj.m_endian_order;
    m_coord                 = obj.m_coord;
    m_raw_data              = obj.m_raw_data;
	return *this;
}

segy_trace_header::segy_trace_header(
    shared_ptr<seismic_header_map> map, 
    byte_t *raw_data, 
    endian_order order,
    seismic_coords_type coord
) : segy_trace_header(map) {

    m_endian_order = order;
    m_coord = coord;
    copy(raw_data, raw_data + segy_file::trace_header_size, m_raw_data.data());
    parse_required();
}

void segy_trace_header::parse_required() {
    if (m_raw_data.size() != segy_file::trace_header_size)
        return;
    
    m_iline             = get("Inline");
    m_crossline         = get("Crossline");
    m_CDP_X             = get("CDP X");
    m_CDP_Y             = get("CDP Y");
    m_CDP               = get("CDP");
    m_Src_X             = get("Src X");
    m_Src_Y             = get("Src Y");
    m_samples_count     = get("Samples count");
    m_sample_interval   = get("Sample interval");

    m_coord_scalar      = get("Scalar coords");
    using namespace seismic_variant_operations;
    if (m_coord_scalar == 0)
        m_coord_scalar = (short)1;

    m_req_field_init    = true;
}

seismic_variant_value segy_trace_header::iline() {
    if (m_req_field_init)
        return m_iline;
    else
        return NOT_INDEX;
}

seismic_variant_value segy_trace_header::crossline() {
    if (m_req_field_init)
        return m_crossline;
    else
        return NOT_INDEX;
}

seismic_variant_value segy_trace_header::CDP_X() {
    if (m_req_field_init)
        return m_CDP_X;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::CDP_Y() {
    if (m_req_field_init)
        return m_CDP_Y;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::CDP() {
    if (m_req_field_init)
        return m_CDP;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::Src_X() {
    if (m_req_field_init)
        return m_Src_X;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::Src_Y() {
    if (m_req_field_init)
        return m_Src_Y;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::coord_scalar() {
    if (m_req_field_init)
        return m_coord_scalar;
    else
        return (int)1;
}

seismic_variant_value segy_trace_header::samples_count() {
    if (m_req_field_init)
        return m_samples_count;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::sample_interval() {
    if (m_req_field_init)
        return m_sample_interval;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::X() {
    if (m_req_field_init) {
        using namespace seismic_variant_operations;
        if (m_coord == seismic_coords_type::CDP)
            return m_CDP_X * m_coord_scalar;
        else
            return m_Src_X * m_coord_scalar;
    }
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::Y() {
    if (m_req_field_init) {
        using namespace seismic_variant_operations;
        if (m_coord == seismic_coords_type::CDP)
            return m_CDP_Y * m_coord_scalar;
        else
            return m_Src_Y * m_coord_scalar;
    }
    else
        return (int)0;
}


size_t segy_trace_header::count() const {
	return m_map->count();
}

int segy_trace_header::contains(const string &name) const {
	return (int)m_map->contains(name);
}

seismic_data_type segy_trace_header::type(const string &name) const {
    if (m_map->contains(name)) {
        auto field_info = m_map->get(name);
        return std::get<0>(field_info);
    }
    else
        SR_THROW(invalid_argument, "header map which tied to this header does not contains field " + name);
}

seismic_variant_value segy_trace_header::get(const string &name) const {
    if (m_map->contains(name)) {
        auto field_info = m_map->get(name);
        auto type       = std::get<0>(field_info);
        int pos         = std::get<1>(field_info);

        seismic_variant_value res;

        res = byte_to_seismic_variant_value(
            type, 
            &m_raw_data[pos], 
            m_endian_order
        );

        return res;
    }
    else
        return {};
}

void segy_trace_header::set(const string &name, seismic_variant_value val) {
    if (m_map->contains(name)) {
        auto field_info = m_map->get(name);
        auto type       = std::get<0>(field_info);
        int pos         = std::get<1>(field_info);

        seismic_variant_value_to_byte(
            type, 
            val, 
            &m_raw_data[pos],
            m_endian_order
        );
    }
    else
        return;
}

map<string, seismic_variant_value> segy_trace_header::to_map() {
    map<string, seismic_variant_value> res;
    for (int i = 0; i < m_map->count(); ++i) {
        auto field_info = m_map->get(i);
        string name = field_info.first;
        res[name] = get(name);
    }
    return res;
}

void segy_trace_header::from_map(const std::map<std::string, seismic_variant_value> &map) {
    for (auto &field : map)
        set(field.first, field.second);
}

bool segy_trace_header::is_valid() {
    if (!m_req_field_init)
        return false;

    int tmp;
    VARIANT_VALUE_CAST(tmp, samples_count());
    if (tmp <= 0)
        return false;

    return true;
}

vector<byte_t> segy_trace_header::raw_data() {
    return m_raw_data;
}

endian_order segy_trace_header::endian() {
    return m_endian_order;
}

seismic_coords_type segy_trace_header::coords() {
    return m_coord;
}

void segy_trace_header::reset_header_map(std::shared_ptr<seismic_header_map> map) {
    std::vector<byte_t> new_raw_data;
    new_raw_data.resize(segy_file::trace_header_size, 0);

    for (auto &old_field : m_map->to_map()) {
        if (map->contains(old_field.first)) {
            auto new_field = map->get(old_field.first);

            int old_byte_start  = std::get<1>(old_field.second);
            int old_byte_size   = std::get<2>(old_field.second);

            int new_byte_start  = std::get<1>(new_field);
            int new_byte_size   = std::get<2>(new_field);

            if (old_byte_size != new_byte_size)
                SR_THROW(invalid_argument, "can't convert old header map to new header map");

            memcpy(
                new_raw_data.data() + new_byte_start, 
                m_raw_data.data() + old_byte_start, 
                new_byte_size
            );
        }
    }

    m_raw_data = new_raw_data;
    m_map = map;
}

void segy_trace_header::set_endian(endian_order order) {
    if (m_endian_order == order)
        return;

    for (auto &field : m_map->to_map()) {
        int pos = std::get<1>(field.second);
        auto type = std::get<0>(field.second);

        swap_endian(type, &m_raw_data[pos], m_endian_order, order);
    }

    m_endian_order = order;
}

void segy_trace_header::set_coords(seismic_coords_type coord) {
    m_coord = coord;
}

#ifdef PYTHON
void py_segy_trace_header_init(py::module &m,
	py::class_<segy_trace_header,
	shared_ptr<segy_trace_header>> &py_segy_trace_header) {

    py::enum_<segy_coord>(m, "segy_coordinates")
        .value("CDP", segy_coord::CDP)
        .value("SRC", segy_coord::SRC)
        .export_values();

	py_segy_trace_header.def(py::init<shared_ptr<seismic_header_map>>(),
		py::arg("header_map")
	);

    py_segy_trace_header.def("reset_header_map", &segy_trace_header::reset_header_map);
}
#endif

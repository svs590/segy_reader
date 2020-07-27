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
    f_raw_data.resize(segy_file::trace_header_size, 0);

    f_endian_order = native_order();

	this->f_map = map;
}

segy_trace_header::segy_trace_header(const segy_trace_header &header) {
    f_map                   = header.f_map;
    f_raw_data              = header.f_raw_data;
    f_coord                 = header.f_coord;
    f_endian_order          = header.f_endian_order;

    if (header.f_req_field_init) {
        f_iline             = header.f_iline;
        f_crossline         = header.f_crossline;
        f_CDP_X             = header.f_CDP_X;
        f_CDP_Y             = header.f_CDP_Y;
        f_CDP               = header.f_CDP;
        f_Src_X             = header.f_Src_X;
        f_Src_Y             = header.f_Src_Y;
        f_samples_count     = header.f_samples_count;
        f_sample_interval   = header.f_sample_interval;
        f_coord_scalar      = header.f_coord_scalar;
    }

    f_req_field_init        = header.f_req_field_init;
}

segy_trace_header& segy_trace_header::operator=(const segy_trace_header &obj) {
    f_map = obj.f_map;
    f_endian_order = obj.f_endian_order;
    f_coord = obj.f_coord;
    f_raw_data = obj.f_raw_data;
	return *this;
}

segy_trace_header::segy_trace_header(
    shared_ptr<seismic_header_map> map, 
    byte_t *raw_data, 
    endian_order order,
    seismic_coords_type coord
) : segy_trace_header(map) {

    f_endian_order = order;
    f_coord = coord;
    copy(raw_data, raw_data + segy_file::trace_header_size, f_raw_data.data());
    parse_required();
}

void segy_trace_header::parse_required() {
    if (f_raw_data.size() != segy_file::trace_header_size)
        return;
    
    f_iline             = get("Inline");
    f_crossline         = get("Crossline");
    f_CDP_X             = get("CDP X");
    f_CDP_Y             = get("CDP Y");
    f_CDP               = get("CDP");
    f_Src_X             = get("Src X");
    f_Src_Y             = get("Src Y");
    f_samples_count     = get("Samples count");
    f_sample_interval   = get("Sample interval");

    f_coord_scalar      = get("Scalar coords");
    using namespace seismic_variant_operations;
    if (f_coord_scalar == 0)
        f_coord_scalar = (short)1;

    f_req_field_init    = true;
}

seismic_variant_value segy_trace_header::iline() {
    if (f_req_field_init)
        return f_iline;
    else
        return NOT_INDEX;
}

seismic_variant_value segy_trace_header::crossline() {
    if (f_req_field_init)
        return f_crossline;
    else
        return NOT_INDEX;
}

seismic_variant_value segy_trace_header::CDP_X() {
    if (f_req_field_init)
        return f_CDP_X;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::CDP_Y() {
    if (f_req_field_init)
        return f_CDP_Y;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::CDP() {
    if (f_req_field_init)
        return f_CDP;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::Src_X() {
    if (f_req_field_init)
        return f_Src_X;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::Src_Y() {
    if (f_req_field_init)
        return f_Src_Y;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::coord_scalar() {
    if (f_req_field_init)
        return f_coord_scalar;
    else
        return (int)1;
}

seismic_variant_value segy_trace_header::samples_count() {
    if (f_req_field_init)
        return f_samples_count;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::sample_interval() {
    if (f_req_field_init)
        return f_sample_interval;
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::X() {
    if (f_req_field_init) {
        using namespace seismic_variant_operations;
        if (f_coord == seismic_coords_type::CDP)
            return f_CDP_X * f_coord_scalar;
        else
            return f_Src_X * f_coord_scalar;
    }
    else
        return (int)0;
}

seismic_variant_value segy_trace_header::Y() {
    if (f_req_field_init) {
        using namespace seismic_variant_operations;
        if (f_coord == seismic_coords_type::CDP)
            return f_CDP_Y * f_coord_scalar;
        else
            return f_Src_Y * f_coord_scalar;
    }
    else
        return (int)0;
}


size_t segy_trace_header::count() const {
	return f_map->count();
}

int segy_trace_header::contains(const string &name) const {
	return (int)f_map->contains(name);
}

seismic_data_type segy_trace_header::type(const string &name) const {
    if (f_map->contains(name)) {
        auto field_info = f_map->get_field(name);
        return std::get<2>(field_info);
    }
    else
        SR_THROW(invalid_argument, "header map which tied to this header does not contains field " + name);
}

seismic_variant_value segy_trace_header::get(const string &name) const {
    if (f_map->contains(name) != NOT_INDEX) {
        auto field_info = f_map->get_field(name);
        int pos = std::get<0>(field_info);
        int size = std::get<1>(field_info);
        auto type = std::get<2>(field_info);
        seismic_variant_value res;

        switch (type) {
        case seismic_data_type::INT:
            if (size != 4)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            res = byte_to_int(&f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::SHORT:
            if (size != 2)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            res = byte_to_short(&f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::USHORT:
            if (size != 2)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            res = byte_to_ushort(&f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::INT64:
            if (size != 8)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            res = byte_to_int64(&f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::UINT64:
            if (size != 8)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            res = byte_to_uint64(&f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::FLOAT:
            if (size != 4)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            res = byte_to_float(&f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::DOUBLE:
            if (size != 8)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            res = byte_to_double(&f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::CHAR:
            if (size != 1)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            res = (char)f_raw_data[pos];
            break;
        default:
            SR_THROW(invalid_argument, "invalid type for field " + name);
            break;
        }

        return res;
    }
    else
        return {};
}

void segy_trace_header::set(const string &name, seismic_variant_value val) {
    if (f_map->contains(name)) {
        auto field_info = f_map->get_field(name);
        int pos = std::get<0>(field_info);
        int size = std::get<1>(field_info);
        auto type = std::get<2>(field_info);

        if (type == seismic_data_type::INT) {
            if (size != 4)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            int __val;
            VARIANT_VALUE_CAST(__val, val);
            int_to_byte(__val, &f_raw_data[pos], f_endian_order);
        }
        else if (type == seismic_data_type::SHORT) {
            if (size != 2)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            int16_t __val;
            VARIANT_VALUE_CAST(__val, val);
            short_to_byte(__val, &f_raw_data[pos], f_endian_order);
        }
        else if (type == seismic_data_type::USHORT) {
            if (size != 2)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            uint16_t __val;
            VARIANT_VALUE_CAST(__val, val);
            ushort_to_byte(__val, &f_raw_data[pos], f_endian_order);
        }
        else if (type == seismic_data_type::INT64) {
            if (size != 8)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            int64_t __val;
            VARIANT_VALUE_CAST(__val, val);
            int64_to_byte(__val, &f_raw_data[pos], f_endian_order);
        }
        else if (type == seismic_data_type::UINT64) {
            if (size != 8)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            uint64_t __val;
            VARIANT_VALUE_CAST(__val, val);
            uint64_to_byte(__val, &f_raw_data[pos], f_endian_order);
        }
        else if (type == seismic_data_type::FLOAT) {
            if (size != 4)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            float __val;
            VARIANT_VALUE_CAST(__val, val);
            float_to_byte(__val, &f_raw_data[pos], f_endian_order);
        }
        else if (type == seismic_data_type::DOUBLE) {
            if (size != 8)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            double __val;
            VARIANT_VALUE_CAST(__val, val);
            float_to_byte(__val, &f_raw_data[pos], f_endian_order);
        }
        else if (type == seismic_data_type::CHAR) {
            if (size != 1)
                SR_THROW(invalid_argument, "invalid byte size for field " + name);
            char __val;
            VARIANT_VALUE_CAST(__val, val);
            f_raw_data[pos] = static_cast<byte_t>(__val);
        }
        else
            SR_THROW(invalid_argument, "invalid type for field " + name);
    }
    else
        return;
}

map<string, seismic_variant_value> segy_trace_header::to_map() {
    map<string, seismic_variant_value> res;
    for (int i = 0; i < f_map->count(); ++i) {
        auto field_info = f_map->get_field(i);
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
    if (!f_req_field_init)
        return false;

    int tmp;
    VARIANT_VALUE_CAST(tmp, samples_count());
    if (tmp <= 0)
        return false;

    return true;
}

vector<byte_t> segy_trace_header::raw_data() {
    return f_raw_data;
}

endian_order segy_trace_header::endian() {
    return f_endian_order;
}

seismic_coords_type segy_trace_header::coords() {
    return f_coord;
}

void segy_trace_header::reset_header_map(std::shared_ptr<seismic_header_map> map) {
    std::vector<byte_t> new_raw_data;
    new_raw_data.resize(segy_file::trace_header_size, 0);

    for (auto &old_field : f_map->to_map()) {
        if (map->contains(old_field.first)) {
            auto new_field = map->get_field(old_field.first);

            int old_byte_start  = std::get<0>(old_field.second);
            int old_byte_size   = std::get<1>(old_field.second);

            int new_byte_start  = std::get<0>(new_field);
            int new_byte_size   = std::get<1>(new_field);

            if (old_byte_size != new_byte_size)
                SR_THROW(invalid_argument, "can't convert old header map to new header map");

            memcpy(
                new_raw_data.data() + new_byte_start, 
                f_raw_data.data() + old_byte_start, 
                new_byte_size
            );
        }
    }

    f_raw_data = new_raw_data;
    f_map = map;
}

void segy_trace_header::reset_endian_order(endian_order order) {
    if (f_endian_order == order)
        return;

    for (auto &field : f_map->to_map()) {
        int pos = std::get<0>(field.second);
        auto type = std::get<2>(field.second);

        if (type == seismic_data_type::INT) {
            int __val = byte_to_int(&f_raw_data[pos], f_endian_order);
            int_to_byte(__val, &f_raw_data[pos], order);
        }
        else if (type == seismic_data_type::SHORT) {
            int16_t __val = byte_to_short(&f_raw_data[pos], f_endian_order);
            short_to_byte(__val, &f_raw_data[pos], order);
        }
        else if (type == seismic_data_type::USHORT) {
            uint16_t __val = byte_to_ushort(&f_raw_data[pos], f_endian_order);
            ushort_to_byte(__val, &f_raw_data[pos], order);
        }
        else if (type == seismic_data_type::INT64) {
            int64_t __val = byte_to_int64(&f_raw_data[pos], f_endian_order);
            int64_to_byte(__val, &f_raw_data[pos], order);
        }
        else if (type == seismic_data_type::UINT64) {
            uint64_t __val = byte_to_uint64(&f_raw_data[pos], f_endian_order);
            uint64_to_byte(__val, &f_raw_data[pos], order);
        }
        else if (type == seismic_data_type::FLOAT) {
            float __val = byte_to_float(&f_raw_data[pos], f_endian_order);
            float_to_byte(__val, &f_raw_data[pos], order);
        }
        else if (type == seismic_data_type::DOUBLE) {
            double __val = byte_to_double(&f_raw_data[pos], f_endian_order);
            double_to_byte(__val, &f_raw_data[pos], order);
        }
        else if (type == seismic_data_type::CHAR) {
            int8_t __val = byte_to_int8_t(&f_raw_data[pos], f_endian_order);
            int8_t_to_byte(__val, &f_raw_data[pos], order);
        }
    }

    f_endian_order = order;
}

void segy_trace_header::reset_coords(seismic_coords_type coord) {
    f_coord = coord;
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

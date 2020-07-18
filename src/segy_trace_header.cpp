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
	this->f_map = map;
}

segy_trace_header::segy_trace_header(const segy_trace_header &header) {
    f_map                   = header.f_map;
    f_raw_data              = header.f_raw_data;
    f_coord                 = header.f_coord;

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
    segy_coord coord
) {
    this->f_map = map;
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
        if (f_coord == segy_coord::CDP)
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
        if (f_coord == segy_coord::CDP)
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

        switch (type) {
        case seismic_data_type::INT:
            if (size != 4)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            if (!holds_alternative<int>(val))
                throw invalid_argument("segy_trace_header: set: value "
                    "for field " + name + " does not match type of INT");
            int_to_byte(std::get<int>(val), &f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::SHORT:
            if (size != 2)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            if (!holds_alternative<short>(val))
                throw invalid_argument("segy_trace_header: set: value "
                    "for field " + name + " does not match type of SHORT");
            short_to_byte(std::get<short>(val), &f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::USHORT:
            if (size != 2)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            if (!holds_alternative<unsigned short>(val))
                throw invalid_argument("segy_trace_header: set: value "
                    "for field " + name + " does not match type of USHORT");
            ushort_to_byte(std::get<unsigned short>(val), &f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::INT64:
            if (size != 8)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            if (!holds_alternative<int64_t>(val))
                throw invalid_argument("segy_trace_header: set: value "
                    "for field " + name + " does not match type of INT64");
            int64_to_byte(std::get<int64_t>(val), &f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::UINT64:
            if (size != 8)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            if (!holds_alternative<uint64_t>(val))
                throw invalid_argument("segy_trace_header: set: value "
                    "for field " + name + " does not match type of UINT64");
            uint64_to_byte(std::get<uint64_t>(val), &f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::FLOAT:
            if (size != 4)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            if (!holds_alternative<float>(val))
                throw invalid_argument("segy_trace_header: set: value "
                    "for field " + name + " does not match type of FLOAT");
            float_to_byte(std::get<float>(val), &f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::DOUBLE:
            if (size != 8)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            if (!holds_alternative<float>(val))
                throw invalid_argument("segy_trace_header: set: value "
                    "for field " + name + " does not match type of FLOAT");
            float_to_byte(std::get<float>(val), &f_raw_data[pos], f_endian_order);
            break;
        case seismic_data_type::CHAR:
            if (size != 1)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            if (!holds_alternative<char>(val))
                throw invalid_argument("segy_trace_header: set: value "
                    "for field " + name + " does not match type of CHAR");
            f_raw_data[pos] = (byte_t)(std::get<char>(val));
            break;
        default:
            throw invalid_argument("segy_trace_header: get: invalid type for field " + name);
            break;
        }
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

void segy_trace_header::set(const std::map<std::string, seismic_variant_value> &map) {
    throw runtime_error("segy_trace_header: set: method not implemented");
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
}
#endif

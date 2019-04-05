#include "segy_trace_header.h"
#include "utils.h"

#include "segy_header_map.h"

using namespace std;
using namespace cseis_geolib;

//DLLIMPORT void*			cseis_csNativecsSegyTraceHeader_createInstance(void *hdrMap);
DLLIMPORT void*			cseis_csNativecsSegyTraceHeader_copyInstance(const void *obj);
DLLIMPORT void			cseis_csNativecsSegyTraceHeader_deleteInstance(const void *obj);
DLLIMPORT int			cseis_csNativecsSegyTraceHeader_numHeaders(const void *obj);
DLLIMPORT char const*	cseis_csNativecsSegyTraceHeader_headerDesc(const void *obj, int hdrIndex);
DLLIMPORT char const*	cseis_csNativecsSegyTraceHeader_headerName(const void *obj, int hdrIndex);
DLLIMPORT int			cseis_csNativecsSegyTraceHeader_headerIndex(const void *obj, const char *name);
DLLIMPORT type_t		cseis_csNativecsSegyTraceHeader_headerType(const void *obj, int hdrIndex);
DLLIMPORT int			cseis_csNativecsSegyTraceHeader_intValue(const void *obj, int hdrIndex);
DLLIMPORT float			cseis_csNativecsSegyTraceHeader_floatValue(const void *obj, int hdrIndex);
DLLIMPORT double		cseis_csNativecsSegyTraceHeader_doubleValue(const void *obj, int hdrIndex);
DLLIMPORT char const*	cseis_csNativecsSegyTraceHeader_stringValue(const void *obj, int hdrIndex);

// Not implemented
DLLIMPORT int			cseis_csNativecsSegyTraceHeader_byteLoc(const void *obj, int hdrIndex);
DLLIMPORT int			cseis_csNativecsSegyTraceHeader_bytePos(const void *obj, int hdrIndex);
DLLIMPORT void			cseis_csNativecsSegyTraceHeader_setIntValue(int hdrIndex, int value);
DLLIMPORT void			cseis_csNativecsSegyTraceHeader_setFloatValue(int hdrIndex, float value);
DLLIMPORT void			cseis_csNativecsSegyTraceHeader_setDoubleValue(int hdrIndex, double value);
DLLIMPORT void			cseis_csNativecsSegyTraceHeader_setStringValue(int hdrIndex, string const& value);


void native_trace_header_deleter(void *header) {
	cseis_csNativecsSegyTraceHeader_deleteInstance(header);
}

segy_trace_header::segy_trace_header(shared_ptr<seismic_header_map> map) {
	this->map = map;
}

segy_trace_header::segy_trace_header(const segy_trace_header &header) {
    map                 = header.map;
    f_raw_data          = header.f_raw_data;
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
    }
    f_req_field_init        = header.f_req_field_init;
}

segy_trace_header& segy_trace_header::operator=(const segy_trace_header &obj) {
	map = obj.map;
    f_raw_data = obj.f_raw_data;
	return *this;
}

segy_trace_header::segy_trace_header(shared_ptr<seismic_header_map> map, 
    byte_t *raw_data, endian_order swap) {

    this->map = map;
    f_swap_endian = swap;
    f_raw_data.resize(segy_file::trace_header_size);
    copy(raw_data, raw_data + segy_file::trace_header_size, f_raw_data.data());
    parse_required();
}

void segy_trace_header::parse_required() {
    if (f_raw_data.size() != segy_file::trace_header_size)
        return;
    
    auto field = get("Inline");
    f_iline = any_cast<int>(field.first);
    field = get("Crossline");
    f_crossline = any_cast<int>(field.first);
    f_CDP_X = get("CDP X");
    f_CDP_Y = get("CDP Y");
    f_CDP = get("CDP");
    f_Src_X = get("Src X");
    f_Src_Y = get("Src Y");
    field = get("Samples count");
    f_samples_count = any_cast<short>(field.first);
    field = get("Sample interval");
    f_sample_interval = any_cast<short>(field.first);

    f_req_field_init = true;
}

int segy_trace_header::iline() {
    if (f_req_field_init)
        return f_iline;
    else
        return NOT_INDEX;
}

int segy_trace_header::crossline() {
    if (f_req_field_init)
        return f_crossline;
    else
        return NOT_INDEX;
}

std::pair<std::any, seismic_data_type>  segy_trace_header::CDP_X() {
    if (f_req_field_init)
        return f_CDP_X;
    else
        return { NOT_INDEX, seismic_data_type::UNKNOWN };
}

std::pair<std::any, seismic_data_type>  segy_trace_header::CDP_Y() {
    if (f_req_field_init)
        return f_CDP_Y;
    else
        return { NOT_INDEX, seismic_data_type::UNKNOWN };
}

std::pair<std::any, seismic_data_type>  segy_trace_header::CDP() {
    if (f_req_field_init)
        return f_CDP;
    else
        return { NOT_INDEX, seismic_data_type::UNKNOWN };
}

std::pair<std::any, seismic_data_type>  segy_trace_header::Src_X() {
    if (f_req_field_init)
        return f_Src_X;
    else
        return { NOT_INDEX, seismic_data_type::UNKNOWN };
}

std::pair<std::any, seismic_data_type>  segy_trace_header::Src_Y() {
    if (f_req_field_init)
        return f_Src_Y;
    else
        return { NOT_INDEX, seismic_data_type::UNKNOWN };
}

int segy_trace_header::samples_count() {
    if (f_req_field_init)
        return f_samples_count;
    else
        return NOT_INDEX;
}

double segy_trace_header::sample_interval() {
    if (f_req_field_init)
        return f_sample_interval;
    else
        return NOT_INDEX;
}

int segy_trace_header::count() const {
	return 0;
}

string segy_trace_header::description(int index) const {
	return string("");
}

string segy_trace_header::name(int index) const {
	return string("");
}

int segy_trace_header::index_of(const string &name) const {
	return 0;
}

seismic_data_type segy_trace_header::type(int index) const {
	return seismic_data_type::UNKNOWN;
}

pair<any, seismic_data_type> segy_trace_header::get(const string &name) const {
    if (map->contains(name)) {
        auto field_info = map->get_field(name);
        int pos = std::get<0>(field_info);
        int size = std::get<1>(field_info);
        auto type = std::get<2>(field_info);
        any res;

        switch (type) {
        case seismic_data_type::INT:
            if (size != 4)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            res = byte_to_int(&f_raw_data[pos], f_swap_endian);
            break;
        case seismic_data_type::SHORT:
            if (size != 2)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            res = byte_to_short(&f_raw_data[pos], f_swap_endian);
            break;
        case seismic_data_type::USHORT:
            if (size != 2)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            res = byte_to_ushort(&f_raw_data[pos], f_swap_endian);
            break;
        case seismic_data_type::INT64:
            if (size != 8)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            res = byte_to_int64(&f_raw_data[pos], f_swap_endian);
            break;
        case seismic_data_type::UINT64:
            if (size != 8)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            res = byte_to_uint64(&f_raw_data[pos], f_swap_endian);
            break;
        case seismic_data_type::FLOAT:
            if (size != 4)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            res = byte_to_float(&f_raw_data[pos], f_swap_endian);
            break;
        case seismic_data_type::DOUBLE:
            if (size != 8)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            res = byte_to_double(&f_raw_data[pos], f_swap_endian);
            break;
        case seismic_data_type::CHAR:
            if (size != 1)
                throw invalid_argument("segy_trace_header: get: invalid "
                    "byte size for field " + name);
            res = (char)f_raw_data[pos];
            break;
        default:
            throw invalid_argument("segy_trace_header: get: type for field " + name);
            break;
        }

        return { res, type };
    }
    else
        return { NOT_INDEX, seismic_data_type::UNKNOWN };
}

void segy_trace_header::set_field(int index, pair<any, seismic_data_type>) {
	throw runtime_error("segy_trace_header::set: method not implemented");
}

#ifdef PYTHON
void py_segy_trace_header_init(py::module &m,
	py::class_<segy_trace_header,
	shared_ptr<segy_trace_header>> &py_segy_trace_header) {

	py_segy_trace_header.def(py::init<shared_ptr<seismic_header_map>>(),
		py::arg("header_map")
	);
}
#endif
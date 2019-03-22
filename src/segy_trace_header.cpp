#include "segy_trace_header.h"
#include "utils.h"

#include "geolib_defines.h"

using namespace std;
using namespace cseis_geolib;


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
DLLIMPORT void			cseis_csNativecsSegyTraceHeader_setStringValue(int hdrIndex, std::string const& value);


void native_trace_header_deleter(void *header) {
	cseis_csNativecsSegyTraceHeader_deleteInstance(header);
}

segy_trace_header::segy_trace_header(const void *const native_header) {
	obj = shared_ptr<void>(
		cseis_csNativecsSegyTraceHeader_copyInstance(native_header),
		native_trace_header_deleter
		);
}

segy_trace_header::segy_trace_header(const segy_trace_header &header) {
	obj = header.obj;
}

segy_trace_header::~segy_trace_header() { }

segy_trace_header& segy_trace_header::operator=(const void *obj) {
	this->obj = shared_ptr<void>(const_cast<void*>(obj));
	return *this;
}

segy_trace_header& segy_trace_header::operator=(const segy_trace_header &obj) {
	this->obj = obj.obj;
	return *this;
}

int segy_trace_header::count() const {
	if (obj == nullptr)
		throw std::runtime_error("Object segy_trace_header is not initialized in native code");
	return cseis_csNativecsSegyTraceHeader_numHeaders(obj.get());
}

std::string segy_trace_header::description(int index) const {
	if (obj == nullptr)
		throw std::runtime_error("Object segy_trace_header is not initialized in native code");
	const char *const descr = cseis_csNativecsSegyTraceHeader_headerDesc(obj.get(), index);
	return std::string(descr);
}

std::string segy_trace_header::name(int index) const {
	if (obj == nullptr)
		throw std::runtime_error("Object segy_trace_header is not initialized in native code");
	const char *const name = cseis_csNativecsSegyTraceHeader_headerName(obj.get(), index);
	return std::string(name);
}

int segy_trace_header::index_of(const std::string &name) const {
	if (obj == nullptr)
		throw std::runtime_error("Object segy_trace_header is not initialized in native code");
	return cseis_csNativecsSegyTraceHeader_headerIndex(obj.get(), name.c_str());
}

seismic_data_type segy_trace_header::type(int index) const {
	if (obj == nullptr)
		throw std::runtime_error("Object segy_trace_header is not initialized in native code");
	cseis_geolib::type_t geolibtype = cseis_csNativecsSegyTraceHeader_headerType(obj.get(), index);
	return geolib_type_converter::convert<cseis_geolib::type_t, seismic_data_type>(geolibtype);
}

int			segy_trace_header::get_int(int index) const {
	if (obj == nullptr)
		throw std::runtime_error("Object segy_trace_header is not initialized in native code");
	return cseis_csNativecsSegyTraceHeader_intValue(obj.get(), index);
}

float		segy_trace_header::get_float(int index) const {
	if (obj == nullptr)
		throw std::runtime_error("Object segy_trace_header is not initialized in native code");
	return cseis_csNativecsSegyTraceHeader_floatValue(obj.get(), index);
}

double		segy_trace_header::get_double(int index) const {
	if (obj == nullptr)
		throw std::runtime_error("Object segy_trace_header is not initialized in native code");
	return cseis_csNativecsSegyTraceHeader_doubleValue(obj.get(), index);
}

/*
template <>
int segy_trace_header::get(int index) const {
	if (obj == nullptr)
		throw std::runtime_error("Object segy_trace_header is not initialized in native code");
	return cseis_csNativecsSegyTraceHeader_intValue(obj.get(), index);
}

template <>
float segy_trace_header::get(int index) const {
	if (obj == nullptr)
		throw std::runtime_error("Object segy_trace_header is not initialized in native code");
	return cseis_csNativecsSegyTraceHeader_floatValue(obj.get(), index);
}

template <>
double segy_trace_header::get(int index) const {
	if (obj == nullptr)
		throw std::runtime_error("Object segy_trace_header is not initialized in native code");
	return cseis_csNativecsSegyTraceHeader_doubleValue(obj.get(), index);
}

template <>
std::string segy_trace_header::get(int index) const {
	if (obj == nullptr)
		throw std::runtime_error("Object segy_trace_header is not initialized in native code");
	return cseis_csNativecsSegyTraceHeader_stringValue(obj.get(), index);
}
*/

#ifdef PYTHON

map<string, py::object> segy_trace_header::to_dict() {
	map<string, py::object> res;
	for (int i = 0; i < count(); ++i) {
		auto n = name(i);
		type_t t = type(i);
		py::object v;
		switch (t) {
			case TYPE_INT:
				v = py::int_(get<int>(i));
				break;
			case TYPE_FLOAT:
				v = py::float_(get<float>(i));
				break;
			case TYPE_DOUBLE:
				v = py::float_(get<double>(i));
				break;
			case TYPE_CHAR:
				v = py::str(get<string>(i));
				break;
			case TYPE_STRING:
				v = py::str(get<string>(i));
				break;
			case TYPE_INT64:
				throw std::runtime_error("Header value type int64 not supported yet");
				break;
			case TYPE_SHORT:
				throw std::runtime_error("Header value type int16 not supported yet");
				break;
			case TYPE_USHORT:
				throw std::runtime_error("Header value type uint16 not supported yet");
				break;
		default:
			v = py::float_(get<double>(i));
			break;
		}

		res[n] = v;
	}

	return res;
}

#endif
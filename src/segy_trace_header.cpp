#include "segy_trace_header.h"
#include "utils.h"

#include "geolib_defines.h"
#include "segy_header_map.h"

using namespace std;
using namespace cseis_geolib;

DLLIMPORT void*			cseis_csNativecsSegyTraceHeader_createInstance(const void *hdrMap);
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
	this->map = shared_ptr<segy_header_map>(new segy_header_map(map));
	obj = shared_ptr<void>(
		cseis_csNativecsSegyTraceHeader_createInstance(this->map.get()),
		native_trace_header_deleter
	);
}

segy_trace_header::segy_trace_header(const void *const native_header) {
	obj = shared_ptr<void>(
		cseis_csNativecsSegyTraceHeader_copyInstance(native_header),
		native_trace_header_deleter
		);
}

segy_trace_header::segy_trace_header(const segy_trace_header &header) {
	obj = shared_ptr<void>(
		cseis_csNativecsSegyTraceHeader_copyInstance(header.obj.get()),
		native_trace_header_deleter
		);
}

segy_trace_header& segy_trace_header::operator=(const void *obj) {
	this->obj = shared_ptr<void>(const_cast<void*>(obj), native_trace_header_deleter);
	return *this;
}

segy_trace_header& segy_trace_header::operator=(const segy_trace_header &obj) {
	this->obj = obj.obj;
	return *this;
}

int segy_trace_header::count() const {
	if (obj == nullptr)
		throw runtime_error("Object segy_trace_header is not initialized in native code");
	return cseis_csNativecsSegyTraceHeader_numHeaders(obj.get());
}

string segy_trace_header::description(int index) const {
	if (obj == nullptr)
		throw runtime_error("Object segy_trace_header is not initialized in native code");
	const char *const descr = cseis_csNativecsSegyTraceHeader_headerDesc(obj.get(), index);
	return string(descr);
}

string segy_trace_header::name(int index) const {
	if (obj == nullptr)
		throw runtime_error("Object segy_trace_header is not initialized in native code");
	const char *const name = cseis_csNativecsSegyTraceHeader_headerName(obj.get(), index);
	return string(name);
}

int segy_trace_header::index_of(const string &name) const {
	if (obj == nullptr)
		throw runtime_error("Object segy_trace_header is not initialized in native code");
	return cseis_csNativecsSegyTraceHeader_headerIndex(obj.get(), name.c_str());
}

seismic_data_type segy_trace_header::type(int index) const {
	if (obj == nullptr)
		throw runtime_error("Object segy_trace_header is not initialized in native code");
	cseis_geolib::type_t geolibtype = cseis_csNativecsSegyTraceHeader_headerType(obj.get(), index);
	return geolib_type_converter::convert<cseis_geolib::type_t, seismic_data_type>(geolibtype);
}

pair<any, seismic_data_type> segy_trace_header::get(int index) const {
	cseis_geolib::type_t geolibtype = cseis_csNativecsSegyTraceHeader_headerType(obj.get(), index);
	auto type = geolib_type_converter::convert<cseis_geolib::type_t, seismic_data_type>(geolibtype);

	any res;
	switch (type)
	{
	case seismic_data_type::INT:
		res = cseis_csNativecsSegyTraceHeader_intValue(obj.get(), index);
		break;
	case seismic_data_type::FLOAT:
		res = cseis_csNativecsSegyTraceHeader_floatValue(obj.get(), index);
		break;
	case seismic_data_type::DOUBLE:
		res = cseis_csNativecsSegyTraceHeader_doubleValue(obj.get(), index);
		break;
	default:
		throw runtime_error("segy_trace_header::get: type not implemented");
		break;
	}

	return { res, type };
}

void segy_trace_header::set_field(int index, pair<any, seismic_data_type>) {
	throw runtime_error("segy_trace_header::set: method not implemented");
}

#ifdef PYTHON

#endif
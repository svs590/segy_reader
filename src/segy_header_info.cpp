#include <string>

#include "segy_header_info.h"
#include "utils.h"

using namespace std;
using namespace cseis_geolib;

DLLIMPORT int				cseis_csNativeSegyHeaderInfo_getByteLoc(void *obj);
DLLIMPORT int				cseis_csNativeSegyHeaderInfo_getByteSize(void *obj);
DLLIMPORT type_t			cseis_csNativeSegyHeaderInfo_getInType(void *obj);
DLLIMPORT type_t			cseis_csNativeSegyHeaderInfo_getOutType(void *obj);
DLLIMPORT const char *		cseis_csNativeSegyHeaderInfo_getName(void *obj);
DLLIMPORT const char *		cseis_csNativeSegyHeaderInfo_getDescription(void *obj);
DLLIMPORT void				cseis_csNativeSegyHeaderInfo_setByteLoc(void *obj, int val);
DLLIMPORT void				cseis_csNativeSegyHeaderInfo_setByteSize(void *obj, int val);
DLLIMPORT void				cseis_csNativeSegyHeaderInfo_setInType(void *obj, type_t val);
DLLIMPORT void				cseis_csNativeSegyHeaderInfo_setOutType(void *obj, type_t val);


segy_traceheader_field::segy_traceheader_field(const void *obj) {
	this->obj = const_cast<void*>(obj);
}

segy_traceheader_field& segy_traceheader_field::operator=(const void *obj) {
	this->obj = const_cast<void*>(obj);
	return *this;
}

int segy_traceheader_field::byte_loc() {
	return cseis_csNativeSegyHeaderInfo_getByteLoc(obj);
}

int segy_traceheader_field::byte_size() {
	return cseis_csNativeSegyHeaderInfo_getByteSize(obj);
}

string segy_traceheader_field::name() {
	return string(cseis_csNativeSegyHeaderInfo_getName(obj));
}

string segy_traceheader_field::description() {
	return string(cseis_csNativeSegyHeaderInfo_getDescription(obj));
}

seismic_data_type segy_traceheader_field::type_in() {
	return geolib_type_converter::convert<type_t, seismic_data_type>(
			cseis_csNativeSegyHeaderInfo_getInType(obj)
		);
}

seismic_data_type segy_traceheader_field::type_out() {
	return geolib_type_converter::convert<type_t, seismic_data_type>(
			cseis_csNativeSegyHeaderInfo_getOutType(obj)
		);
}

void segy_traceheader_field::set_byte_loc(int byteLoc) {
	cseis_csNativeSegyHeaderInfo_setByteLoc(obj, byteLoc);
}

void segy_traceheader_field::set_byte_size(int byteSize) {
	cseis_csNativeSegyHeaderInfo_setByteSize(obj, byteSize);
}

void segy_traceheader_field::set_type_in(seismic_data_type type) {
	type_t geolibtype =
		geolib_type_converter::convert<seismic_data_type, type_t>(type);
	cseis_csNativeSegyHeaderInfo_setInType(obj, geolibtype);
}

void segy_traceheader_field::set_type_out(seismic_data_type type) {
	type_t geolibtype =
		geolib_type_converter::convert<seismic_data_type, type_t>(type);
	cseis_csNativeSegyHeaderInfo_setOutType(obj, geolibtype);
}

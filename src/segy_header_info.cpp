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

segy_header_info::segy_header_info(const void *obj) {
	this->obj = const_cast<void*>(obj);
}
segy_header_info& segy_header_info::operator=(const void *obj) {
	this->obj = const_cast<void*>(obj);
	return *this;
}

int segy_header_info::getByteLoc() {
	return cseis_csNativeSegyHeaderInfo_getByteLoc(obj);
}
int segy_header_info::getByteSize() {
	return cseis_csNativeSegyHeaderInfo_getByteSize(obj);
}
string segy_header_info::getName() {
	return string(cseis_csNativeSegyHeaderInfo_getName(obj));
}
string segy_header_info::getDescription() {
	return string(cseis_csNativeSegyHeaderInfo_getDescription(obj));
}
type_t segy_header_info::getInType() {
	return cseis_csNativeSegyHeaderInfo_getInType(obj);
}
type_t segy_header_info::getOutType() {
	return cseis_csNativeSegyHeaderInfo_getOutType(obj);
}

void segy_header_info::setByteLoc(int byteLoc) {
	cseis_csNativeSegyHeaderInfo_setByteLoc(obj, byteLoc);
}
void segy_header_info::setByteSize(int byteSize) {
	cseis_csNativeSegyHeaderInfo_setByteSize(obj, byteSize);
}
void segy_header_info::setInType(type_t type) {
	cseis_csNativeSegyHeaderInfo_setInType(obj, type);
}
void segy_header_info::setOutType(type_t type) {
	cseis_csNativeSegyHeaderInfo_setOutType(obj, type);
}

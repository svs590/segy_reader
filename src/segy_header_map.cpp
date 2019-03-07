#include <string>

#include "segy_header_map.h"
#include "utils.h"

using namespace std;
using namespace cseis_geolib;

DLLIMPORT int				cseis_csNativeSegyHdrMap_getMapID(void *obj);
DLLIMPORT bool				cseis_csNativeSegyHdrMap_copyHeader(void *obj, void *hdr);
DLLIMPORT bool				cseis_csNativeSegyHdrMap_addHeader(void *obj, const char *getName, int getByteLoc, int getByteSize, type_t inType, const char *desc);
DLLIMPORT bool				cseis_csNativeSegyHdrMap_removeHeaderByIndex(void *obj, int hdrIndex);
DLLIMPORT bool				cseis_csNativeSegyHdrMap_removeHeaderByName(void *obj, const char *getName);
DLLIMPORT void				cseis_csNativeSegyHdrMap_removeAll(void *obj);
DLLIMPORT int				cseis_csNativeSegyHdrMap_headerIndex(void *obj, const char *getName);
DLLIMPORT bool				cseis_csNativeSegyHdrMap_contains(void *obj, const char *getName, int* hdrIndex);
DLLIMPORT const void * const cseis_csNativeSegyHdrMap_headerByIndex(void *obj, int hdrIndex);
DLLIMPORT const void * const cseis_csNativeSegyHdrMap_headerByName(void *obj, const char *getName);
DLLIMPORT int				cseis_csNativeSegyHdrMap_numHeaders(void *obj);
DLLIMPORT int				cseis_csNativeSegyHdrMap_lockUserDefenition(void *obj);
DLLIMPORT int				cseis_csNativeSegyHdrMap_unlockUserDefenition(void *obj);

segy_header_map::segy_header_map(const void *obj) {
	this->obj = const_cast<void*>(obj);
}

segy_header_map& segy_header_map::operator=(const void *obj) {
	this->obj = const_cast<void*>(obj);
	return *this;
}

void segy_header_map::addHeader(const segy_header_map &header) {
	if (!cseis_csNativeSegyHdrMap_copyHeader(obj, header.obj))
		throw std::runtime_error("Error while adding header in map");
}
void segy_header_map::addHeader(string getName, int getByteLoc, int getByteSize, type_t inType, string desc) {
	cseis_csNativeSegyHdrMap_unlockUserDefenition(obj);
	if (!cseis_csNativeSegyHdrMap_addHeader(obj, getName.c_str(), getByteLoc, getByteSize, inType, desc.c_str()))
		throw std::runtime_error("Error while adding header in map");
	cseis_csNativeSegyHdrMap_lockUserDefenition(obj);
}
void segy_header_map::remove(int index) {
	if (!cseis_csNativeSegyHdrMap_removeHeaderByIndex(obj, index))
		throw std::runtime_error("Error while deleting header in map");
}
void segy_header_map::remove(string getName) {
	if (!cseis_csNativeSegyHdrMap_removeHeaderByName(obj, getName.c_str()))
		throw std::runtime_error("Error while deleting header from map");
}
void segy_header_map::removeAll() {
	cseis_csNativeSegyHdrMap_removeAll(obj);
}
int segy_header_map::getIndexOf(string getName) {
	return cseis_csNativeSegyHdrMap_headerIndex(obj, getName.c_str());
}
int segy_header_map::contains(string getName) {
	int index;
	if (!cseis_csNativeSegyHdrMap_contains(obj, getName.c_str(), &index))
		return NOT_INDEX;
	return index;
}
segy_header_info segy_header_map::headerInfo(int index) {
	return cseis_csNativeSegyHdrMap_headerByIndex(obj, index);
}
segy_header_info segy_header_map::headerInfo(string getName) {
	return cseis_csNativeSegyHdrMap_headerByName(obj, getName.c_str());
}
int segy_header_map::count() {
	return cseis_csNativeSegyHdrMap_numHeaders(obj);
}
int segy_header_map::mapId() {
	return cseis_csNativeSegyHdrMap_getMapID(obj);
}
#include <string>

#include "segy_header_map.h"
#include "segy_header_info.h"
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

void segy_header_map::set_field(shared_ptr<seismic_traceheader_field> header) {
	segy_traceheader_field *sgyfield = nullptr;

	switch (header->type_id())
	{
	case object_type::SEGY_TRACEHEADER_FIELD:
		sgyfield = reinterpret_cast<segy_traceheader_field*>(header.get());
		cseis_csNativeSegyHdrMap_copyHeader(obj, sgyfield->obj);
		break;
	default:
		add_field(
			header->name(), 
			header->byte_loc(),
			header->byte_size(),
			header->type_out(), 
			header->description()
		);
		break;
	}
}

void segy_header_map::add_field(
	string name, 
	int byte_loc,	
	int byte_size, 
	seismic_data_type type, 
	string desc) {

	cseis_csNativeSegyHdrMap_unlockUserDefenition(obj);
	cseis_geolib::type_t geolibtype =
		geolib_type_converter::convert<seismic_data_type, cseis_geolib::type_t>(type);

	int flag = cseis_csNativeSegyHdrMap_addHeader(
		obj,
		name.c_str(),
		byte_loc,
		byte_size,
		geolibtype,
		desc.c_str()
	);
	if (!flag)
		throw runtime_error("Error while adding header in map");
	cseis_csNativeSegyHdrMap_lockUserDefenition(obj);
}

void segy_header_map::remove(int index) {
	if (delete_check(index))
		if (!cseis_csNativeSegyHdrMap_removeHeaderByIndex(obj, index))
			throw runtime_error("Error while deleting header in map");
}

void segy_header_map::remove(const string &name) {
	if (delete_check(name))
		if (!cseis_csNativeSegyHdrMap_removeHeaderByName(obj, name.c_str()))
			throw runtime_error("Error while deleting header from map");
}

void segy_header_map::clear() {
	for (int i = 0; i < count(); ++i)
		remove(i);
}

int segy_header_map::index_of(const string &name) const {
	return cseis_csNativeSegyHdrMap_headerIndex(obj, name.c_str());
}

int segy_header_map::contains(const string &name) const {
	int index;
	if (!cseis_csNativeSegyHdrMap_contains(obj, name.c_str(), &index))
		return NOT_INDEX;
	return index;
}

shared_ptr<seismic_traceheader_field> segy_header_map::get_field(int index) const {
	return shared_ptr<seismic_traceheader_field>(
			new segy_traceheader_field(
				cseis_csNativeSegyHdrMap_headerByIndex(obj, index)
			)
		);
}

shared_ptr<seismic_traceheader_field> segy_header_map::get_field(const string &name) const {
	return shared_ptr<seismic_traceheader_field>(
			new segy_traceheader_field(
				cseis_csNativeSegyHdrMap_headerByName(obj, name.c_str())
			)
		);
}

int segy_header_map::count() const {
	return cseis_csNativeSegyHdrMap_numHeaders(obj);
}

header_map_type segy_header_map::type() const {
	return map_type;
}

bool segy_header_map::delete_check(int index) {
	if (index < 0 || index > count())
		return false;
	auto f = get_field(index);
	auto name = f->name();
	auto it = find(constant_fields_3d.begin(), constant_fields_3d.end(), name);
	if (it != constant_fields_3d.end())
		return true;
	return false;
}

bool segy_header_map::delete_check(const string name) {
	if (contains(name) < 0)
		return false;
	int index = index_of(name);
	return delete_check(index);
}

map<string, tuple<int, int, seismic_data_type, string>> segy_header_map::to_map() const {

	map<string, tuple<int, int, seismic_data_type, string>> res;
	for (int i = 0; i < count(); ++i) {
		auto f = get_field(i);
		res[f->name()] = make_tuple(
				f->byte_loc(), 
				f->byte_size(),
				f->type_out(), 
				f->description()
			);
	}
	return res;
}

void segy_header_map::set(
	const map<string, tuple<int, int, seismic_data_type, string>> &m) {

	clear();

	for (auto &p : m) {
		auto t = p.second;
		add_field(
			p.first,
			get<0>(t),
			get<1>(t),
			get<2>(t),
			get<3>(t)
		);
	}
}
#include <string>

#include "segy_header_map.h"
#include "segy_header_info.h"
#include "data_types.h"
#include "utils.h"

using namespace std;
using namespace cseis_geolib;

DLLIMPORT void*				cseis_csNativeSegyHdrMap_createInstance(int map_type);
DLLIMPORT void				cseis_csNativeSegyHdrMap_deleteInstance(void* obj);
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


void native_header_map_deleter(void *obj) {
	cseis_csNativeSegyHdrMap_deleteInstance(obj);
}

segy_header_map::segy_header_map(header_map_type type) {
	obj = shared_ptr<void>(
			cseis_csNativeSegyHdrMap_createInstance(
				geolib_type_converter::convert<header_map_type, const int>(type)
			),
			native_header_map_deleter
		);
}

segy_header_map::segy_header_map(const void *obj) {
	this->obj = shared_ptr<void>(
			const_cast<void*>(obj),
			native_header_map_deleter
		);
}

segy_header_map& segy_header_map::operator=(const void *obj) {
	this->obj = shared_ptr<void>(
			const_cast<void*>(obj),
			native_header_map_deleter
		);
	return *this;
}

segy_header_map::segy_header_map() : segy_header_map(header_map_type::STANDARD) {
	clear();
	map_type = header_map_type::CUSTOM;
}

segy_header_map::segy_header_map(const segy_header_map &map)
	: segy_header_map(map.type()) {
	map_type = map.type();
	for (int i = 0; i < map.count(); ++i)
		set_field(map.get_field(i));
}

segy_header_map::segy_header_map(shared_ptr<seismic_header_map> map) : segy_header_map() {
	set(map->to_map());
	map_type = map->type();
}

void segy_header_map::set_field(shared_ptr<seismic_traceheader_field> header) {
	segy_traceheader_field *sgyfield = nullptr;

	switch (header->type_id())
	{
	case object_type::SEGY_TRACEHEADER_FIELD:
		sgyfield = reinterpret_cast<segy_traceheader_field*>(header.get());
		cseis_csNativeSegyHdrMap_copyHeader(obj.get(), sgyfield->obj);
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

	map_type = header_map_type::CUSTOM;
}

void segy_header_map::add_field(
	string name, 
	int byte_loc,	
	int byte_size, 
	seismic_data_type type, 
	string desc) {

	cseis_csNativeSegyHdrMap_unlockUserDefenition(obj.get());
	cseis_geolib::type_t geolibtype =
		geolib_type_converter::convert<seismic_data_type, cseis_geolib::type_t>(type);

	int flag = cseis_csNativeSegyHdrMap_addHeader(
		obj.get(),
		name.c_str(),
		byte_loc,
		byte_size,
		geolibtype,
		desc.c_str()
	);
	if (!flag)
		throw runtime_error("Error while adding header in map");
	cseis_csNativeSegyHdrMap_lockUserDefenition(obj.get());

	map_type = header_map_type::CUSTOM;
}

void segy_header_map::remove(int index) {
	if (delete_check(index)) {
		if (!cseis_csNativeSegyHdrMap_removeHeaderByIndex(obj.get(), index))
			throw runtime_error("Error while deleting header in map");
		map_type = header_map_type::CUSTOM;
	}
}

void segy_header_map::remove(const string &name) {
	if (delete_check(name)) {
		if (!cseis_csNativeSegyHdrMap_removeHeaderByName(obj.get(), name.c_str()))
			throw runtime_error("Error while deleting header from map");
		map_type = header_map_type::CUSTOM;
	}
}

void segy_header_map::clear() {
	// Очень примитивно
	int i = 0;
	int n = count();
	while (n > constant_fields_3d.size()) {
		remove(i);
		if (count() < n)
			n = count();
		else
			i++;
	}
	map_type = header_map_type::CUSTOM;
}

int segy_header_map::index_of(const string &name) const {
	return cseis_csNativeSegyHdrMap_headerIndex(obj.get(), name.c_str());
}

int segy_header_map::contains(const string &name) const {
	int index;
	if (!cseis_csNativeSegyHdrMap_contains(obj.get(), name.c_str(), &index))
		return NOT_INDEX;
	return index;
}

shared_ptr<seismic_traceheader_field> segy_header_map::get_field(int index) const {
	return shared_ptr<seismic_traceheader_field>(
			new segy_traceheader_field(
				cseis_csNativeSegyHdrMap_headerByIndex(obj.get(), index)
			)
		);
}

shared_ptr<seismic_traceheader_field> segy_header_map::get_field(const string &name) const {
	return shared_ptr<seismic_traceheader_field>(
			new segy_traceheader_field(
				cseis_csNativeSegyHdrMap_headerByName(obj.get(), name.c_str())
			)
		);
}

int segy_header_map::count() const {
	return cseis_csNativeSegyHdrMap_numHeaders(obj.get());
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
		return false;
	return true;
}

bool segy_header_map::delete_check(const string name) {
	if (contains(name) < 0)
		return false;
	int index = index_of(name);
	return delete_check(index);
}

void segy_header_map::add_constant_fields() {
	for (auto i : constant_fields_3d) {
		add_field(i, 0, 0, seismic_data_type::UNKNOWN, "");
	}
}

map<string, tuple<int, int, seismic_data_type, string>> segy_header_map::to_map() const {

	map<string, tuple<int, int, seismic_data_type, string>> res;
	for (int i = 0; i < count(); ++i) {
		auto f = get_field(i);
		res[f->name()] = make_tuple(
				f->byte_loc(), 
				f->byte_size(),
				f->type_in(),
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

	map_type = header_map_type::CUSTOM;
}

#ifdef PYTHON
void py_segy_header_map_init(py::module &m,
	py::class_<segy_header_map, std::shared_ptr<segy_header_map>> &py_segy_header_map) {
		
	py_segy_header_map.def(py::init<>());
	py_segy_header_map.def(py::init<header_map_type>(),
		py::arg("map_type")
		);
}
#endif

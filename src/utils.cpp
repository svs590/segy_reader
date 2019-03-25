#include "utils.h"

#include <string>
#include <locale>
#include <codecvt>
#include <cassert>

#include "data_types.h"

using namespace std;


wstring utf8_to_unicode(const string& internal) {
	locale::global(locale("en_US.utf8"));
	auto& f = use_facet<codecvt<char, wchar_t, mbstate_t>>(locale());

	// note that the following can be done with wstring_convert
	mbstate_t mb{}; // initial shift state
	wstring external(internal.size() * f.max_length(), '\0');
	const char* from_next;
	wchar_t* to_next;
	f.out(mb, &internal[0], &internal[internal.size()], from_next,
		&external[0], &external[external.size()], to_next);
	// error checking skipped for brevity
	external.resize(to_next - &external[0]);
	return external;
}

template <>
seismic_data_type geolib_type_converter::convert(cseis_geolib::type_t type) {
	switch (type)
	{
	case cseis_geolib::TYPE_UNKNOWN:
		return seismic_data_type::UNKNOWN;
		break;
	case cseis_geolib::TYPE_EMPTY:
		return seismic_data_type::EMPTY;
		break;
	case cseis_geolib::TYPE_INT:
		return seismic_data_type::INT;
		break;
	case cseis_geolib::TYPE_FLOAT:
		return seismic_data_type::FLOAT;
		break;
	case cseis_geolib::TYPE_DOUBLE:
		return seismic_data_type::DOUBLE;
		break;
	case cseis_geolib::TYPE_CHAR:
		return seismic_data_type::CHAR;
		break;
	case cseis_geolib::TYPE_STRING:
		return seismic_data_type::STRING;
		break;
	case cseis_geolib::TYPE_INT64:
		return seismic_data_type::INT64;
		break;
	case cseis_geolib::TYPE_SHORT:
		return seismic_data_type::SHORT;
		break;
	case cseis_geolib::TYPE_USHORT:
		return seismic_data_type::USHORT;
		break;
	default:
		return seismic_data_type::UNKNOWN;
		break;
	}
}

template <>
cseis_geolib::type_t geolib_type_converter::convert(seismic_data_type type) {
	switch (type)
	{
	case seismic_data_type::UNKNOWN:
		return cseis_geolib::TYPE_UNKNOWN;
		break;
	case seismic_data_type::EMPTY:
		return cseis_geolib::TYPE_EMPTY;
		break;
	case seismic_data_type::INT:
		return cseis_geolib::TYPE_INT;
		break;
	case seismic_data_type::FLOAT:
		return cseis_geolib::TYPE_FLOAT;
		break;
	case seismic_data_type::DOUBLE:
		return cseis_geolib::TYPE_DOUBLE;
		break;
	case seismic_data_type::CHAR:
		return cseis_geolib::TYPE_CHAR;
		break;
	case seismic_data_type::STRING:
		return cseis_geolib::TYPE_STRING;
		break;
	case seismic_data_type::INT64:
		return cseis_geolib::TYPE_INT64;
		break;
	case seismic_data_type::SHORT:
		return cseis_geolib::TYPE_SHORT;
		break;
	case seismic_data_type::USHORT:
		return cseis_geolib::TYPE_USHORT;
		break;
	default:
		return cseis_geolib::TYPE_UNKNOWN;
		break;
	}
}

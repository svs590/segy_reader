#include "utils.h"

#include <string>
#include <locale>
#include <codecvt>
#include <cassert>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#undef small
#elif __linux__
// TODO
#endif

#include "data_types.h"
#include "csSegyHdrMap.h"

#include "seismic_header_map.h"

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

long long get_available_memory() {

#ifdef _WIN32
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	return statex.ullAvailPhys;
#elif __linux__
	// TODO
	return;
#endif

}

void seekg_relative(int64_t rel_pos, bfs::ifstream &stream) {
    // Complex algorithm to be able to make step that is larger than 2Gb : Make several smaller steps instead

    int maxInt = std::numeric_limits<int>::max() - 1;   // -1 to be on the safe side, also for negative byte positions
    if (rel_pos < 0) maxInt *= -1;

    csInt64_t numSteps = rel_pos / (int64_t)maxInt + 1LL;
    int bytePosResidual = (int)(rel_pos % (int64_t)maxInt);

    for (csInt64_t istep = 0; istep < numSteps - 1; ++istep) {
        stream.clear(); // Clear all flags
        stream.seekg(maxInt, std::ios_base::cur);
        if (stream.fail()) return;
    }

    stream.seekg(bytePosResidual, std::ios_base::cur);
}

bool is_little_endian() {
    union {
        unsigned char  cc[2];
        unsigned short int si;
    } aa;
    aa.si = 258;

    // Intel: aa.cc[0]=2  // LittleEndian
    // SPARC: aa.cc[0]=1  // BigEndian

    return(aa.cc[0] == 2);
}

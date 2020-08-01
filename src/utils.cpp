#include "utils.h"

#include <string>
#include <locale>
#include <codecvt>
#include <cassert>
#include <chrono>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#undef small
#elif __linux__
// TODO
#endif

#include "data_types.h"
#include "seismic_header_map.h"

using namespace std;


wstring utf8_to_unicode(const string& internal) {
	//locale::global(locale("en_US.utf8"));
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

string tab_to_space(const string& str) {
    string res = str;

    int pos = 0;
    int seq = 0;

    while (pos < res.size()) {
        if (res[pos] == '\n')
            seq = -1;

        if (res[pos] == '\t') {
            int count = 4 - (seq + 1) % 4;
            res[pos] = ' ';
            res = res.insert(pos + 1, count, ' ');
        }
        ++pos;
        ++seq;
    }

    return res;
}

long long get_available_memory() {

#ifdef _WIN32
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	return statex.ullAvailPhys;
#elif __linux__
	// TODO
	return 1024 * 1024 * 1024;
#endif

}

void seekg_relative(int64_t rel_pos, bfs::ifstream &stream) {
    // Complex algorithm to be able to make step that is larger than 2Gb : Make several smaller steps instead

    int maxInt = std::numeric_limits<int>::max() - 1;   // -1 to be on the safe side, also for negative byte positions
    if (rel_pos < 0) maxInt *= -1;

    int64_t numSteps = rel_pos / (int64_t)maxInt + 1LL;
    int bytePosResidual = (int)(rel_pos % (int64_t)maxInt);

    for (int64_t istep = 0; istep < numSteps - 1; ++istep) {
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

string date_time() {
    chrono::system_clock::time_point p = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(p);
    char str[26];
    ctime_s(str, sizeof str, &t);

    string res(str);
    return res.replace(res.end() - 1, res.end(), 1, ' ');
}

string first_to_upper(const string &s) {
    string res = s;
    res[0] = static_cast<string::value_type>(toupper(res[0]));
    return res;
}

string first_to_lower(const string &s) {
    string res = s;
    res[0] = static_cast<string::value_type>(tolower(res[0]));
    return res;
}

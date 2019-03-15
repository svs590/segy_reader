#include <vector>
#include <string>
#include <algorithm>

#include "geolib_defines.h"
#include "csSegyHeader.h"

#include "segy_reader.h"
#include "segy_bin_header.h"
#include "utils.h"

using namespace std;
using namespace cseis_geolib;

DLLIMPORT void* cseis_csNativeSegyReader_createInstance(
	const char *filename_in,
	int nTracesBuffer,
	int segyHeaderMap,
	bool reverseByteOrderData_in,
	bool reverseByteOrderHdr_in,
	bool autoscale_hdrs_in
);
DLLIMPORT void				cseis_csNativeSegyReader_deleteInstance(void *obj);
DLLIMPORT void				cseis_csNativeSegyReader_closeFile(void *obj);
DLLIMPORT int				cseis_SegyReader_sampleByteSize(void *obj);
DLLIMPORT int				cseis_SegyReader_numTraces(void *obj);
DLLIMPORT int				cseis_SegyReader_numSamples(void *obj);
DLLIMPORT int				cseis_SegyReader_numHeaders(void *obj);
DLLIMPORT float				cseis_SegyReader_sampleInterval(void *obj);
DLLIMPORT const void *		cseis_SegyReader_binHeader(void *obj);
DLLIMPORT const char *		cseis_csSegyReader_1charHeader(void *obj);
DLLIMPORT int				cseis_csReader_1hdrIntValue(void *obj, int hdrIndex);
DLLIMPORT float				cseis_csSegyReader_1hdrFloatValue(void *obj, int hdrIndex);
DLLIMPORT double			cseis_csSegyReader_1hdrDoubleValue(void *obj, int hdrIndex);
DLLIMPORT bool				cseis_csSegyReader_1moveToTrace(void *obj, int firstTraceIndex, int numTracesToRead);
DLLIMPORT const float *		cseis_csSegyReader_1getNextTrace(void *obj);
DLLIMPORT const void * const cseis_csSegyReader_getTrcHdrMap(void *obj);
DLLIMPORT void				cseis_csNativeSegyReader_charBinHeader(void *obj, char *buf);
DLLIMPORT bool				cseis_csSegyReader_2moveToTrace(void *obj, int firstTraceIndex);

segy_reader::segy_reader(const void *obj) {
	this->obj = const_cast<void*>(obj);
}

segy_reader::segy_reader(const segy_reader &obj) {
	this->obj = obj.obj;
}

segy_reader::~segy_reader() {
	cseis_csNativeSegyReader_deleteInstance(obj);
}

segy_reader& segy_reader::operator=(const void *obj) {
	this->obj = const_cast<void*>(obj);
	samples_count = NOT_INDEX;
	return *this;
}

segy_reader::segy_reader(
	string filename_in,
	int nTracesBuffer,
	int segyHeaderMap,
	bool reverseByteOrderData_in,
	bool reverseByteOrderHdr_in,
	bool autoscale_hdrs_in
) {
	obj = cseis_csNativeSegyReader_createInstance(
		filename_in.c_str(),
		nTracesBuffer,
		segyHeaderMap,
		reverseByteOrderData_in,
		reverseByteOrderHdr_in,
		autoscale_hdrs_in
	);
}

void segy_reader::closeFile() {
	cseis_csNativeSegyReader_closeFile(obj);
}
int segy_reader::sampleByteSize() {
	return cseis_SegyReader_sampleByteSize(obj);
}
int segy_reader::tracesCount() {
	return cseis_SegyReader_numTraces(obj);
}
int segy_reader::samplesCount() {
	if (samples_count == NOT_INDEX)
		samples_count = cseis_SegyReader_numSamples(obj);
	return samples_count;
}
int segy_reader::headersCount() {
	return cseis_SegyReader_numHeaders(obj);
}
float segy_reader::sampleInterval() {
	return cseis_SegyReader_sampleInterval(obj);
}
segy_bin_header segy_reader::binHeader() {
	return cseis_SegyReader_binHeader(obj);
}

void segy_reader::moveToTrace(int firstTraceIndex, int numTracesToRead) {
	if (!cseis_csSegyReader_1moveToTrace(obj, firstTraceIndex, numTracesToRead))
		throw std::runtime_error("Error while moving to trace " +
			to_string(firstTraceIndex));
}
void segy_reader::moveToTrace(int firstTraceIndex) {
	if (!cseis_csSegyReader_2moveToTrace(obj, firstTraceIndex))
		throw std::runtime_error("Error while moving to trace " +
			to_string(firstTraceIndex));
}
const float *segy_reader::nextTraceRef() {
	return cseis_csSegyReader_1getNextTrace(obj);
}
std::vector<float> segy_reader::getNextTrace() {
	const float *nativeTrace = nextTraceRef();
	if (nativeTrace == nullptr)
		return {};

	samplesCount();
	std::vector<float> trace(samples_count);
	std::memcpy((void*)trace.data(), (void*)nativeTrace, samples_count * sizeof(float));
	return trace;
}
std::vector<float> segy_reader::getTrace(int index) {
	moveToTrace(index, 1);
	return getNextTrace();
}

segy_header_map segy_reader::traceHeaderMap() {
	return cseis_csSegyReader_getTrcHdrMap(obj);
}
string segy_reader::charBinHeader() {
	char *buf = new char[csSegyHeader::SIZE_CHARHDR];
	cseis_csNativeSegyReader_charBinHeader(obj, buf);
	auto res = string(buf);
	delete[] buf;
	return res;
}

std::vector<float> segy_reader::iline(int il_num) {
	if (sorting != segy_sorting::unknown)
		if (!(il_num >= lineMap.il_start) ||
			!(il_num < lineMap.il_start + lineMap.il_count))
			throw std::invalid_argument("invalid iline number.");

	samplesCount();

	std::vector<float> res;

	int startTrace = (il_num - lineMap.il_start) * lineMap.il_offset;
	int count = lineMap.xl_count;
	int offset;
	int buffer;

	switch (sorting) {
	case segy_sorting::iline:
		offset = lineMap.xl_offset;
		buffer = 1; // lineMap.xl_count;
		break;
	case segy_sorting::xline:
		offset = lineMap.il_offset;
		buffer = 1;
		break;
	case segy_sorting::unsorted:
		throw std::exception("reading lines from unsorted .segy file still not inplemented.");
		break;
	case segy_sorting::unknown:
		determineSorting();
		res = iline(il_num);
		break;
	default:
		return {};
		break;
	}

	getLine(iline_trcs[il_num], buffer, res);
	return res;
}

std::vector<float> segy_reader::xline(int xl_num) {
	if (sorting != segy_sorting::unknown)
		if (!(xl_num >= lineMap.xl_start) ||
			!(xl_num < lineMap.xl_start + lineMap.xl_count))
			throw std::invalid_argument("invalid xline number.");

	samplesCount();

	std::vector<float> res;

	int startTrace = (xl_num - lineMap.xl_start) * lineMap.xl_offset;
	int count = lineMap.il_count;
	int offset;
	int buffer;

	switch (sorting) {
	case segy_sorting::iline:
		offset = lineMap.il_offset;
		buffer = 1;
		break;

	case segy_sorting::xline:
		offset = lineMap.xl_offset;
		buffer = 1; // lineMap.il_count;
		break;
	case segy_sorting::unsorted:
		throw std::exception("reading lines from unsorted .segy file still not inplemented.");
		break;
	case segy_sorting::unknown:
		determineSorting();
		res = xline(xl_num);
		break;
	default:
		return {};
		break;
	}

	getLine(xline_trcs[xl_num], buffer, res);
	return res;
}

void segy_reader::getLine(const vector<int> &trcs, int trc_buffer, vector<float> &line) {
	line.resize(trcs.size() * samples_count);
	//moveToTrace(trcs[0], trc_buffer);
	for (int i = 0; i < trcs.size(); ++i) {
		moveToTrace(trcs[i], trc_buffer);
		auto trace = nextTraceRef();
		memcpy(&line[i * samples_count], trace, samples_count * sizeof(float));
	}
}

template <>
int segy_reader::headerValue(int index) {
	return cseis_csReader_1hdrIntValue(obj, index);
}
template <>
float segy_reader::headerValue(int index) {
	return cseis_csSegyReader_1hdrFloatValue(obj, index);
}
template <>
double segy_reader::headerValue(int index) {
	return cseis_csSegyReader_1hdrDoubleValue(obj, index);
}

void segy_reader::determineSorting() {
	if (sorting != segy_sorting::unknown)
		return;

	lineMap.il_count = NOT_INDEX;
	lineMap.xl_count = NOT_INDEX;
	lineMap.il_offset = NOT_INDEX;
	lineMap.xl_offset = NOT_INDEX;

	segy_header_map hdrMap = traceHeaderMap();
	int hdrRowPos = hdrMap.contains("row");
	int hdrColPos = hdrMap.contains("col");

	int il, il_prev;
	int xl, xl_prev;

	int il_min = numeric_limits<int>::max();
	int xl_min = numeric_limits<int>::max();
	int il_max = 0;
	int xl_max = 0;

	int il_count = 1;
	int xl_count = 1;
	int il_offset = 1;
	int xl_offset = 1;

	moveToTrace(0, 1);
	nextTraceRef();

	il_prev = headerValue<int>(hdrRowPos);
	xl_prev = headerValue<int>(hdrColPos);

	iline_trcs[il_prev].push_back(0);
	xline_trcs[xl_prev].push_back(0);

	for (int i = 1; i < tracesCount(); ++i) {
		nextTraceRef();

		il = headerValue<int>(hdrRowPos);
		xl = headerValue<int>(hdrColPos);

		iline_trcs[il].push_back(i);
		xline_trcs[xl].push_back(i);

		if (il < il_min)
			il_min = il;
		if (il > il_max)
			il_max = il;
		if (xl < xl_min)
			xl_min = xl;
		if (xl > xl_max)
			xl_max = xl;

		if (il_prev == il && xl_prev != xl) {
			if (sorting == segy_sorting::unknown)
				sorting = segy_sorting::iline;
			else if (sorting == segy_sorting::xline) {
				sorting = segy_sorting::unsorted;
			}
		}

		if (il_prev != il && xl_prev == xl) {
			if (sorting == segy_sorting::unknown)
				sorting = segy_sorting::xline;
			else if (sorting == segy_sorting::iline) {
				sorting = segy_sorting::unsorted;
			}
		}

		il_prev = il;
		xl_prev = xl;
	}

	lineMap.il_count = il_max - il_min;
	lineMap.xl_count = xl_max - xl_min;
}

#ifdef PYTHON

void vector_to_numpy(vector<float> &x, py::array_t<float> y) {
	y.resize({ x.size() });
	memcpy((void*)y.data(), (void*)x.data(), x.size() * sizeof(float));
}

py::array_t<float> segy_reader::pyGetNextTrace() {
	py::array_t<float> trace;

	const float *nativeTrace = nextTraceRef();
	if (nativeTrace == nullptr)
		return trace;

	samplesCount();
	trace.resize({ samples_count });
	std::memcpy((void*)trace.data(), (void*)nativeTrace, samples_count * sizeof(float));
	return trace;
}

py::array_t<float> segy_reader::pyIline(int il_num) {
	py::array_t<float> line;
	auto il = iline(il_num);

	vector_to_numpy(il, line);
	line.resize({ lineMap.xl_count, samples_count });

	return line;
}

py::array_t<float> segy_reader::pyXline(int xl_num) {
	py::array_t<float> line;
	auto xl = xline(xl_num);

	vector_to_numpy(xl, line);
	line.resize({ lineMap.il_count, samples_count });

	return line;
}

#endif
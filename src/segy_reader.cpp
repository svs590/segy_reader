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
std::string segy_reader::charBinHeader() {
	char *buf = new char[csSegyHeader::SIZE_CHARHDR];
	cseis_csNativeSegyReader_charBinHeader(obj, buf);
	auto res = std::string(buf);
	delete[] buf;
	return res;
}

std::vector<float> segy_reader::iline(int il_num) {
	if (!(il_num >= lineMap.il_start) ||
		!(il_num < lineMap.il_start + lineMap.il_count))
		throw std::invalid_argument("invalid iline number.");

	samplesCount();

	std::vector<float> res;

	int startTrace = (il_num - lineMap.il_start) * lineMap.il_offset;
	int count = lineMap.xl_count;
	int offset;

	switch (sorting) {
	case segy_sorting::iline:
		offset = lineMap.xl_offset;
		break;
	case segy_sorting::xline:
		offset = lineMap.il_offset;
		break;
	case segy_sorting::unsorting:
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

	getLineFromSorted(startTrace, count, offset, res);
	return res;
}

std::vector<float> segy_reader::xline(int xl_num) {
	if (!(xl_num >= lineMap.xl_start) ||
		!(xl_num < lineMap.xl_start + lineMap.xl_count))
		throw std::invalid_argument("invalid xline number.");

	samplesCount();

	std::vector<float> res;

	int startTrace = (xl_num - lineMap.xl_start) * lineMap.xl_offset;
	int count = lineMap.il_count;
	int offset;

	switch (sorting) {
	case segy_sorting::iline:
		offset = lineMap.il_offset;
		break;

	case segy_sorting::xline:
		offset = lineMap.xl_offset;
		break;
	case segy_sorting::unsorting:
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

	getLineFromSorted(startTrace, count, offset, res);
	return res;
}

void segy_reader::getLineFromSorted(int startTrace, int count, int offset, vector<float> &line) {
	line.resize(count * samples_count);
	//moveToTrace(startTrace, offset + 1);
	for (int i = 0; i < count; ++i) {
		moveToTrace(startTrace + i * offset, offset + 1);
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

	moveToTrace(0, tracesCount());
	nextTraceRef();

	int il, il_prev;
	int xl, xl_prev;

	il_prev = headerValue<int>(hdrRowPos);
	xl_prev = headerValue<int>(hdrColPos);

	lineMap.il_start = il_prev;
	lineMap.xl_start = xl_prev;

	int il_count = 1;
	int xl_count = 1;
	int il_offset = 1;
	int xl_offset = 1;

	for (int i = 0; i < tracesCount(); ++i) {
		nextTraceRef();

		il = headerValue<int>(hdrRowPos);
		xl = headerValue<int>(hdrColPos);

		if (il_prev == il && xl_prev != xl) {
			il_offset++;

			if (sorting == segy_sorting::unknown)
				sorting = segy_sorting::iline;
			else if (sorting == segy_sorting::xline) {
				sorting = segy_sorting::unsorting;
				break;
			}

			xl_count++;
			xl_offset = xl - xl_prev;
		}

		if (il_prev != il && xl_prev == xl) {
			xl_offset++;

			if (sorting == segy_sorting::unknown)
				sorting = segy_sorting::xline;
			else if (sorting == segy_sorting::iline) {
				sorting = segy_sorting::unsorting;
				break;
			}

			il_count++;
			il_offset = xl - xl_prev;
		}

		if (sorting == segy_sorting::iline)
			if (il_prev != il) {
				if (lineMap.xl_count == NOT_INDEX)
					lineMap.xl_count = xl_count;
				else if (lineMap.xl_count != xl_count) {
					sorting = segy_sorting::unsorting;
					break;
				}
				if (lineMap.xl_offset == NOT_INDEX)
					lineMap.xl_offset = xl_offset;
				else if (lineMap.xl_offset != xl_offset) {
					sorting = segy_sorting::unsorting;
					break;
				}
				if (lineMap.il_offset == NOT_INDEX)
					lineMap.il_offset = il_offset;
				else if (lineMap.il_offset != il_offset) {
					sorting = segy_sorting::unsorting;
					break;
				}

				il_count++;
				xl_count = 1;
				xl_offset = 1;
				il_offset = 1;
			}
			else {}
		else if (sorting == segy_sorting::xline)
			if (xl_prev != xl) {
				if (lineMap.il_count == NOT_INDEX)
					lineMap.il_count = il_count;
				else if (lineMap.il_count != il_count) {
					sorting = segy_sorting::unsorting;
					break;
				}
				if (lineMap.il_offset == NOT_INDEX)
					lineMap.il_offset = il_offset;
				else if (lineMap.il_offset != il_offset) {
					sorting = segy_sorting::unsorting;
					break;
				}
				if (lineMap.xl_offset == NOT_INDEX)
					lineMap.xl_offset = xl_offset;
				else if (lineMap.xl_offset != xl_offset) {
					sorting = segy_sorting::unsorting;
					break;
				}

				xl_count++;
				il_count = 1;
				il_offset = 1;
				xl_offset = 1;
			}

		il_prev = il;
		xl_prev = xl;
	}

	if (sorting == segy_sorting::iline)
		lineMap.il_count = il_count;
	else if (sorting == segy_sorting::xline)
		lineMap.xl_count = xl_count;
}

void segy_reader::create_mem_map() {
	determineSorting();
}
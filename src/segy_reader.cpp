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
DLLIMPORT const void*const	cseis_csNativeSegyReader_getTraceHeader(void *obj);
DLLIMPORT const char*		cseis_csNativeSegyReader_getFileName(void *obj);

segy_reader::segy_reader(const void *obj) {
	this->obj = const_cast<void*>(obj);
	filename = std::string(cseis_csNativeSegyReader_getFileName(this->obj));
}

segy_reader::segy_reader(const segy_reader &obj) {
	this->obj = obj.obj;
}

segy_reader::~segy_reader() {
	cseis_csNativeSegyReader_deleteInstance(obj);
}

segy_reader& segy_reader::operator=(const void *obj) {
	this->obj = const_cast<void*>(obj);
	f_samples_count = NOT_INDEX;
	return *this;
}

segy_reader::segy_reader(
	string filename_in,
	int nTracesBuffer,
	int segyHeaderMap,
	bool reverseByteOrderData_in,
	bool reverseByteOrderHdr_in,
	bool autoscale_hdrs_in
) : seismic_data_provider(filename_in) {

	obj = cseis_csNativeSegyReader_createInstance(
		filename_in.c_str(),
		nTracesBuffer,
		segyHeaderMap,
		reverseByteOrderData_in,
		reverseByteOrderHdr_in,
		autoscale_hdrs_in
	);
}

void segy_reader::close() {
	cseis_csNativeSegyReader_closeFile(obj);
}

int segy_reader::sampleByteSize() {
	return cseis_SegyReader_sampleByteSize(obj);
}

int segy_reader::traces_count() {
	return cseis_SegyReader_numTraces(obj);
}

int segy_reader::samples_count() {
	if (f_samples_count == NOT_INDEX)
		f_samples_count = cseis_SegyReader_numSamples(obj);
	return f_samples_count;
}

int segy_reader::headersCount() {
	return cseis_SegyReader_numHeaders(obj);
}

float segy_reader::sample_interval() {
	return cseis_SegyReader_sampleInterval(obj);
}

shared_ptr<seismic_abstract_header> segy_reader::bin_header() {
	return shared_ptr<seismic_abstract_header>(
			new segy_bin_header(cseis_SegyReader_binHeader(obj))
		);
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

	samples_count();
	std::vector<float> trace(f_samples_count);
	std::memcpy((void*)trace.data(), (void*)nativeTrace, f_samples_count * sizeof(float));
	return trace;
}

std::vector<float> segy_reader::get_trace_data(int index) {
	moveToTrace(index, 1);
	return getNextTrace();
}

shared_ptr<seismic_trace> segy_reader::get_trace(int index) {
	//TODO: Для оптимизации необходимо определять, что лучше: move или read в зависимости от размера прыжка.
	moveToTrace(index, 1);
	return shared_ptr<segy_trace>(
		new segy_trace(
			segy_trace_header(cseis_csNativeSegyReader_getTraceHeader(obj)),
			getNextTrace()
			)
		);
}

shared_ptr<seismic_header_map> segy_reader::header_map() {
	return shared_ptr<seismic_header_map>(
			new segy_header_map(cseis_csSegyReader_getTrcHdrMap(obj))
		);
}
string segy_reader::text_header() {
	char *buf = new char[csSegyHeader::SIZE_CHARHDR];
	cseis_csNativeSegyReader_charBinHeader(obj, buf);
	auto res = string(buf);
	delete[] buf;
	return res;
}

shared_ptr<seismic_trace_header> segy_reader::trace_header(int index) {
	moveToTrace(index);
	nextTraceRef();
	return current_trace_header();
}

shared_ptr<seismic_trace_header> segy_reader::current_trace_header() {
	return shared_ptr<seismic_trace_header>(
		new segy_trace_header(cseis_csNativeSegyReader_getTraceHeader(obj))
		);
}

std::vector<segy_trace> segy_reader::iline(int il_num) {
	if (sorting != segy_sorting::unknown)
		if (!(il_num >= geometry.il_start) ||
			!(il_num < geometry.il_start + geometry.il_count))
			throw std::invalid_argument("invalid iline number.");

	samples_count();

	vector<segy_trace> res;

	int startTrace = (il_num - geometry.il_start) * geometry.il_offset;
	int count = geometry.xl_count;
	int offset;
	int buffer;

	switch (sorting) {
	case segy_sorting::iline:
		offset = geometry.xl_offset;
		buffer = 1; // lineMap.xl_count;
		break;
	case segy_sorting::xline:
		offset = geometry.il_offset;
		buffer = 1;
		break;
	case segy_sorting::unsorted:
		throw std::exception("reading lines from unsorted .segy file still not inplemented.");
		break;
	case segy_sorting::unknown:
		preprocessing();
		res = iline(il_num);
		break;
	default:
		return {};
		break;
	}

	getLine(iline_trcs[il_num], buffer, res);
	return res;
}

std::vector<segy_trace> segy_reader::xline(int xl_num) {
	if (sorting != segy_sorting::unknown)
		if (!(xl_num >= geometry.xl_start) ||
			!(xl_num < geometry.xl_start + geometry.xl_count))
			throw std::invalid_argument("invalid xline number.");

	samples_count();

	vector<segy_trace> res;

	int startTrace = (xl_num - geometry.xl_start) * geometry.xl_offset;
	int count = geometry.il_count;
	int offset;
	int buffer;

	switch (sorting) {
	case segy_sorting::iline:
		offset = geometry.il_offset;
		buffer = 1;
		break;

	case segy_sorting::xline:
		offset = geometry.xl_offset;
		buffer = 1; // lineMap.il_count;
		break;
	case segy_sorting::unsorted:
		throw std::exception("reading lines from unsorted .segy file still not inplemented.");
		break;
	case segy_sorting::unknown:
		preprocessing();
		res = xline(xl_num);
		break;
	default:
		return {};
		break;
	}

	getLine(xline_trcs[xl_num], buffer, res);
	return res;
}

void segy_reader::getLine(const vector<int> &trcs, int trc_buffer, std::vector<segy_trace> &line) {
	line.resize(trcs.size());
	//moveToTrace(trcs[0], trc_buffer);
	for (int i = 0; i < trcs.size(); ++i) {
		moveToTrace(trcs[i], trc_buffer);
		auto trace = nextTraceRef();
		auto header = current_trace_header();
		line[i] = segy_trace(
			shared_ptr<segy_trace_header>(static_cast<segy_trace_header*>(header.get())),
			samples_count(), 
			trace
		);
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

void segy_reader::preprocessing() {
	if (sorting != segy_sorting::unknown)
		return;

	geometry.il_count = NOT_INDEX;
	geometry.xl_count = NOT_INDEX;
	geometry.il_offset = NOT_INDEX;
	geometry.xl_offset = NOT_INDEX;

	auto header_map_p = dynamic_pointer_cast<segy_header_map>(header_map());
	int hdrRowPos = header_map_p->contains("row");
	int hdrColPos = header_map_p->contains("col");

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

	for (int i = 1; i < traces_count(); ++i) {
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

	geometry.il_start = il_min;
	geometry.xl_start = xl_min;
	geometry.il_count = il_max - il_min;
	geometry.xl_count = xl_max - xl_min;
}

segy_line::segy_line(
	int il_index,
	int xl_index,
	int cdpx_index,
	int cdpy_index,
	int srcx_index,
	int srcy_index
) {
	this->il_index		= il_index;
	this->xl_index		= xl_index;
	this->cdpx_index	= cdpx_index;
	this->cdpy_index	= cdpy_index;
	this->srcx_index	= srcx_index;
	this->srcy_index	= srcy_index;
}

#ifdef PYTHON

void vector_to_numpy(vector<float> &x, py::array_t<float> y) {
	y.resize({ x.size() });
	memcpy((void*)y.data(), (void*)x.data(), x.size() * sizeof(float));
}

segy_trace segy_reader::py_get_trace() {
	const float *nativeTrace = nextTraceRef();
	
	samplesCount();

	return segy_trace(trace_header(), samples_count, nativeTrace);
}

#endif
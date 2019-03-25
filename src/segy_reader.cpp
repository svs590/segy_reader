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

	geometry = shared_ptr<seismic_geometry_info>(new seismic_geometry_info);
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

vector<seismic_trace> segy_reader::get_traces(seismic_line_info line_info) {
	preprocessing();
	samples_count();

	vector<segy_trace> res;

	if (line_info.by_bounds) {
		vector<segy_trace> buffer;
		for (int i = 0; i < line_info.bounds.size(); ++i) {
			get_traces(line_info.bounds[i].first, line_info.bounds[i].second, buffer);
			res.insert(res.end(), buffer.begin(), buffer.end());
		}
	}
	else {
		get_traces(line_info.traces_indexes, 1, res);
	}

	return res;
}

//std::vector<segy_trace> segy_reader::xline(int xl_num) {
//	if (sorting != segy_sorting::unknown)
//		if (!(xl_num >= seismic_geometry_info.xl_start) ||
//			!(xl_num < seismic_geometry_info.xl_start + seismic_geometry_info.xl_count))
//			throw std::invalid_argument("invalid xline number.");
//
//	samples_count();
//
//	vector<segy_trace> res;
//
//	int startTrace = (xl_num - seismic_geometry_info.xl_start) * seismic_geometry_info.xl_offset;
//	int count = seismic_geometry_info.il_count;
//	int offset;
//	int buffer;
//
//	switch (sorting) {
//	case segy_sorting::iline:
//		offset = seismic_geometry_info.il_offset;
//		buffer = 1;
//		break;
//
//	case segy_sorting::xline:
//		offset = seismic_geometry_info.xl_offset;
//		buffer = 1; // lineMap.il_count;
//		break;
//	case segy_sorting::unsorted:
//		throw std::exception("reading lines from unsorted .segy file still not inplemented.");
//		break;
//	case segy_sorting::unknown:
//		preprocessing();
//		res = xline(xl_num);
//		break;
//	default:
//		return {};
//		break;
//	}
//
//	get_traces(xline_trcs[xl_num], buffer, res);
//	return res;
//}

void segy_reader::get_traces(const vector<int> &trcs, int trc_buffer, std::vector<segy_trace> &line) {
	line.resize(trcs.size());
	//moveToTrace(trcs[0], trc_buffer);
	for (int i = 0; i < trcs.size(); ++i) {
		moveToTrace(trcs[i], trc_buffer);
		auto trace = nextTraceRef();
		auto header = current_trace_header();
		line[i] = segy_trace(
			dynamic_pointer_cast<segy_trace_header>(header),
			samples_count(),
			trace
		);
	}
}

void segy_reader::get_traces(int start_trace, int end_trace, std::vector<segy_trace> &line) {
	int count = end_trace - start_trace + 1;
	int buffer_size = end_trace - start_trace + 1;
	line.resize(count);
	moveToTrace(start_trace, buffer_size);

	for (int i = 0; i < count; ++i) {
		auto trace = nextTraceRef();
		auto header = current_trace_header();
		line[i] = segy_trace(
			dynamic_pointer_cast<segy_trace_header>(header),
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

void segy_reader::line_processing(
	map<int, seismic_line_info> &line, 
	seismic_line_info::seismic_line_type type,
	string name_prefix, 
	int line_no, 
	int trace_no,
	pair<float, float> point) {

	if (line[line_no].start_trace == NOT_INDEX) {
		line[line_no].name = name_prefix + " " + to_string(line_no);
		line[line_no].start_trace = trace_no;
		line[line_no].end_trace = trace_no;
		line[line_no].traces_indexes.push_back(trace_no);
		line[line_no].start = point;
		line[line_no].type = type;
	}
	else {
		if (line[line_no].end_trace + 1 == trace_no) {
			line[line_no].sequential_traces_count++;
			if (line[line_no].sequential_traces_count > 3 && !line[line_no].by_bounds) {
				line[line_no].by_bounds = true;
				line[line_no].bounds.push_back({ line[line_no].start_trace, trace_no });
				line[line_no].traces_indexes.clear();
				line[line_no].traces_indexes.shrink_to_fit();
			}
			else if (line[line_no].by_bounds) {
				auto last_bound = --line[line_no].bounds.end();
				last_bound->second = trace_no;
			}
			else
				line[line_no].traces_indexes.push_back(trace_no);
		}
		else {
			if (line[line_no].by_bounds)
				line[line_no].bounds.push_back({ trace_no, trace_no });
			else
				line[line_no].traces_indexes.push_back(trace_no);
		}
		line[line_no].end_trace = trace_no;
		line[line_no].end = point;
	}
}

void segy_reader::preprocessing() {
	if (processed)
		return;

	bool x_coord_present = false;
	bool y_coord_present = false;
	float x = 0, y = 0;
	int il, xl;

	map<int, seismic_line_info> iline;
	map<int, seismic_line_info> xline;

	auto header_map_p = dynamic_pointer_cast<segy_header_map>(header_map());
	int row_index = header_map_p->contains("row");
	int col_index = header_map_p->contains("col");
	int x_index = header_map_p->contains("bin_x");
	int y_index = header_map_p->contains("bin_y");
	if (x_index != NOT_INDEX)
		x_coord_present = true;
	if (y_index != NOT_INDEX)
		y_coord_present = true;
	int count = traces_count();

	moveToTrace(0, 1);

	int i = 0;
	do {
		nextTraceRef();

		il = headerValue<int>(row_index);
		xl = headerValue<int>(col_index);

		if (x_coord_present)
			x = headerValue<float>(x_index);
		if (y_coord_present)
			y = headerValue<float>(y_index);

		line_processing(iline, 
			seismic_line_info::seismic_line_type::iline, "Iline", il, i, { x, y });
		line_processing(xline,
			seismic_line_info::seismic_line_type::xline, "Xline", xl, i, { x, y });
		++i;
	} while (i < count);

	std::transform(
		iline.begin(),
		iline.end(),
		std::back_inserter(geometry->lines),
		[](auto &kv) { return kv.second; }
	);
	iline.clear();
	std::transform(
		xline.begin(),
		xline.end(),
		std::back_inserter(geometry->lines),
		[](auto &kv) { return kv.second; }
	);
	xline.clear();

	processed = true;
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
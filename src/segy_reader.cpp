#include <vector>
#include <string>
#include <algorithm>

#include "geolib_defines.h"
#include "csSegyHeader.h"
#include "csSegyTraceHeader.h"
#include "csFlexHeader.h"

#include "segy_reader.h"
#include "segy_bin_header.h"
#include "segy_header_map.h"
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
DLLIMPORT void * const		cseis_csSegyReader_getTrcHdrMap(void *obj);
DLLIMPORT void				cseis_csSegyReader_setTrcHdrMap(void *obj, void *map);
DLLIMPORT void				cseis_csNativeSegyReader_charBinHeader(void *obj, char *buf);
DLLIMPORT bool				cseis_csSegyReader_2moveToTrace(void *obj, int firstTraceIndex);
DLLIMPORT const void*const	cseis_csNativeSegyReader_getTraceHeader(void *obj);
DLLIMPORT const char*		cseis_csNativeSegyReader_getFileName(void *obj);

segy_reader::segy_reader(const void *obj) {
	this->obj = const_cast<void*>(obj);
	filename = string(cseis_csNativeSegyReader_getFileName(this->obj));
}

segy_reader::segy_reader(const segy_reader &obj) {
	this->obj = obj.obj;
}

segy_reader::~segy_reader() {
	close();
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
	header_map_type segyHeaderMap,
	bool reverseByteOrderData_in,
	bool reverseByteOrderHdr_in,
	bool autoscale_hdrs_in
) : seismic_data_provider(filename_in) {

	obj = cseis_csNativeSegyReader_createInstance(
		filename_in.c_str(),
		nTracesBuffer,
		geolib_type_converter::convert<header_map_type, const int>(segyHeaderMap),
		reverseByteOrderData_in,
		reverseByteOrderHdr_in,
		autoscale_hdrs_in
	);

	geometry = shared_ptr<seismic_geometry_info>(new seismic_geometry_info);
}

segy_reader::segy_reader(
	string filename_in,
	header_map_type segyHeaderMap,
	bool reverseByteOrderData_in,
	bool reverseByteOrderHdr_in,
	bool autoscale_hdrs_in
) : segy_reader(
		filename_in, 
		1, 
		segyHeaderMap, 
		reverseByteOrderData_in, 
		reverseByteOrderHdr_in, 
		autoscale_hdrs_in
	) { }

segy_reader::segy_reader(string filename_in, header_map_type segyHeaderMap) 
	: segy_reader(
		filename_in,
		1,
		segyHeaderMap,
		false,
		false,
		false
	) { }

void segy_reader::close() {
	cseis_csNativeSegyReader_closeFile(obj);
}

int segy_reader::sampleByteSize() {
	return cseis_SegyReader_sampleByteSize(obj);
}

int segy_reader::traces_count() {
	if (f_traces_count == NOT_INDEX)
		f_traces_count = cseis_SegyReader_numTraces(obj);
	return f_traces_count;
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
		throw runtime_error("Error while moving to trace " +
			to_string(firstTraceIndex));
}
void segy_reader::moveToTrace(int firstTraceIndex) {
	if (!cseis_csSegyReader_2moveToTrace(obj, firstTraceIndex))
		throw runtime_error("Error while moving to trace " +
			to_string(firstTraceIndex));
}
const float *segy_reader::nextTraceRef() {
	return cseis_csSegyReader_1getNextTrace(obj);
}

Eigen::VectorXf segy_reader::getNextTrace() {
	const float *nativeTrace = nextTraceRef();
	if (nativeTrace == nullptr)
		return {};

	samples_count();
	Eigen::VectorXf trace(f_samples_count);
	memcpy((void*)trace.data(), (void*)nativeTrace, f_samples_count * sizeof(float));
	return trace;
}

Eigen::VectorXf segy_reader::get_trace_data(int index) {
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

void segy_reader::set_header_map(shared_ptr<seismic_header_map> map) {
	shared_ptr<segy_header_map> segy_map(new segy_header_map(map));
	cseis_csSegyReader_setTrcHdrMap(obj, segy_map->obj.get());
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

vector<shared_ptr<seismic_trace>> segy_reader::get_traces(int start, int end) {
	vector<shared_ptr<seismic_trace>> res;
	get_traces(start, end, res);
	return res;
}

vector<shared_ptr<seismic_trace>> segy_reader::get_traces(seismic_line_info line_info) {
	preprocessing();
	samples_count();

	vector<shared_ptr<seismic_trace>> res;

	if (line_info.by_bounds) {
		vector<shared_ptr<seismic_trace>> buffer;
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

void segy_reader::get_traces(
	const vector<int> &trcs, 
	int trc_buffer, 
	vector<shared_ptr<seismic_trace>> &line) {

	line.resize(trcs.size());
	
	for (int i = 0; i < trcs.size(); ++i) {
		moveToTrace(trcs[i], trc_buffer);
		auto trace = nextTraceRef();
		auto header = current_trace_header();
		line[i] = shared_ptr<seismic_trace>(new segy_trace(
			dynamic_pointer_cast<segy_trace_header>(header),
			samples_count(),
			trace
			)
		);
	}
}

void segy_reader::get_traces(int start_trace, int end_trace,
	vector<shared_ptr<seismic_trace>> &line) {

	int count = end_trace - start_trace + 1;
	int buffer_size = end_trace - start_trace + 1;
	line.resize(count);
	moveToTrace(start_trace, buffer_size);

	for (int i = 0; i < count; ++i) {
		auto trace = nextTraceRef();
		auto header = current_trace_header();
		line[i] = shared_ptr<seismic_trace>(new segy_trace(
			dynamic_pointer_cast<segy_trace_header>(header),
			samples_count(),
			trace
			)
		);
	}
}

vector<shared_ptr<seismic_trace_header>> segy_reader::get_headers(seismic_line_info line_info) {
	preprocessing();
	samples_count();

	vector<shared_ptr<seismic_trace_header>> res;

	if (line_info.by_bounds) {
		vector<shared_ptr<seismic_trace_header>> buffer;
		for (int i = 0; i < line_info.bounds.size(); ++i) {
			get_headers(line_info.bounds[i].first, line_info.bounds[i].second, buffer);
			res.insert(res.end(), buffer.begin(), buffer.end());
		}
	}
	else {
		get_headers(line_info.traces_indexes, 1, res);
	}

	return res;
}

vector<shared_ptr<seismic_trace_header>> segy_reader::get_headers(int start, int end) {
	vector<shared_ptr<seismic_trace_header>> res;
	get_headers(start, end, res);
	return res;
}

void segy_reader::get_headers(
	const vector<int> &trcs,
	int trc_buffer,
	vector<shared_ptr<seismic_trace_header>> &headers) {

	headers.resize(trcs.size());
	if (!headers_in_memory) {
		for (int i = 0; i < trcs.size(); ++i) {
			moveToTrace(trcs[i], trc_buffer);
			nextTraceRef();
			auto header = current_trace_header();
			headers[i] = dynamic_pointer_cast<segy_trace_header>(header);
		}
	}
	else {
		for (int i = 0; i < trcs.size(); ++i)
			headers.push_back(this->headers[trcs[i]]);
	}
}

void segy_reader::get_headers(
	int start_trace,
	int end_trace,
	vector<shared_ptr<seismic_trace_header>> &headers) {

	int count = end_trace - start_trace + 1;
	int buffer_size = end_trace - start_trace + 1;
	headers.resize(count);
	if (!headers_in_memory){
		moveToTrace(start_trace, buffer_size);
		for (int i = 0; i < count; ++i) {
			nextTraceRef();
			auto header = current_trace_header();
			headers[i] = dynamic_pointer_cast<segy_trace_header>(header);
		}
	}
	else if (this->headers.size() > end_trace) {
		auto start_copying = this->headers.cbegin() + start_trace;
		copy(start_copying, start_copying + count, headers.begin());
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

void segy_reader::check_memory_for_headers() {
	int count = traces_count();
	int fields_count = headersCount();

	// Грубая оценка памяти, необходимой для хранения всех хэдеров
	// Запрашиваем 30% ОЗУ под хэдеры
	uint64_t need_mem = count * (
		(size_t)sizeof(csSegyTraceHeader)					// Нативный класс
		+ (size_t)(sizeof(csFlexHeader) * fields_count)		// Размер вектора в csSegyTraceHeader
		+ (size_t)(sizeof(double) * fields_count)			// Добавочный размер csFlexHeader с установленым значением
		+ (size_t)sizeof(segy_trace_header)
		+ (size_t)csSegyHeader::SIZE_TRCHDR					// Не нужно, наверное
		);
	uint64_t mem_avail = get_available_memory();
	if (0.3 * mem_avail >= need_mem)
		headers_in_memory = true;
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

	check_memory_for_headers();

	moveToTrace(0, 1);
	int i = 0;
	do {
		nextTraceRef();

		if (headers_in_memory)
			headers.push_back(current_trace_header());

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

	transform(
		iline.begin(),
		iline.end(),
		back_inserter(geometry->lines),
		[](auto &kv) { return kv.second; }
	);
	iline.clear();
	transform(
		xline.begin(),
		xline.end(),
		back_inserter(geometry->lines),
		[](auto &kv) { return kv.second; }
	);
	xline.clear();

	processed = true;
}

#ifdef PYTHON
void py_segy_reader_init(py::module &m,
	py::class_<segy_reader, shared_ptr<segy_reader>> &py_segy_reader) {

	py_segy_reader.def(py::init<string, header_map_type, bool, bool, bool>(),
		py::arg("filename"), 
		py::arg("header_map_type"), 
		py::arg("reverse_byte_order_data"),
		py::arg("reverse_byte_order_header"), 
		py::arg("autoscale_headers")
	);
	py_segy_reader.def(py::init<string, header_map_type>(),
		py::arg("filename"), 
		py::arg("header_map_type")
	);
}
#endif
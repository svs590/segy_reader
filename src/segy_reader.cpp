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
#include "data_conversion.h"
#include "segy_file.h"
#include <bitset>

#include <Eigen/Dense>

using namespace std;
using namespace cseis_geolib;


void segy_reader::init(bool reopen) {
    headers.clear();

    if (reopen) {
        if (f_istream.is_open())
            close_file();
        open_file();
    }
    
    f_text_header.clear();
    text_header();

    f_bin_header = nullptr;
    bin_header();

    geometry = shared_ptr<seismic_geometry_info>(new seismic_geometry_info);

    processed = false;
    headers_in_memory = false;

    f_filesize = bfs::file_size(f_config.filename);
    f_samples_count = f_bin_header->samples_count();

    int ethc                    = f_bin_header->extended_text_headers_count();
    int max_trc_addheaders      = f_bin_header->max_add_trc_headers_count();
    size_t text_headers_size    = segy_file::text_header_size * (1 + ethc);
    size_t bin_header_size      = segy_file::bin_header_size;
    size_t max_trc_header_size  = segy_file::trace_header_size * (1 + max_trc_addheaders);
    size_t data_format_size     = segy_data_format_size(f_bin_header->data_format());
    bool is_same                = f_bin_header->is_same_for_file();

    if (f_bin_header->is_segy_2() && f_bin_header->stream_traces_count() != 0)
        f_traces_count = f_bin_header->stream_traces_count();
    else {
        f_approx_traces_count = (int64_t)((f_filesize - text_headers_size - bin_header_size) /
            (max_trc_header_size + (size_t)f_samples_count * data_format_size));

        size_t approx_filesize = text_headers_size + bin_header_size +
            max_trc_header_size * f_approx_traces_count +
            f_samples_count * data_format_size * f_approx_traces_count;

        cout << f_filesize << '\t' << approx_filesize << endl;

        if (!is_same && f_filesize != approx_filesize)
            cout << "Warning: traces count is approximate, precise count will be "
            "be received after file preprocessing" << endl;
    }

    if (f_bin_header->is_segy_2() && f_bin_header->first_trace_offset() != 0)
        f_first_trc_offset = f_bin_header->first_trace_offset();
    else
        f_first_trc_offset = text_headers_size + bin_header_size;

    smart_buffer = smart_trc_buffer(f_header_map, f_bin_header);
    resize_buffer(100);

    f_istream.seekg(f_first_trc_offset, ios::beg);
}

segy_reader::segy_reader(const segy_reader_config &config) {
    f_config = config;
    init(true);
}

segy_reader::~segy_reader() {
    close_file();
}

void segy_reader::set_config(const segy_reader_config &config) {
    bool reopen = f_config.filename == config.filename;
    f_config = config;
    init(reopen);
}

void segy_reader::close() {
    close_file();
}

int64_t segy_reader::traces_count() {
    if (f_traces_count == NOT_INDEX)
        return f_approx_traces_count;
	return f_traces_count;
}

int segy_reader::samples_count() {
	if (f_samples_count == NOT_INDEX)
		f_samples_count = 0;
	return f_samples_count;
}

double segy_reader::sample_interval() {
    return f_bin_header->sample_interval();
}

shared_ptr<seismic_abstract_header> segy_reader::bin_header() {
	if (f_bin_header != nullptr)
		return f_bin_header;

	std::vector<byte_t> buf(segy_file::bin_header_size);

	size_t pos = f_istream.tellg();
	f_istream.seekg(segy_file::text_header_size, ios::beg);
	f_istream.read((char*)buf.data(), segy_file::bin_header_size);
	f_istream.seekg(pos, ios::beg);

	if (f_istream.fail())
		throw runtime_error("segy_reader: unexpected error occurred when reading segy binary header");

	f_bin_header.reset(new segy_bin_header(buf));

	return f_bin_header;
}

void segy_reader::move(int trc_index) {
    cout << trc_index << endl;
    if (smart_buffer.is_header_loaded(trc_index))
        return;

    int64_t relative_pos = f_first_trc_offset
        + (int64_t)trc_index * (
            (int64_t)segy_file::trace_header_size
            + (int64_t)f_samples_count * (int64_t)segy_data_format_size(f_bin_header->data_format())
        );
    f_istream.seekg(relative_pos, ios::beg);

    if (f_istream.fail())
        throw runtime_error("segy_reader: unexpected error occurred while moving to trace "
            + to_string(trc_index));
}

shared_ptr<seismic_trace> segy_reader::get_trace(int index) {
    if (smart_buffer.is_trc_loaded(index)) {
        return smart_buffer.get_trace(index);
    }

    store_traces_to_buffer(index);
    return get_trace(index);

    //auto header = shared_ptr<segy_trace_header>(
    //        new segy_trace_header(f_header_map, buffer.data(), f_bin_header->endian())
    //    );
    //char *raw_data = reinterpret_cast<char*>(&buffer[segy_file::trace_header_size]);
    //Eigen::VectorXf data(f_samples_count);
    //for (int i = 0; i < f_samples_count; ++i) {
    //    std::string binary = bitset<8>(raw_data[i]).to_string(); //to binary
    //    //std::cout << binary << "\n";
    //
    //    //bool sng = signbit((float)raw_data[i]);
    //    //if (sng)
    //    //    data[i] = static_cast<float>(-(~raw_data[i]));
    //    //else
    //        data[i] = static_cast<float>(raw_data[i]);
    //}


	//return shared_ptr<segy_trace>(
    //    new segy_trace(
    //        header,
    //        data
    //    )
	//);
}

shared_ptr<seismic_trace_header> segy_reader::trace_header(int index) {
    if (smart_buffer.is_trc_loaded(index)) {
        return smart_buffer.get_header(index);
    }
    store_traces_to_buffer(index);
    return trace_header(index);
}

shared_ptr<seismic_header_map> segy_reader::header_map() {
    return f_header_map;
}

void segy_reader::set_header_map(shared_ptr<seismic_header_map> map) {
    f_header_map = map;
    smart_buffer.reset(map, f_bin_header);
}

string segy_reader::text_header() {
	if (!f_text_header.empty())
		return f_text_header;

	char *buf = new char[csSegyHeader::SIZE_CHARHDR];

	f_istream.seekg(0);
	f_istream.read(buf, csSegyHeader::SIZE_CHARHDR);
	if (f_istream.fail()) 
		throw runtime_error("segy_reader: unexpected error occurred when reading segy text header");
	
	if (f_config.ebcdic_header)
		f_text_header = ebcdic_to_char(buf);
	else
		f_text_header = string(buf);
	
	return f_text_header;
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

    smart_buffer.set_trc_capacity(trc_buffer);
	line.resize(trcs.size());
	
	for (int i = 0; i < trcs.size(); ++i) 
        line[i] = get_trace(trcs[i]);
}

void segy_reader::get_traces(int start_trace, int end_trace,
	vector<shared_ptr<seismic_trace>> &line) {

	int count = end_trace - start_trace + 1;
	int buffer_size = end_trace - start_trace + 1;
    smart_buffer.set_trc_capacity(buffer_size);
	line.resize(count);
	move(start_trace);

	for (int i = 0; i < count; ++i) 
        line[i] = get_trace(start_trace + i);
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

    smart_buffer.set_trc_capacity(trc_buffer);

	headers.resize(trcs.size());
	if (!headers_in_memory) {
		for (int i = 0; i < trcs.size(); ++i)
			headers[i] = trace_header(trcs[i]);
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
    smart_buffer.set_trc_capacity(buffer_size);
	headers.resize(count);
	if (!headers_in_memory){
		move(start_trace);
		for (int i = 0; i < count; ++i)
            headers[i] = trace_header(start_trace + i);
	}
	else if (this->headers.size() > end_trace) {
		auto start_copying = this->headers.cbegin() + start_trace;
		copy(start_copying, start_copying + count, headers.begin());
	}
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
	int fields_count = 85; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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
	int x_index = header_map_p->contains("CDP X");
	int y_index = header_map_p->contains("CDP Y");
	if (x_index != NOT_INDEX)
		x_coord_present = true;
	if (y_index != NOT_INDEX)
		y_coord_present = true;

	int count = traces_count();

	check_memory_for_headers();
    smart_buffer.set_optimal_capacity();

	int i = 0;
	do {
        auto header = dynamic_pointer_cast<segy_trace_header>(trace_header(i));

		if (headers_in_memory)
			headers.push_back(header);
        
        VARIANT_CAST(int, il, header->iline());
        VARIANT_CAST(int, xl, header->crossline());
        if (x_coord_present)
            VARIANT_CAST(int, x, header->CDP_X());
        if (y_coord_present)
            VARIANT_CAST(int, y, header->CDP_Y());

		line_processing(iline, 
			seismic_line_info::seismic_line_type::iline, "Iline", il, i, { x, y });
		line_processing(xline,
			seismic_line_info::seismic_line_type::xline, "Xline", xl, i, { x, y });
		++i;
	} while (i < count);

    f_traces_count = i;

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

void segy_reader::open_file() {
	f_istream.open(f_config.filename, std::ios_base::in | std::ios_base::binary);
	if (f_istream.fail()) {
		throw runtime_error("segy_reader: could not open segy file");
	}
}

void segy_reader::close_file() {
	if (f_istream.is_open())
		f_istream.close();
}

void segy_reader::resize_buffer(size_t size) {
    size_t max_trc_header_size = segy_file::trace_header_size * (1 + max_trc_addheaders);
    size_t data_format_size = segy_data_format_size(f_bin_header->data_format());

    int approx_fulltrace_size = max_trc_header_size + f_samples_count * data_format_size;
    smart_buffer.set_trc_capacity(size);
}

void segy_reader::store_traces_to_buffer(int index) {
    // if buffer stores previous trace, there is no need to use move
    if (!(index - 1 == smart_buffer.end_trace()))
        move(index);

    // if buffer stores underloaded trace with index 'index'
    // read underloaded bytes and parse
    else if (smart_buffer.fail()) {
        size_t underloaded_bytes = smart_buffer.underload();
        vector<byte_t> tmp(underloaded_bytes);
        f_istream.read((char*)tmp.data(), underloaded_bytes);
        smart_buffer.parse_underloaded(tmp);
        return;
    }
    
    size_t bytes_count = smart_buffer.raw_size();
    size_t pos = (size_t)f_istream.tellg();
    if (pos + bytes_count > f_filesize)
        bytes_count = f_filesize - pos;
    memset(smart_buffer.raw(), 0, smart_buffer.raw_size());
    f_istream.read((char*)smart_buffer.raw(), bytes_count);
    smart_buffer.parse(index);
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
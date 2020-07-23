#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <bitset>

#include "segy_reader.h"
#include "segy_bin_header.h"
#include "segy_header_map.h"
#include "utils.h"
#include "data_conversion.h"
#include "segy_file.h"
#include "seismic_exception.h"

#include <Eigen/Dense>

using namespace std;


void segy_reader::init(bool reopen) {
    if (reopen) {
        f_headers.clear();

        if (f_istream.is_open())
            close_file();
        open_file();

        f_text_header = nullptr;
        text_header();

        f_bin_header = nullptr;
        bin_header();

        processed = false;
        headers_in_memory = false;

        f_filesize = bfs::file_size(f_config.filename);
        f_samples_count = f_bin_header->samples_count();

        int ethc = f_bin_header->extended_text_headers_count();
        int max_trc_addheaders = f_bin_header->max_add_trc_headers_count();
        size_t text_headers_size = segy_file::text_header_size * (1 + ethc);
        size_t bin_header_size = segy_file::bin_header_size;
        size_t max_trc_header_size = segy_file::trace_header_size * (1 + max_trc_addheaders);
        size_t data_format_size = segy_data_format_size(f_bin_header->data_format());
        bool is_same = f_bin_header->is_same_for_file();

        if (f_bin_header->is_segy_2() && f_bin_header->stream_traces_count() != 0)
            f_traces_count = f_bin_header->stream_traces_count();
        else {
            f_approx_traces_count = (int64_t) ((f_filesize - text_headers_size - bin_header_size) /
                (max_trc_header_size + (size_t) f_samples_count * data_format_size));

            size_t approx_filesize = text_headers_size + bin_header_size +
                max_trc_header_size * f_approx_traces_count +
                f_samples_count * data_format_size * f_approx_traces_count;

            if (!is_same && f_filesize != approx_filesize)
                cout << "Warning: traces count is approximate, precise count will be "
                "be received after file preprocessing" << endl;
        }

        if (f_bin_header->is_segy_2() && f_bin_header->first_trace_offset() != 0)
            f_first_trc_offset = f_bin_header->first_trace_offset();
        else
            f_first_trc_offset = text_headers_size + bin_header_size;

        f_istream.seekg(f_first_trc_offset, ios::beg);
    }

    if (f_header_map == nullptr) {
        f_headers.clear();
        f_header_map = shared_ptr<seismic_header_map>(new segy_header_map(header_map_type::STANDARD));
    }

    if (f_geometry == nullptr)
        f_geometry = shared_ptr<seismic_geometry_info>(new seismic_geometry_info);

    smart_buffer = smart_trc_buffer(f_header_map, f_bin_header, f_config.coord);
    smart_buffer.set_trc_capacity(1);
}

segy_reader::segy_reader(const segy_reader_config &config) {
    f_config = config;
    init(true);
}

segy_reader::~segy_reader() {
    close_file();
}

void segy_reader::set_config(const segy_reader_config &config) {
    bool reopen = f_config.filename != config.filename;
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
    f_istream.read((char*) buf.data(), segy_file::bin_header_size);
    f_istream.seekg(pos, ios::beg);

    if (f_istream.fail())
        throw runtime_error("segy_reader: unexpected error occurred when reading segy binary header");

    f_bin_header.reset(new segy_bin_header(buf));

    return f_bin_header;
}

void segy_reader::move(int trc_index) {
    if (smart_buffer.is_header_loaded(trc_index))
        return;

    int64_t pos = f_first_trc_offset
        + (int64_t) trc_index * (
        (int64_t) segy_file::trace_header_size
            + (int64_t) f_samples_count * (int64_t) segy_data_format_size(f_bin_header->data_format())
            );

    if (pos > f_filesize)
        throw bad_trace_index();

    f_istream.seekg(pos, ios::beg);

    if (f_istream.fail())
        throw runtime_error("segy_reader: unexpected error occurred while moving to trace "
            + to_string(trc_index));
}

shared_ptr<seismic_trace> segy_reader::trace(int index) {
    if (smart_buffer.is_trc_loaded(index))
        return smart_buffer.trace(index);

    store_traces_to_buffer(index);
    if (smart_buffer.is_trc_loaded(index))
        return smart_buffer.trace(index);
    else
        throw bad_trace_index();
}

shared_ptr<seismic_trace_header> segy_reader::header(int index) {
    if (smart_buffer.is_trc_loaded(index))
        return smart_buffer.get_header(index);

    store_traces_to_buffer(index);
    if (smart_buffer.is_trc_loaded(index))
        return smart_buffer.get_header(index);
    else
        throw bad_trace_index();
}

shared_ptr<seismic_header_map> segy_reader::header_map() {
    return f_header_map;
}

void segy_reader::set_header_map(shared_ptr<seismic_header_map> map) {
    f_header_map = map;
    smart_buffer.reset(map, f_bin_header);
}

shared_ptr<seismic_abstract_header> segy_reader::text_header() {
    if (f_text_header != nullptr)
        return f_text_header;

    vector<char> buf(segy_file::text_header_size);
    vector<byte_t> bytes(segy_file::text_header_size);

    f_istream.seekg(0);
    f_istream.read(buf.data(), segy_file::text_header_size);
    if (f_istream.fail())
        throw runtime_error("segy_reader: unexpected error occurred while reading segy text header");

    memcpy(bytes.data(), buf.data(), segy_file::text_header_size);

    f_text_header.reset(new segy_text_header(bytes, f_config.ebcdic_header));

    return f_text_header;
}

vector<shared_ptr<seismic_trace>> segy_reader::traces(int start, int end) {
    vector<shared_ptr<seismic_trace>> res;
    traces(start, end, res);
    return res;
}

vector<shared_ptr<seismic_trace>> segy_reader::traces(seismic_line_info line_info) {
    preprocessing();
    samples_count();

    vector<shared_ptr<seismic_trace>> res;

    if (line_info.f_by_bounds) {
        vector<shared_ptr<seismic_trace>> buffer;
        for (int i = 0; i < line_info.f_bounds.size(); ++i) {
            traces(line_info.f_bounds[i].first, line_info.f_bounds[i].second, buffer);
            res.insert(res.end(), buffer.begin(), buffer.end());
        }
    }
    else {
        traces(line_info.f_traces_indexes, 1, res);
    }

    return res;
}

void segy_reader::traces(
    const vector<int> &trcs,
    int trc_buffer,
    vector<shared_ptr<seismic_trace>> &line) {

    smart_buffer.set_trc_capacity(trc_buffer);
    line.resize(trcs.size());

    for (int i = 0; i < trcs.size(); ++i)
        line[i] = trace(trcs[i]);
}

void segy_reader::traces(int start_trace, int end_trace,
    vector<shared_ptr<seismic_trace>> &line) {

    int count = end_trace - start_trace + 1;
    int buffer_size = end_trace - start_trace + 1;
    smart_buffer.set_trc_capacity(buffer_size);
    line.resize(count);
    move(start_trace);

    for (int i = 0; i < count; ++i)
        line[i] = trace(start_trace + i);
}

vector<shared_ptr<seismic_trace_header>> segy_reader::headers(seismic_line_info line_info) {
    preprocessing();
    samples_count();

    vector<shared_ptr<seismic_trace_header>> res;

    if (line_info.f_by_bounds) {
        vector<shared_ptr<seismic_trace_header>> buffer;
        for (int i = 0; i < line_info.f_bounds.size(); ++i) {
            headers(line_info.f_bounds[i].first, line_info.f_bounds[i].second, buffer);
            res.insert(res.end(), buffer.begin(), buffer.end());
        }
    }
    else {
        headers(line_info.f_traces_indexes, 1, res);
    }

    return res;
}

vector<shared_ptr<seismic_trace_header>> segy_reader::headers(int start, int end) {
    vector<shared_ptr<seismic_trace_header>> res;
    headers(start, end, res);
    return res;
}

void segy_reader::headers(
    const vector<int> &trcs,
    int trc_buffer,
    vector<shared_ptr<seismic_trace_header>> &headers_buffer) {

    smart_buffer.set_trc_capacity(trc_buffer);

    headers_buffer.resize(trcs.size());
    if (!headers_in_memory) {
        for (int i = 0; i < trcs.size(); ++i)
            headers_buffer[i] = header(trcs[i]);
    }
    else {
        for (int i = 0; i < trcs.size(); ++i)
            headers_buffer.push_back(this->f_headers[trcs[i]]);
    }
}

void segy_reader::headers(
    int start_trace,
    int end_trace,
    vector<shared_ptr<seismic_trace_header>> &headers_buffer) {

    int count = end_trace - start_trace + 1;
    int buffer_size = end_trace - start_trace + 1;
    smart_buffer.set_trc_capacity(buffer_size);
    headers_buffer.resize(count);
    if (!headers_in_memory) {
        move(start_trace);
        for (int i = 0; i < count; ++i)
            headers_buffer[i] = header(start_trace + i);
    }
    else if (this->f_headers.size() > end_trace) {
        auto start_copying = this->f_headers.cbegin() + start_trace;
        copy(start_copying, start_copying + count, headers_buffer.begin());
    }
}

void segy_reader::line_processing(
    map<int, seismic_line_info> &line,
    seismic_line_info::seismic_line_type type,
    string name_prefix,
    int line_no,
    int trace_no,
    pair<float, float> point) {

    auto &this_line = line[line_no];

    if (this_line.f_start_trace == NOT_INDEX) {
        this_line.f_name = name_prefix + " " + to_string(line_no);
        this_line.f_start_trace = trace_no;
        this_line.f_end_trace = trace_no;
        this_line.f_traces_indexes.push_back(trace_no);
        this_line.f_start_point = point;
        this_line.f_type = type;
    }
    else {
        if (this_line.f_end_trace + 1 == trace_no) {
            this_line.f_sequential_traces_count++;
            if (this_line.f_sequential_traces_count > 3 && !this_line.f_by_bounds) {
                this_line.f_by_bounds = true;
                this_line.f_bounds.push_back({ this_line.f_start_trace, trace_no });
                this_line.f_traces_indexes.clear();
                this_line.f_traces_indexes.shrink_to_fit();
            }
            else if (this_line.f_by_bounds) {
                auto last_bound = --this_line.f_bounds.end();
                last_bound->second = trace_no;
            }
            else
                this_line.f_traces_indexes.push_back(trace_no);
        }
        else {
            if (this_line.f_by_bounds)
                this_line.f_bounds.push_back({ trace_no, trace_no });
            else
                this_line.f_traces_indexes.push_back(trace_no);
        }
        this_line.f_end_trace = trace_no;
        this_line.f_end_point = point;
    }
    this_line.f_traces_count++;
}

void segy_reader::check_memory_for_headers() {
    int count = traces_count();
    int fields_count = f_header_map->count();

    // Грубая оценка памяти, необходимой для хранения всех хэдеров
    // Запрашиваем 30% ОЗУ под хэдеры
    uint64_t need_mem = count * (
        (size_t)sizeof(segy_trace_header)
        + (size_t) (sizeof(double) * fields_count)
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

    size_t count = (size_t) traces_count();

    check_memory_for_headers();
    smart_buffer.set_optimal_capacity(count);

    shared_ptr<segy_trace_header> header;
    int i = 0;
    bool flag = true;

    do {
        try {
            header = dynamic_pointer_cast<segy_trace_header>(this->header(i));
        }
        catch (bad_trace_index& e) {
            break;
        }
        catch (end_of_file& e) {
            break;
        }

        if (headers_in_memory)
            f_headers.push_back(header);

        VARIANT_VALUE_CAST(il, header->iline());
        VARIANT_VALUE_CAST(xl, header->crossline());
        if (x_coord_present)
            VARIANT_VALUE_CAST(x, header->CDP_X());
        if (y_coord_present)
            VARIANT_VALUE_CAST(y, header->CDP_Y());

        line_processing(iline,
            seismic_line_info::seismic_line_type::iline, "Iline", il, i, { x, y });
        line_processing(xline,
            seismic_line_info::seismic_line_type::xline, "Xline", xl, i, { x, y });
        ++i;
    } while (flag);

    f_traces_count = i;

    // Delete lines with one trace (probably there is 2D seismic)
    for (auto it = iline.begin(); it != iline.end();)
        if (it->second.f_traces_count == 1)
            it = iline.erase(it);
        else
            it++;
    for (auto it = xline.begin(); it != xline.end();)
        if (it->second.f_traces_count == 1)
            it = xline.erase(it);
        else
            it++;

    if (iline.size() == 0 || xline.size() == 0)
        f_geometry->f_gtype = seismic_geometry_info::seismic_geometry_type::g2D;
    else
        f_geometry->f_gtype = seismic_geometry_info::seismic_geometry_type::g3D;

    f_geometry->f_ilines.clear();
    f_geometry->f_crosslines.clear();

    transform(
        iline.begin(),
        iline.end(),
        back_inserter(f_geometry->f_ilines),
        [](auto &kv) { return kv.second; }
    );
    iline.clear();
    transform(
        xline.begin(),
        xline.end(),
        back_inserter(f_geometry->f_crosslines),
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

        size_t pos = (size_t) f_istream.tellg();
        if (pos + underloaded_bytes > f_filesize)
            underloaded_bytes = f_filesize - pos;
        if (underloaded_bytes == 0)
            throw end_of_file();

        f_istream.read((char*) tmp.data(), underloaded_bytes);
        smart_buffer.parse_underloaded(tmp);
        return;
    }

    size_t bytes_count = smart_buffer.raw_size();
    size_t pos = (size_t) f_istream.tellg();
    if (pos + bytes_count > f_filesize)
        bytes_count = f_filesize - pos;
    if (bytes_count == 0)
        throw end_of_file();

    memset(smart_buffer.raw(), 0, smart_buffer.raw_size());
    f_istream.read((char*) smart_buffer.raw(), bytes_count);
    smart_buffer.parse(index);
}

#ifdef PYTHON
void py_segy_reader_init(py::module &m,
    py::class_<segy_reader, shared_ptr<segy_reader>> &py_segy_reader) {

    py::class_<segy_reader_config> reader_config(m, "reader_config");
    reader_config.def(py::init<>());
    reader_config.def_property("filename",
        [](segy_reader_config &c) { return c.filename; },
        [](segy_reader_config &c, std::wstring &filename) { c.filename = filename; }
    );
    reader_config.def_property("coordinates",
        [](segy_reader_config &c) { return c.coord; },
        [](segy_reader_config &c, segy_coord coord) { c.coord = coord; }
    );
    //reader_config.def_property("ebdict_header", &segy_reader_config::ebcdic_header);

    py_segy_reader.def(py::init<segy_reader_config>(),
        py::arg("config")
    );
    py_segy_reader.def("config", &segy_reader::config);
    py_segy_reader.def("set_config", &segy_reader::set_config,
        py::arg("config")
    );
}
#endif
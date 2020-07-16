#include "segy_smart_trc_buffer.h"
#include "utils.h"

using namespace std;


smart_trc_buffer::smart_trc_buffer(
    shared_ptr<seismic_header_map> header_map, 
    shared_ptr<segy_bin_header> bin_header,
    segy_coord coord
) {
    f_coord = coord;
    reset(header_map, bin_header);
    set_trc_capacity(1);
}

void smart_trc_buffer::reset(shared_ptr<seismic_header_map> header_map, shared_ptr<segy_bin_header> bin_header) {
    f_header_map = header_map;
    f_bin_header = bin_header;
    set_trc_capacity(f_capacity);
}

void smart_trc_buffer::set_trc_capacity(size_t cap) {
    f_capacity = cap;
    f_raw_buffer.resize(cap * (
        segy_file::trace_header_size + f_bin_header->samples_count() * segy_data_format_size(f_bin_header->data_format()))
    );
    f_headers_buffer.resize(cap);
    f_size = (f_size > 0) ? (f_size <= cap) ? f_size : cap : 0;
}

void smart_trc_buffer::set_optimal_capacity(size_t max_cap) {
    size_t new_cap = 0.3 * get_available_memory() / 2 
        / (segy_file::trace_header_size + f_bin_header->samples_count() * segy_data_format_size(f_bin_header->data_format()));
    set_trc_capacity(new_cap < max_cap ? new_cap : max_cap);
}

void smart_trc_buffer::load(const vector<byte_t> &raw_buffer, size_t absolute_index_start_trc) {
    if (raw_buffer.size() > f_raw_buffer.size())
        throw invalid_argument("smart_trc_buffer: load: readed buffer is greater than required");

    copy(raw_buffer.cbegin(), raw_buffer.cend(), f_raw_buffer.begin());

    parse(absolute_index_start_trc);
}

void smart_trc_buffer::parse_underloaded(const vector<byte_t> &raw_underload) {
    if (!fail())
        return;
    f_fail = false;
    f_underload = 0;

    size_t old_size = f_raw_buffer.size();
    f_raw_buffer.resize(old_size + raw_underload.size());
    copy(raw_underload.cbegin(), raw_underload.cend(), f_raw_buffer.begin() + old_size);

    size_t last_trc_offset = f_trc_offsets.back();
    size_t last_trc_samples;
    VARIANT_VALUE_CAST(last_trc_samples, f_headers_buffer.back()->samples_count());
    size_t last_trc_size = last_trc_samples * segy_data_format_size(f_bin_header->data_format());

    _parse(last_trc_offset + last_trc_size);
}

void smart_trc_buffer::parse(size_t absolute_index_start_trc) {
    f_fail = false;
    f_underload = 0;
    f_headers_buffer.clear();
    f_trc_offsets.clear();
    f_size = 0;
    f_absolute_trc_beg = absolute_index_start_trc;

    _parse(0);
}

void smart_trc_buffer::_parse(size_t start_offset) {
    size_t samples_count = 0;
    size_t offset = start_offset;

    while (offset < f_raw_buffer.size()) {
        f_trc_offsets.push_back(segy_file::trace_header_size + offset);

        byte_t *raw_header = &f_raw_buffer[offset];
        auto header = shared_ptr<segy_trace_header>(
            new segy_trace_header(f_header_map, raw_header, f_bin_header->endian(), f_coord)
            );

        if (!header->is_valid())
            break;

        f_headers_buffer.push_back(header);

        VARIANT_VALUE_CAST(samples_count, header->samples_count());

        offset += segy_file::trace_header_size
            + samples_count * segy_data_format_size(f_bin_header->data_format());

        f_size++;
    }

    // Check if the last trace completely stored in buffer. If not, delete this trace and
    // set fail/underload
    size_t last_offset = f_trc_offsets.back();
    size_t trc_size = samples_count * segy_data_format_size(f_bin_header->data_format());
    if (last_offset + trc_size > f_raw_buffer.size()) {
        f_headers_buffer.erase(--f_headers_buffer.end());
        f_trc_offsets.erase(--f_trc_offsets.end()--);
        f_size--;
        f_fail = true;
        f_underload = last_offset + trc_size - f_raw_buffer.size();
    }
}

size_t smart_trc_buffer::trc_capacity() {
    return f_capacity;
}

size_t smart_trc_buffer::loaded_trc_count() {
    return f_size;
}

size_t smart_trc_buffer::raw_size() { 
    return f_raw_buffer.size(); 
}

byte_t *smart_trc_buffer::raw() {
    return f_raw_buffer.data(); 
}

bool smart_trc_buffer::is_trc_loaded(size_t absolute_index) {
    return (absolute_index >= f_absolute_trc_beg
        && absolute_index < f_absolute_trc_beg + f_size);
}

bool smart_trc_buffer::is_header_loaded(size_t absolute_index) {
    return is_trc_loaded(absolute_index);
}

shared_ptr<segy_trace_header> smart_trc_buffer::get_header(size_t absolute_index) {
    if (is_header_loaded(absolute_index))
        return f_headers_buffer[absolute_index - f_absolute_trc_beg];
    else
        throw invalid_argument("smart_trc_buffer: get_header: header is not stored in buffer");
}

shared_ptr<segy_trace> smart_trc_buffer::trace(size_t absolute_index) {
    if (is_trc_loaded(absolute_index)) {
        size_t offset = f_trc_offsets[absolute_index - f_absolute_trc_beg];
        size_t samples_count;
        VARIANT_VALUE_CAST(
            samples_count,
            f_headers_buffer[absolute_index - f_absolute_trc_beg]->samples_count()
        );

        byte_t *this_trc_data = &f_raw_buffer[offset];
        auto res = shared_ptr<segy_trace>(
            new segy_trace(
                f_headers_buffer[absolute_index - f_absolute_trc_beg],
                this_trc_data,
                get<short>(f_headers_buffer[absolute_index - f_absolute_trc_beg]->samples_count()),
                f_bin_header->data_format(),
                f_bin_header->endian()
            )
        );
        return res;
    }
    else
        throw invalid_argument("smart_trc_buffer: trace: header is not stored in buffer");
}

#pragma once

#include <memory>
#include <vector>

#include "seismic_header_map.h"
#include "segy_bin_header.h"
#include "segy_trace_header.h"
#include "segy_trace.h"
#include "data_conversion.h"

/*
    Service class for segy_reader. Stores byte buffer loaded from segy file with
    parsed trace headers and returns trace from buffer by absolute index
*/
class smart_trc_buffer {
    std::shared_ptr<seismic_header_map>             f_header_map;
    std::shared_ptr<segy_bin_header>                f_bin_header;

    std::vector<std::shared_ptr<segy_trace_header>> f_headers_buffer;

    std::vector<byte_t>                             f_raw_buffer;
    std::vector<size_t>                             f_trc_offsets;
    size_t                                          f_capacity          = 1;
    size_t                                          f_absolute_trc_beg;
    size_t                                          f_size              = 0;

public:
    smart_trc_buffer() {}
    smart_trc_buffer(std::shared_ptr<seismic_header_map> header_map, std::shared_ptr<segy_bin_header> bin_header);

    void                                            reset(std::shared_ptr<seismic_header_map> header_map, std::shared_ptr<segy_bin_header> bin_header);

    void                                            load(const std::vector<byte_t> &raw_buffer, size_t absolute_index_start_trc);
    void                                            parse(size_t absolute_index_start_trc);

    void                                            set_trc_capacity(size_t cap);
    size_t                                          trc_capacity();
    size_t                                          loaded_trc_count();
    size_t                                          raw_size();
    bool                                            is_trc_loaded(size_t absolute_index);
    bool                                            is_header_loaded(size_t absolute_index);
    std::shared_ptr<segy_trace_header>              get_header(size_t absolute_index);
    std::shared_ptr<segy_trace>                     get_trace(size_t absolute_index);

    void                                            set_optimal_capacity();

    byte_t                                          *raw();
};
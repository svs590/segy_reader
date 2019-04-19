#include "segy_trace.h"
#include "data_conversion.h"

using namespace std;


#define DATA_TO_NATIVE_CASE_LINE(data_type, samples_count, order)   \
f_data = segy_data_to_native<data_type>(                            \
    data,                                                           \
    segy_data_format_size(data_type) * num_samples,                 \
    order                                                           \
);

segy_trace::segy_trace(const segy_trace &trace) {
    f_header = make_shared<segy_trace_header>(*dynamic_pointer_cast<segy_trace_header>(trace.f_header));
    f_data = trace.f_data;
}

segy_trace::segy_trace(
    const segy_trace_header &header, 
    const byte_t *data, 
    int num_samples, 
    segy_data_format format,
    endian_order order) {

    f_header = shared_ptr<seismic_trace_header>(new segy_trace_header(header));
    parse(data, num_samples, format, order);
}

segy_trace::segy_trace(
    const segy_trace_header &header, 
    const seismic_variant_vector &data) {

    f_header = shared_ptr<seismic_trace_header>(new segy_trace_header(header));
    f_data = data;
}

segy_trace::segy_trace(
    shared_ptr<segy_trace_header> header, 
    const seismic_variant_vector &data) {

    f_header = make_shared<segy_trace_header>(*header);
    f_data = data;
}

segy_trace::segy_trace(
    shared_ptr<segy_trace_header> header, 
    const byte_t *data, 
    int num_samples, 
    segy_data_format format,
    endian_order order) {

    f_header = make_shared<segy_trace_header>(*header);
    parse(data, num_samples, format, order);
}

segy_trace::~segy_trace() { }

shared_ptr<seismic_trace_header> segy_trace::header() {
	return f_header;
}

void segy_trace::set_header(shared_ptr<seismic_trace_header> header) {
    f_header = header;
}

seismic_variant_vector &segy_trace::data() {
	return f_data;
}

void segy_trace::set_data(const seismic_variant_vector &data) {
	f_data = data;
}

void segy_trace::parse(const byte_t *data, int num_samples, segy_data_format format, endian_order order) {
    switch (format) {
    case segy_data_format::float32_ibm:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::float32_ibm, num_samples, order);
        break;
    case segy_data_format::int32_2complement:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::int32_2complement, num_samples, order);
        break;
    case segy_data_format::int16_2complement:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::int16_2complement, num_samples, order);
        break;
    case segy_data_format::float32_obsolete:
        throw exception("segy_trace: parse: float32 obsolete is not supported");
        break;
    case segy_data_format::float32:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::float32, num_samples, order);
        break;
    case segy_data_format::float64:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::float64, num_samples, order);
        break;
    case segy_data_format::int24_2complement:
        throw exception("segy_trace: parse: int24 two's complement is not supported");
        break;
    case segy_data_format::int8_2complement:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::int8_2complement, num_samples, order);
        break;
    case segy_data_format::int64_2complement:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::int64_2complement, num_samples, order);
        break;
    case segy_data_format::uint32:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::uint32, num_samples, order);
        break;
    case segy_data_format::uint16:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::uint16, num_samples, order);
        break;
    case segy_data_format::uint64:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::uint64, num_samples, order);
        break;
    case segy_data_format::uint24:
        throw exception("segy_trace: parse: uint24 is not supported");
        break;
    case segy_data_format::uint8:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::uint8, num_samples, order);
        break;
    default:
        throw invalid_argument("segy_trace: parse: invalid data type");
        break;
    }
}
#include <exception>

#ifdef PYTHON
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
#endif

#include "segy_trace.h"
#include "data_conversion.h"


using namespace std;


#define DATA_TO_NATIVE_CASE_LINE(data_type, samples_count, order)   \
f_data = segy_data_to_native<data_type>(                            \
    data,                                                           \
    segy_data_format_size(data_type) * num_samples,                 \
    order                                                           \
);

#define NATIVE_TO_DATA_CASE_LINE(data_type, order)                  \
data = native_to_segy_data<data_type>(                              \
    f_data,                                                         \
    order                                                           \
);

segy_trace::segy_trace(const segy_trace &trace) {
    f_header = make_shared<segy_trace_header>(*dynamic_pointer_cast<segy_trace_header>(trace.f_header));
    f_order = f_header->endian();
    f_data = trace.f_data;
}

segy_trace::segy_trace(
    const segy_trace_header &header, 
    const byte_t *data, 
    int num_samples, 
    segy_data_format format,
    endian_order order) {

    f_header = shared_ptr<seismic_trace_header>(new segy_trace_header(header));
    f_order = order;
    parse(data, num_samples, format, order);
}

segy_trace::segy_trace(
    const segy_trace_header &header, 
    const seismic_variant_vector &data) {

    f_header = shared_ptr<seismic_trace_header>(new segy_trace_header(header));
    f_order = f_header->endian();
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
    f_order = order;
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

seismic_variant_value segy_trace::X() {
    return f_header->X();
}

seismic_variant_value segy_trace::Y() {
    return f_header->Y();
}

seismic_variant_value segy_trace::iline() {
    return f_header->iline();
}

seismic_variant_value segy_trace::crossline() {
    return f_header->crossline();
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
        SR_THROW(runtime_error, "float32 obsolete is not supported");
        break;
    case segy_data_format::float32:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::float32, num_samples, order);
        break;
    case segy_data_format::float64:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::float64, num_samples, order);
        break;
    case segy_data_format::int24_2complement:
        SR_THROW(runtime_error, "int24 two's complement is not supported");
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
        throw runtime_error("segy_trace: parse: uint24 is not supported");
        break;
    case segy_data_format::uint8:
        DATA_TO_NATIVE_CASE_LINE(segy_data_format::uint8, num_samples, order);
        break;
    default:
        SR_THROW(invalid_argument, "invalid data type");
        break;
    }
}

vector<byte_t> segy_trace::raw_data(segy_data_format format, endian_order order) {
    vector<byte_t> header;
    vector<byte_t> data;
    
    f_header->set_endian(order);

    header = f_header->raw_data();

    switch (format) {
    case segy_data_format::float32_ibm:
        NATIVE_TO_DATA_CASE_LINE(segy_data_format::float32_ibm, order);
        break;
    case segy_data_format::int32_2complement:
        NATIVE_TO_DATA_CASE_LINE(segy_data_format::int32_2complement, order);
        break;
    case segy_data_format::int16_2complement:
        NATIVE_TO_DATA_CASE_LINE(segy_data_format::int16_2complement, order);
        break;
    case segy_data_format::float32_obsolete:
        SR_THROW(runtime_error, "float32 obsolete is not supported");
        break;
    case segy_data_format::float32:
        NATIVE_TO_DATA_CASE_LINE(segy_data_format::float32, order);
        break;
    case segy_data_format::float64:
        NATIVE_TO_DATA_CASE_LINE(segy_data_format::float64, order);
        break;
    case segy_data_format::int24_2complement:
        SR_THROW(runtime_error, "int24 two's complement is not supported");
        break;
    case segy_data_format::int8_2complement:
        NATIVE_TO_DATA_CASE_LINE(segy_data_format::int8_2complement, order);
        break;
    case segy_data_format::int64_2complement:
        NATIVE_TO_DATA_CASE_LINE(segy_data_format::int64_2complement, order);
        break;
    case segy_data_format::uint32:
        NATIVE_TO_DATA_CASE_LINE(segy_data_format::uint32, order);
        break;
    case segy_data_format::uint16:
        NATIVE_TO_DATA_CASE_LINE(segy_data_format::uint16, order);
        break;
    case segy_data_format::uint64:
        NATIVE_TO_DATA_CASE_LINE(segy_data_format::uint64, order);
        break;
    case segy_data_format::uint24:
        throw runtime_error("segy_trace: parse: uint24 is not supported");
        break;
    case segy_data_format::uint8:
        NATIVE_TO_DATA_CASE_LINE(segy_data_format::uint8, order);
        break;
    default:
        SR_THROW(invalid_argument, "invalid data type");
        break;
    }

    vector<byte_t> all(header.size() + data.size());
    copy(header.begin(), header.end(), all.begin());
    copy(data.begin(), data.end(), all.begin() + header.size());

    return all;
}

#ifdef PYTHON
void py_segy_trace_init(
    py::module &m,
    py::class_<segy_trace, shared_ptr<segy_trace>> &py_segy_trace
) {
    py_segy_trace.def(py::init<const segy_trace &>());
    py_segy_trace.def(
        py::init<
            const segy_trace_header &,
            const seismic_variant_vector &
        >()
    );
}
#endif
#ifndef __DATA_TYPES
#define __DATA_TYPES

#include <variant>
#include <string>

#ifdef PYTHON
#include <pybind11/stl.h>

namespace py = pybind11;
#endif

typedef unsigned char byte_t;

using seismic_variant_value =
std::variant<
    short,
    unsigned short,
    int,
    int64_t,
    uint64_t,
    float,
    double,
    char,
    std::string
>;

#define VARIANT_CAST(type_out, value_out, variant_value)                        \
{                                                                               \
    if (holds_alternative<int>(variant_value))                                        \
        value_out = static_cast<type_out>(get<int>(variant_value));             \
    else if (holds_alternative<int64_t>(variant_value))                         \
        value_out = static_cast<type_out>(get<int64_t>(variant_value));         \
    else if (holds_alternative<uint64_t>(variant_value))                        \
        value_out = static_cast<type_out>(get<uint64_t>(variant_value));        \
    else if (holds_alternative<short>(variant_value))                           \
        value_out = static_cast<type_out>(get<short>(variant_value));           \
    else if (holds_alternative<unsigned short>(variant_value))                  \
        value_out = static_cast<type_out>(get<unsigned short>(variant_value));  \
    else if (holds_alternative<float>(variant_value))                           \
        value_out = static_cast<type_out>(get<float>(variant_value));           \
    else if (holds_alternative<double>(variant_value))                          \
        value_out = static_cast<type_out>(get<double>(variant_value));          \
    else if (holds_alternative<char>(variant_value))                            \
        value_out = static_cast<type_out>(get<char>(variant_value));            \
}

enum class seismic_data_type {
	UNKNOWN,
	EMPTY,
	INT,
	FLOAT,
	DOUBLE,
	CHAR,
	STRING,
	INT64,
    UINT64,
	SHORT,
	USHORT,

    VALUES_COUNT
};

enum class object_type {
	UNKNOWN,
	SEGY_READER,
	SEGY_HEADERMAP,
	SEGY_TRACEHEADER_FIELD,
	SEGY_TRACE,
	SEGY_TRACEHEADER,
	SEGY_BIN_HEADER,

    VALUES_COUNT
};

#ifdef PYTHON
void py_seismic_data_types_init(py::module &m);
#endif

#endif

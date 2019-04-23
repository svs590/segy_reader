#pragma once

#include <variant>
#include <string>
#include <cstdint>

#include <Eigen/Dense>

#ifdef PYTHON
#include <pybind11/stl.h>

namespace py = pybind11;
#endif

typedef unsigned char byte_t;

using seismic_variant_value =
std::variant<
    char,
    short,
    unsigned short,
    int,
    int64_t,
    uint64_t,
    float,
    double,
    std::string
>;

using seismic_variant_vector = std::variant<
    Eigen::Matrix<short, -1, 1>,
    Eigen::Matrix<unsigned short, -1, 1>,
    Eigen::Matrix<int, -1, 1>,
    Eigen::Matrix<uint32_t, -1, 1>,
    Eigen::Matrix<int64_t, -1, 1>,
    Eigen::Matrix<uint64_t, -1, 1>,
    Eigen::Matrix<float, -1, 1>,
    Eigen::Matrix<double, -1, 1>
>;

#define VARIANT_VALUE_CAST(type_out, value_out, variant_value)                      \
{                                                                                   \
    if (holds_alternative<type_out>(variant_value))                                 \
        value_out = std::get<type_out>(variant_value);                              \
    else if (holds_alternative<int>(variant_value))                                 \
        value_out = static_cast<type_out>(std::get<int>(variant_value));            \
    else if (holds_alternative<int64_t>(variant_value))                             \
        value_out = static_cast<type_out>(std::get<int64_t>(variant_value));        \
    else if (holds_alternative<uint64_t>(variant_value))                            \
        value_out = static_cast<type_out>(std::get<uint64_t>(variant_value));       \
    else if (holds_alternative<short>(variant_value))                               \
        value_out = static_cast<type_out>(std::get<short>(variant_value));          \
    else if (holds_alternative<unsigned short>(variant_value))                      \
        value_out = static_cast<type_out>(std::get<unsigned short>(variant_value)); \
    else if (holds_alternative<float>(variant_value))                               \
        value_out = static_cast<type_out>(std::get<float>(variant_value));          \
    else if (holds_alternative<double>(variant_value))                              \
        value_out = static_cast<type_out>(std::get<double>(variant_value));         \
    else if (holds_alternative<char>(variant_value))                                \
        value_out = static_cast<type_out>(std::get<char>(variant_value));           \
}


#define VARIANT_VALUE_OPERATION_CASE_LINE(val_1, val_2, res, type, op)              \
{                                                                                   \
    type a, b;                                                                      \
    VARIANT_VALUE_CAST(type, a, val_1)                                              \
    VARIANT_VALUE_CAST(type, b, val_2)                                              \
    res = static_cast<type>(a op b);                                                \
}

#define VARIANT_VALUE_OPERATION(val_1, val_2, res, op) {                            \
    int t1 = val_1.index();                                                         \
    int t2 = val_2.index();                                                         \
    int t;                                                                          \
    if (t1 >= t2)                                                                   \
        t = t1;                                                                     \
    else                                                                            \
        t = t2;                                                                     \
    switch (t) {                                                                    \
    case 0:                                                                         \
        VARIANT_VALUE_OPERATION_CASE_LINE(val_1, val_2, res, char, op)              \
        break;                                                                      \
    case 1:                                                                         \
        VARIANT_VALUE_OPERATION_CASE_LINE(val_1, val_2, res, short, op)             \
        break;                                                                      \
    case 2:                                                                         \
        VARIANT_VALUE_OPERATION_CASE_LINE(val_1, val_2, res, unsigned short, op)    \
        break;                                                                      \
    case 3:                                                                         \
        VARIANT_VALUE_OPERATION_CASE_LINE(val_1, val_2, res, int, op)               \
        break;                                                                      \
    case 4:                                                                         \
        VARIANT_VALUE_OPERATION_CASE_LINE(val_1, val_2, res, int64_t, op)           \
        break;                                                                      \
    case 5:                                                                         \
        VARIANT_VALUE_OPERATION_CASE_LINE(val_1, val_2, res, uint64_t, op)          \
        break;                                                                      \
    case 6:                                                                         \
        VARIANT_VALUE_OPERATION_CASE_LINE(val_1, val_2, res, float, op)             \
        break;                                                                      \
    case 7:                                                                         \
        VARIANT_VALUE_OPERATION_CASE_LINE(val_1, val_2, res, double, op)            \
        break;                                                                      \
    default:                                                                        \
        break;                                                                      \
    }                                                                               \
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

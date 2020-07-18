#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include <numeric>

#include <Eigen/Dense>

#ifdef PYTHON
#include <pybind11/stl.h>

namespace py = pybind11;
#endif

typedef unsigned char byte_t;

using seismic_variant_value =
std::variant<
    char,
    byte_t,
    short,
    unsigned short,
    int,
    unsigned int,
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

template <typename From, typename To>
struct numeric_cast {
    static inline void cast(To &value, const From &from) {
        value = static_cast<To>(from);
    }
};

template <typename From, typename To>
struct no_numeric_cast {
    static inline void cast(To &value, const From &from) {
        throw std::invalid_argument("You mixed numeric type with string type.");
    }
};

template <>
struct no_numeric_cast<std::string, std::string> {
    static inline void cast(std::string &value, const std::string &from) {
        value = from;
    }
};

template <typename From, typename To>
struct caster_selector {
    using type = std::conditional_t<
        std::is_arithmetic<From>::value && std::is_arithmetic<To>::value,
        numeric_cast<From, To>,
        no_numeric_cast<From, To>
    >;
};

#define VARIANT_VALUE_CAST(value_out, variant_value)                                \
{                                                                                   \
    using type_out = std::decay_t<decltype(value_out)>;                             \
    std::visit([&value_out](auto&& arg) {                                           \
        using T = std::decay_t<decltype(arg)>;                                      \
        caster_selector<T, type_out>::type::cast(value_out, arg);                   \
    }, variant_value);                                                              \
}

namespace seismic_variant_operations {
    seismic_variant_value operator+(const seismic_variant_value& left, const seismic_variant_value& right);
    seismic_variant_value operator*(const seismic_variant_value& left, const seismic_variant_value& right);

    template <typename T>
    bool operator==(const seismic_variant_value &left, T right);
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

#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include <numeric>

#include <Eigen/Dense>

#include <boost/preprocessor.hpp>
#include <boost/preprocessor/facilities/is_empty_variadic.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#include "seismic_exception.h"
#include "segy_file.h"
#include "common.h"

#ifdef PYTHON
#include <pybind11/stl.h>

namespace py = pybind11;
#endif


typedef unsigned char byte_t;

using seismic_variant_value =
std::variant<
    byte_t,
    short,
    unsigned short,
    int,
    unsigned int,
    int64_t,
    uint64_t,
    float,
    double,
    std::string,
    char            ///< char after std::string for properly string cast
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

template <typename From>
struct enum_cast {
    using To = std::conditional_t<std::is_enum<From>::value, int, From>;
    static inline To cast(const From &from) {
        return static_cast<To>(from);
    }
};

template <typename From, typename To>
struct numeric_cast {
    static inline void cast(To &value, const From &from) {
        value = static_cast<To>(from);
    }
};

template <typename From, typename To>
struct no_numeric_cast {
    static inline void cast(To &value, const From &from) {
        SR_THROW(
            std::invalid_argument,
            "Mixing inappropriate types (" + std::string(typeid(From).name())
            + " with " + std::string(typeid(To).name()) + ")"
        );
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
        (std::is_arithmetic<From>::value && std::is_arithmetic<To>::value)
        || (std::is_integral<From>::value && std::is_enum<To>::value)
        || (std::is_enum<From>::value && std::is_integral<To>::value),
        numeric_cast<From, To>,
        no_numeric_cast<From, To>
    >;
};

bool is_integral_type(const seismic_variant_value &val);

segy_data_format segy_format_from_data(const seismic_variant_vector &val);

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

    int                   size(const seismic_variant_vector& vec);

    template <typename T>
    bool operator==(const seismic_variant_value &left, T right);
}

#define SEISMIC_DATA_TYPE                                                       \
    ((UNKNOWN,      nullptr_t))                                                 \
    ((EMPTY,        nullptr_t))                                                 \
    ((INT,          int))                                                       \
    ((FLOAT,        float))                                                     \
    ((DOUBLE,       double))                                                    \
    ((CHAR,         char))                                                      \
    ((STRING,       ))                                                          \
    ((INT64,        int64_t))                                                   \
    ((UINT64,       uint64_t))                                                  \
    ((SHORT,        int16_t))                                                   \
    ((USHORT,       uint16_t))

#define DT_FILL_SEISMIC_DATA_TYPE_ENUM_OP(z, data, el)                          \
    BOOST_PP_TUPLE_ELEM(2, 0, el),

#define DT_FILL_SEISMIC_DATA_TYPE(seq)                                          \
    BOOST_PP_SEQ_FOR_EACH(DT_FILL_SEISMIC_DATA_TYPE_ENUM_OP, ~, seq)

enum class seismic_data_type {
    DT_FILL_SEISMIC_DATA_TYPE(SEISMIC_DATA_TYPE)

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

#ifndef __DATA_TYPES
#define __DATA_TYPES

#include <variant>

#ifdef PYTHON
#include <pybind11/stl.h>

namespace py = pybind11;
#endif

typedef unsigned char byte_t;

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
};

enum class object_type {
	UNKNOWN,
	SEGY_READER,
	SEGY_HEADERMAP,
	SEGY_TRACEHEADER_FIELD,
	SEGY_TRACE,
	SEGY_TRACEHEADER,
	SEGY_BIN_HEADER
};

struct seismic_variant_value {
    std::variant<int, uint64_t, double> val;
    seismic_data_type type;
};


#ifdef PYTHON
void py_seismic_data_types_init(py::module &m);
#endif

#endif

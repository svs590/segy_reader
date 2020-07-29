#include "segy_file.h"

short segy_data_format_size(segy_data_format format) {
    switch (format)
    {
    case segy_data_format::float32_ibm:
        return 4;
        break;
    case segy_data_format::int32_2complement:
        return 4;
        break;
    case segy_data_format::int16_2complement:
        return 2;
        break;
    case segy_data_format::float32_obsolete:
        return 4;
        break;
    case segy_data_format::float32:
        return 4;
        break;
    case segy_data_format::float64:
        return 8;
        break;
    case segy_data_format::int24_2complement:
        return 3;
        break;
    case segy_data_format::int8_2complement:
        return 1;
        break;
    case segy_data_format::int64_2complement:
        return 8;
        break;
    case segy_data_format::uint32:
        return 4;
        break;
    case segy_data_format::uint16:
        return 2;
        break;
    case segy_data_format::uint64:
        return 8;
        break;
    case segy_data_format::uint24:
        return 3;
        break;
    case segy_data_format::uint8:
        return 1;
        break;
    default:
        return 0;
        break;
    }
}

#ifdef PYTHON
void py_segy_file_init(py::module &m) {
    py::enum_<segy_data_format>(m, "segy_data_format")
        .value("float32",           segy_data_format::float32)
        .value("float32_ibm",       segy_data_format::float32_ibm)
        .value("float32_obsolete",  segy_data_format::float32_obsolete)
        .value("float64",           segy_data_format::float64)
        .value("int16_2complement", segy_data_format::int16_2complement)
        .value("int24_2complement", segy_data_format::int24_2complement)
        .value("int32_2complement", segy_data_format::int32_2complement)
        .value("int64_2complement", segy_data_format::int64_2complement)
        .value("int8_2complement",  segy_data_format::int8_2complement)
        .value("uint16",            segy_data_format::uint16)
        .value("uint24",            segy_data_format::uint24)
        .value("uint32",            segy_data_format::uint32)
        .value("uint64",            segy_data_format::uint64)
        .value("uint8",             segy_data_format::uint8)
        .value("automatic",         segy_data_format::automatic)
        .export_values();

    py::enum_<segy_sorting>(m, "segy_sorting")
        .value("as_is",             segy_sorting::as_is)
        .value("crossline",         segy_sorting::crossline)
        .value("iline",             segy_sorting::iline)
        .export_values();
}
#endif
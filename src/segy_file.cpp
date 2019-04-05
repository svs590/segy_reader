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
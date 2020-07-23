#include "segy_bin_header.h"
#include "segy_file.h"
#include "data_conversion.h"
#include "utils.h"
#include "seismic_exception.h"

#include <iostream>
#include <iterator>
#include <string>
#include <regex>

using namespace std;


string first_to_upper(const string &s) {
    string res = s;
    res[0] = static_cast<string::value_type>(toupper(res[0]));
    return res;
}

string first_to_lower(const string &s) {
    string res = s;
    res[0] = static_cast<string::value_type>(tolower(res[0]));
    return res;
}

#define TO_USERFRIENDLY_STRING(str)                                                 \
    first_to_upper(regex_replace(str, regex("_"), " "))

#define FROM_USERFRIENDLY_STRING(str)                                               \
    first_to_lower(regex_replace(str, regex(" "), "_"))

#define SyBH_READ_FROM_RAW_BUFFER_OP(z, data, el)                                   \
    BOOST_PP_CAT(m_, SyBH_FIELD_NAME(el)) = static_cast<SyBH_FIELD_OUT_TYPE(el)>(   \
        BOOST_PP_CAT(byte_to_, SyBH_FIELD_RAW_TYPE(el)) (                           \
            &m_raw_data[SyBH_FIELD_OFFSET(el)],                                     \
            m_endian_order                                                          \
        )                                                                           \
    );

#define SyBH_READ_FROM_RAW_BUFFER(seq)                                              \
    BOOST_PP_SEQ_FOR_EACH(SyBH_READ_FROM_RAW_BUFFER_OP, ~, seq)

#define SyBH_WRITE_TO_RAW_BUFFER_OP(z, data, el)                                    \
    SyBH_FIELD_RAW_TYPE(el)_to_byte(                                                \
        static_cast<SyBH_FIELD_RAW_TYPE(el)>(BOOST_PP_CAT(m_, SyBH_FIELD_NAME(el))),\
        &m_raw_data[SyBH_FIELD_OFFSET(el)],                                         \
        m_endian_order                                                              \
    );

#define SyBH_WRITE_TO_RAW_BUFFER(seq)                                               \
        BOOST_PP_SEQ_FOR_EACH(SyBH_WRITE_TO_RAW_BUFFER_OP, ~, seq)

#define SyBH_WRITE_MAP_OP(z, data, el)                                              \
    {                                                                               \
        TO_USERFRIENDLY_STRING(BOOST_PP_STRINGIZE(SyBH_FIELD_NAME(el))),            \
        enum_cast<decltype(BOOST_PP_CAT(m_, SyBH_FIELD_NAME(el)))>::cast(           \
            BOOST_PP_CAT(m_, SyBH_FIELD_NAME(el))                                   \
        )                                                                           \
    },

#define SyBH_WRITE_MAP(seq)                                                         \
    m_fields = {                                                                    \
        BOOST_PP_SEQ_FOR_EACH(SyBH_WRITE_MAP_OP, ~, seq)                            \
    };

#define SyBH_SET_FIELD_SWITCH_OP(z, data, el)                                       \
    if (FROM_USERFRIENDLY_STRING(name) == BOOST_PP_STRINGIZE(SyBH_FIELD_NAME(el))) {\
        SyBH_FIELD_OUT_TYPE(el) __val;                                              \
        VARIANT_VALUE_CAST(__val, value);                                           \
        BOOST_PP_CAT(set_, SyBH_FIELD_NAME(el))(__val);                             \
        return;                                                                     \
    }

#define SyBH_SET_FIELD_SWITCH(seq)                                                  \
        BOOST_PP_SEQ_FOR_EACH(SyBH_SET_FIELD_SWITCH_OP, ~, seq)

#define SyBH_INIT_DESCR_OP(z, data, el)                                             \
    { TO_USERFRIENDLY_STRING(BOOST_PP_STRINGIZE(SyBH_FIELD_NAME(el))),              \
        {{                                                                          \
            "Bytes",                                                                \
            to_string(segy_file::text_header_size + 1 + SyBH_FIELD_OFFSET(el))      \
            + "-" + to_string(segy_file::text_header_size +                         \
                SyBH_FIELD_OFFSET(el) + sizeof(SyBH_FIELD_RAW_TYPE(el))             \
            )                                                                       \
        },                                                                          \
        {                                                                           \
            "Description",                                                          \
            SyBH_FIELD_DESCR(el)                                                    \
        }}                                                                          \
    },

#define SyBH_INIT_DESCR(seq)                                                        \
    map<string, map<string, string>> segy_bin_header::m_descr = {                   \
        BOOST_PP_SEQ_FOR_EACH(SyBH_INIT_DESCR_OP, ~, seq)                           \
        {                                                                           \
            TO_USERFRIENDLY_STRING("endian"),                                       \
            {                                                                       \
                {                                                                   \
                    "Bytes",                                                        \
                    ""                                                              \
                },                                                                  \
                {                                                                   \
                    "Description",                                                  \
                    "Byte ordering to expect for this SEG-Y file:\n\t0 - big"       \
                    "\n\t1 - little\n\t2 - mid_big\n\t3 - mid_little"               \
                }                                                                   \
            }                                                                       \
        }                                                                           \
    };


SyBH_INIT_DESCR(SyBH_FIELDS);

segy_bin_header::~segy_bin_header() { }

segy_bin_header::segy_bin_header() {
	set_zero();
}

segy_bin_header::segy_bin_header(const vector<byte_t> &m_raw_data) {
    if (m_raw_data.size() != segy_file::bin_header_size)
        SR_THROW(runtime_error, "binary header has invalid size");

    this->m_raw_data = m_raw_data;
    init_endian();
	init_from_raw();

    m_modifier = modifier::read_only;
}

segy_bin_header::segy_bin_header(const map<string, seismic_variant_value> &map)
    : segy_bin_header() {
    
    from_map(map);
}

void segy_bin_header::set_zero() {
    m_raw_data = vector<byte_t>(segy_file::bin_header_size, (byte_t) 0);
    init_from_raw();
}

void segy_bin_header::init_endian() {
    int little_to_native = byte_to_int<endian_order::little>(&m_raw_data[96]);
    int big_to_native = byte_to_int<endian_order::big>(&m_raw_data[96]);
    int midlittle_to_native = byte_to_int<endian_order::mid_little>(&m_raw_data[96]);

    if (little_to_native == 16909060)
        m_endian_order = endian_order::little;
    else if (big_to_native == 16909060)
        m_endian_order = endian_order::big;
    else if (midlittle_to_native == 16909060)
        m_endian_order = endian_order::mid_little;
    else if (big_to_native == 16909060)
        m_endian_order = endian_order::mid_big;
    else {
        int data_format_little = byte_to_ushort<endian_order::little>(&m_raw_data[24]);
        int data_format_big = byte_to_ushort<endian_order::big>(&m_raw_data[24]);
        if (data_format_little >= 1 && data_format_little <= 16)
            m_endian_order = endian_order::little;
        else if (data_format_big >= 1 && data_format_big <= 16)
            m_endian_order = endian_order::big;
        else {
            cout << "Warning: can't determine endianness. "
                "Data may be incorrect, try to change endianness by yourself." << endl;
            m_endian_order = endian_order::big;
        }
    }
}

void segy_bin_header::write_endian() {
    ushort_to_byte((uint16_t)16909060, &m_raw_data[96], m_endian_order);
}

void segy_bin_header::init_from_raw() {
    SyBH_READ_FROM_RAW_BUFFER(SyBH_FIELDS);
    override_by_extended();
    init_map();
}

void segy_bin_header::write_to_raw() {
    SyBH_WRITE_TO_RAW_BUFFER(SyBH_FIELDS);
    write_endian();
}

void segy_bin_header::override_by_extended() {
    if (m_extended_traces_count != 0)
        m_traces_count = m_extended_traces_count;
    if (m_extended_aux_traces_count != 0)
        m_aux_traces_count = m_extended_aux_traces_count;
    if (m_extended_samples_count != 0)
        m_samples_count = m_extended_samples_count;
    if (m_extended_sample_interval != 0)
        m_sample_interval = m_extended_sample_interval;
    if (m_extended_sample_interval_orig != 0)
        m_sample_interval_orig = m_extended_sample_interval_orig;
    if (m_extended_ensemble_fold != 0)
        m_ensemble_fold = m_extended_ensemble_fold;
}

void segy_bin_header::init_map() {
    SyBH_WRITE_MAP(SyBH_FIELDS);
    m_fields[TO_USERFRIENDLY_STRING("endian")] = static_cast<int>(m_endian_order);
    m_map_need_update = false;
}

seismic_variant_value segy_bin_header::get(const string &name) {
    if (m_map_need_update)
        init_map();
    auto it = m_fields.find(TO_USERFRIENDLY_STRING(name));
    if (it != m_fields.cend())
        return it->second;
    else
        SR_THROW(invalid_argument, "invalid field name");
}

void segy_bin_header::set(const string &name, seismic_variant_value value) {
    if (m_modifier == modifier::read_only)
        SR_THROW(runtime_error, "object is read only");

    auto it = m_fields.find(TO_USERFRIENDLY_STRING(name));
    if (it != m_fields.cend()) {
        SyBH_SET_FIELD_SWITCH(SyBH_FIELDS);

        if (FROM_USERFRIENDLY_STRING(name) == "endian") {
            int __endian_order;
            VARIANT_VALUE_CAST(__endian_order, value);
            set_endian(static_cast<endian_order>(__endian_order));
        }
    }
    else
        SR_THROW(invalid_argument, "invalid field name");
}

map<string, seismic_variant_value> segy_bin_header::to_map() {
    if (m_map_need_update)
        init_map();
	return m_fields;
}

void segy_bin_header::from_map(const map<string, seismic_variant_value> &map) {
    for (auto &p : map)
        set(p.first, p.second);
}

map<string, map<string, string>> segy_bin_header::fields_descr() {
    return m_descr;
}

vector<byte_t> segy_bin_header::raw_data() {
    if (m_modifier != modifier::read_only)
        write_to_raw();
    return m_raw_data;
}

#ifdef PYTHON
void py_segy_bin_header_init(
    py::module &m,
	py::class_<segy_bin_header, shared_ptr<segy_bin_header>> &py_segy_bin_header
) {
    py_segy_bin_header.def(py::init<>());
    py_segy_bin_header.def(
        py::init<const map<string, seismic_variant_value>>(),
        py::arg("fields_dict")
    );

    py_segy_bin_header.def(
        "raw_data", 
        &segy_bin_header::raw_data,
        "Returns byte array in SEG-Y raw format"
    );
}
#endif
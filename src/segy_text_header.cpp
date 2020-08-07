#include "segy_text_header.h"
#include "data_conversion.h"

#include <regex>
#include <sstream>

using namespace std;


int heaviside(int x) {
    return (x > 0) ? 1 : 0;
}

string name_to_3char(string name) {
    if (name.size() < 3)
        name.insert(name.begin() + 1, ' ');
    return name;
}


#define SyTH_WRITE_MAP_OP(z, data, el) {                                            \
        name_to_3char(BOOST_PP_STRINGIZE(SyTH_FIELD_NAME(el))),                     \
        SyTH_FIELD_TEMPLATE(el)                                                     \
    },

#define SyTH_WRITE_MAP(seq)                                                         \
    m_fields = {                                                                    \
        BOOST_PP_SEQ_FOR_EACH(SyTH_WRITE_MAP_OP, ~, seq)                            \
    };


string segy_text_header::m_parse_pattern = "(C ?[0-9]{1,2}) (.{76})";

segy_text_header::segy_text_header() {
    set_zero();
    init_map();
    m_modifier = modifier::read_write;
}

segy_text_header::segy_text_header(
    const vector<byte_t> &raw_data,
    bool is_ebdic
) : segy_text_header() {

    m_raw_data = raw_data;
    m_is_ebdic = is_ebdic;
    init_from_raw();
    m_modifier = modifier::read_only;
}

segy_text_header::segy_text_header(
    const map<string, seismic_variant_value> &map,
    bool is_ebdic
) : segy_text_header() {

    m_is_ebdic = is_ebdic;
    from_map(map);
    m_modifier = modifier::read_write;
}

seismic_variant_value segy_text_header::get(const std::string &name) {
    auto it = m_fields.find(name_to_3char(name));
    if (it != m_fields.cend())
        return it->second;
    else
        SR_THROW(invalid_argument, "invalid field name");
}

map<string, seismic_variant_value> segy_text_header::to_map() {
    return m_fields;
}

void segy_text_header::set(const string &name, seismic_variant_value val) {
    if (m_modifier == modifier::read_only)
        SR_THROW(runtime_error, "object is read only");

    string name_3char = name_to_3char(name);

    auto it = m_fields.find(name_3char);
    if (it != m_fields.cend()) {
        string __str;
        VARIANT_VALUE_CAST(__str, val);
        
        if (__str.size() > 76)
            SR_THROW(invalid_argument, "invalid text header field size (use "
                "string with length not greater than 76 characters)");

        m_fields[name_3char] = __str;
    }
    else
        SR_THROW(invalid_argument, "invalid field name");
}

void segy_text_header::from_map(const map<string, seismic_variant_value> &map) {
    for (auto &it : map)
        set(it.first, it.second);
}

map<string, map<string, string>> segy_text_header::fields_descr() {
    return { };
}

void segy_text_header::set_zero() {
    m_raw_data = vector<byte_t>(segy_file::text_header_size, (byte_t)0);
}

void segy_text_header::init_map() {
    SyTH_WRITE_MAP(SyTH_FIELDS);
}

void segy_text_header::init_from_raw() {
    string raw_str;

    if (m_is_ebdic)
        raw_str = ebcdic_to_char(reinterpret_cast<char*>(m_raw_data.data()));
    else
        raw_str = string(reinterpret_cast<char*>(m_raw_data.data()));

    regex e(m_parse_pattern);
    smatch m;

    while (regex_search(raw_str, m, e)) {
        set(m[1], m[2]);
        raw_str = m.suffix().str();
    }
}

void segy_text_header::write_to_raw() {
    string raw_str = "";
    regex e("%[A-Za-z_]+");

    for (auto &it : m_fields) {
        string field_srt = tab_to_space(std::get<string>(it.second));

        field_srt = regex_replace(field_srt, e, "<und>");

        int size76 = 76 - field_srt.size();

        if (field_srt.size() > 76)
            field_srt.erase(field_srt.begin() + 76, field_srt.end());

        raw_str.append(
            it.first 
            + " "
            + field_srt.append(heaviside(size76) * size76, ' ')
        );
    }

    raw_str = char_to_ecdic(raw_str);

    memcpy(
        m_raw_data.data(), 
        reinterpret_cast<const byte_t*>(raw_str.c_str()),
        raw_str.size()
    );
}

vector<byte_t> segy_text_header::raw_data() {
    if (m_modifier != modifier::read_only)
        write_to_raw();
    return m_raw_data;
}

void segy_text_header::subs(const string &pattern, seismic_variant_value val) {
    for (auto &it : m_fields) {

        ostringstream val_srt;
        if (holds_alternative<string>(val))
            val_srt << std::get<string>(val);
        else {
            if (is_integral_type(val)) {
                size_t int64_val;
                VARIANT_VALUE_CAST(int64_val, val);

                val_srt << int64_val;
            }
            else {
                double double_val;
                VARIANT_VALUE_CAST(double_val, val);

                val_srt << double_val;
            }
        }

        it.second = regex_replace(
            std::get<string>(it.second),
            regex(pattern),
            val_srt.str()
        );
    }
}

void segy_text_header::set_product_name(const string &name) {
    subs("%ProductName", name);
}

void segy_text_header::set_date_time(const std::string &name) {
    subs("%DateTime", name);
}

void segy_text_header::set_object_name(const string &name) {
    subs("%name", name);
}

void segy_text_header::set_seismic_type(const string &type) {
    subs("%type", type);
}

#ifdef PYTHON
void py_segy_text_header_init(
    py::module &m,
    py::class_<segy_text_header,
    std::shared_ptr<segy_text_header>> &py_segy_text_header
) { 
    py_segy_text_header.def(py::init<>());
    py_segy_text_header.def(py::init<const vector<byte_t> &, bool>());
    py_segy_text_header.def(
        py::init<const map<string, seismic_variant_value> &, bool>(),
        py::arg("fields_dict"),
        py::arg("is_ebdic") = true
    );
    py_segy_text_header.def("raw_data", &segy_text_header::raw_data);

    py_segy_text_header.def("set_product_name", &segy_text_header::set_product_name);
    py_segy_text_header.def("set_object_name", &segy_text_header::set_object_name);
    py_segy_text_header.def("set_seismic_type", &segy_text_header::set_seismic_type);
}
#endif
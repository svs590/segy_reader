#include <string>
#include <regex>
#include <iterator>

#include <boost/container_hash/hash.hpp>
#include <boost/preprocessor.hpp>
#include <boost/preprocessor/control.hpp>

#include "segy_header_map.h"
#include "data_types.h"
#include "utils.h"

using namespace std;


#define EMPTY ;

#define SyHM_FIELD_NAME(tuple)                                                      \
    BOOST_PP_TUPLE_ELEM(6, 0, tuple)

#define SyHM_FIELD_TYPE(tuple)                                                  \
    BOOST_PP_TUPLE_ELEM(6, 1, tuple)

#define SyHM_FIELD_OFFSET(tuple)                                                \
    BOOST_PP_TUPLE_ELEM(6, 2, tuple)

#define SyHM_FIELD_SIZE(tuple)                                                  \
    BOOST_PP_TUPLE_ELEM(6, 3, tuple)

#define SyHM_IS_FIELD_REQ(tuple)                                                \
    BOOST_PP_TUPLE_ELEM(6, 4, tuple)

#define SyHM_FIELD_DESCR(tuple)                                                 \
    BOOST_PP_TUPLE_ELEM(6, 5, tuple)

#define SyHM_FILL_REQ_OP(z, data, el)                                           \
    BOOST_PP_IF(                                                                \
        SyHM_IS_FIELD_REQ(el), data.push_back({                                 \
            TO_USERFRIENDLY_STRING(BOOST_PP_STRINGIZE(SyHM_FIELD_NAME(el))), {  \
                SyHM_FIELD_TYPE(el), SyHM_FIELD_OFFSET(el), SyHM_FIELD_SIZE(el),\
                static_cast<bool>(SyHM_IS_FIELD_REQ(el))                        \
            }                                                                   \
        }), EMPTY                                                               \
    );

#define SyHM_FILL_REQ(seq, container)                                           \
    BOOST_PP_SEQ_FOR_EACH(SyHM_FILL_REQ_OP, container, seq)

#define SyHM_FILL_STD_OP(z, data, el)                                           \
    data.push_back({                                                            \
        TO_USERFRIENDLY_STRING(BOOST_PP_STRINGIZE(SyHM_FIELD_NAME(el))), {      \
            SyHM_FIELD_TYPE(el), SyHM_FIELD_OFFSET(el), SyHM_FIELD_SIZE(el),    \
            static_cast<bool>(SyHM_IS_FIELD_REQ(el))                            \
        }                                                                       \
    });

#define SyHM_FILL_STD(seq, container)                                           \
    BOOST_PP_SEQ_FOR_EACH(SyHM_FILL_STD_OP, container, seq)

#define SyHM_FILL_DESCR_REQ_OP(z, data, el)                                     \
    BOOST_PP_IF(                                                                \
        SyHM_IS_FIELD_REQ(el), data.push_back({                                 \
            TO_USERFRIENDLY_STRING(BOOST_PP_STRINGIZE(SyHM_FIELD_NAME(el))),    \
            SyHM_FIELD_DESCR(el)                                                \
        }), EMPTY                                                               \
    );

#define SyHM_FILL_DESCR_REQ(seq, container)                                     \
    BOOST_PP_SEQ_FOR_EACH(SyHM_FILL_DESCR_REQ_OP, container, seq)

#define SyHM_FILL_DESCR_STD_OP(z, data, el)                                     \
    data.push_back({                                                            \
        TO_USERFRIENDLY_STRING(BOOST_PP_STRINGIZE(SyHM_FIELD_NAME(el))),        \
        SyHM_FIELD_DESCR(el)                                                    \
    });

#define SyHM_FILL_DESCR_STD(seq, container)                                     \
    BOOST_PP_SEQ_FOR_EACH(SyHM_FILL_DESCR_STD_OP, container, seq)


seismic_header_map::storage_t segy_header_map::m_fields_required = {};

segy_header_map::segy_header_map() : segy_header_map(header_map_type::STANDARD) {}

segy_header_map::segy_header_map(header_map_type type) {
    // Static init
    if (m_fields_required.empty())
        SyHM_FILL_REQ(SyHM_FIELDS, m_fields_required);

    m_type = type;
    m_update_hash = true;

    switch (m_type)
    {
    case header_map_type::CUSTOM:
        m_fields.clear();
        fill_required();
        break;
    case header_map_type::STANDARD:
        m_fields.clear();
        fill_standard();
        break;
    default:
        SR_THROW(invalid_argument, "invalid header map type");
        break;
    }
}

segy_header_map::segy_header_map(const map_storage_t &map) {
    from_map(map);
}

segy_header_map::segy_header_map(const segy_header_map &map) {
    m_type          = map.m_type;
    m_fields        = map.m_fields;
    m_descr         = map.m_descr;
    m_update_hash   = true;
}

segy_header_map::segy_header_map(shared_ptr<segy_header_map> map) {
    m_type          = map->m_type;
    m_fields        = map->m_fields;
    m_descr         = map->m_descr;
    m_update_hash   = true;
}

segy_header_map::segy_header_map(shared_ptr<seismic_header_map> map) {
    // TODO: cast to segy
    from_map(map->to_map());
}

void segy_header_map::set(
    string     name,
    header_field_t  val
) {
    set(name, std::get<0>(val), std::get<1>(val), std::get<2>(val), "");
}

void segy_header_map::set(
    string     name,
    header_field_t  val,
    string     desc
) {
    set(name, std::get<0>(val), std::get<1>(val), std::get<2>(val), desc);
}

void segy_header_map::set(
    string name,
    seismic_data_type type,
    int byte_loc,
    int byte_size
) {
    set(name, type, byte_loc, byte_size, "");
}

void segy_header_map::set(
    string name,
    seismic_data_type type,
    int byte_loc,
    int byte_size,
    string desc
) {
    byte_size = seismic_data_type_size(type);

    int idx = contains(name, m_fields);
    if (idx != NOT_INDEX) {
        if (byte_loc != std::get<1>(m_fields[idx].second))
            m_type = header_map_type::CUSTOM;

        m_fields[idx].second = { 
            type, byte_loc, byte_size, std::get<3>(m_fields[idx].second) 
        };

        if (!desc.empty())
            m_descr[idx] = { name,  desc };
    }
    else {
        m_fields.push_back(
            { name, 
                { type, byte_loc, byte_size, std::get<3>(m_fields[idx].second) } 
            }
        );
        m_descr.push_back({ name, desc });
    }

    m_update_hash = true;
}

void segy_header_map::remove(const string &name) {
    int idx = contains(name, m_fields);
    if (idx != NOT_INDEX)
        if (contains(name, m_fields_required) == NOT_INDEX)
            m_fields.erase(m_fields.begin() + idx);
        else
            SR_THROW(invalid_argument, "can't remove required header map field");
    
    m_update_hash = true;
}

void segy_header_map::clear() {
    m_fields.clear();
    fill_required();
    m_type = header_map_type::CUSTOM;

    m_update_hash = true;
}

bool segy_header_map::contains(const string &name) const {
    return contains(name, m_fields) != NOT_INDEX;
}

seismic_header_map::header_field_t segy_header_map::get(
    const string &name
) const {
    int idx = contains(name, m_fields);

    if (idx >= 0 && idx < m_fields.size())
        return m_fields[idx].second;
    else
        SR_THROW(
            invalid_argument, 
            "header map doesn't contains field with name " + name
        );
}

pair<string, seismic_header_map::header_field_t>
segy_header_map::get(int index) const {
    if (index >= 0 && index < m_fields.size())
        return m_fields[index];
    else
        SR_THROW(invalid_argument, "invalid index");
}

size_t segy_header_map::count() const {
    return m_fields.size();
}

header_map_type segy_header_map::type() const {
    return m_type;
}

seismic_header_map::map_storage_t segy_header_map::to_map() const {
    seismic_header_map::map_storage_t res;
    copy(m_fields.begin(), m_fields.end(), inserter(res, res.begin()));
    return res;
}

void segy_header_map::from_map(const seismic_header_map::map_storage_t &m) {
    clear();
    for (auto &field : m) {
        int index = contains(field.first, m_fields);
        if (index != NOT_INDEX)
            m_fields[index].second = field.second;
        else
            m_fields.push_back({ field.first, field.second });
    }
}

map<string, string> segy_header_map::fields_descr() {
    map<string, string> res;
    copy(m_descr.begin(), m_descr.end(), inserter(res, res.begin()));

    return res;
}

bool segy_header_map::operator==(shared_ptr<seismic_header_map> map) {
    auto syhm_map = dynamic_pointer_cast<segy_header_map>(map);
    if (syhm_map != nullptr) 
        return operator==(syhm_map);
    else
        SR_THROW(invalid_argument, "Mixing inappropriate types");
}

bool segy_header_map::operator==(shared_ptr<segy_header_map> map) {
    return *this == *map;
}

bool segy_header_map::operator==(segy_header_map &map) {
    if (m_type == header_map_type::STANDARD
        && map.m_type == header_map_type::STANDARD)
        return true;

    if (m_update_hash)
        calc_hash();

    if (map.m_update_hash)
        map.calc_hash();

    return m_hash == map.m_hash;
}

int segy_header_map::contains(
    const string &name, 
    const seismic_header_map::storage_t &map
) const {
    for (int i = 0; i < map.size(); ++i)
        if (map[i].first == name)
            return i;

    return NOT_INDEX;
}

void segy_header_map::fill_required() {
    SyHM_FILL_REQ(SyHM_FIELDS, m_fields);
    SyHM_FILL_DESCR_REQ(SyHM_FIELDS, m_descr);
}

void segy_header_map::fill_standard() {
    SyHM_FILL_STD(SyHM_FIELDS, m_fields);
    SyHM_FILL_DESCR_STD(SyHM_FIELDS, m_descr);
}

void segy_header_map::calc_hash() {
    m_hash = boost::hash_value(m_fields);
}

#ifdef PYTHON
void py_segy_header_map_init(
    py::module &m,
    py::class_<segy_header_map, shared_ptr<segy_header_map>> &py_segy_header_map
) {
    py_segy_header_map.def(py::init<>());

    py_segy_header_map.def(
        py::init<header_map_type>(),
        py::arg("map_type")
    );

    py_segy_header_map.def(
        py::init<const seismic_header_map::map_storage_t &>(),
        py::arg("dict")
    );

    py_segy_header_map.def(
        py::init<shared_ptr<segy_header_map>>(),
        py::arg("header_map")
    );

    py_segy_header_map.def(
        py::init<shared_ptr<seismic_header_map>>(),
        py::arg("header_map")
    );
}
#endif

#pragma once

#include <string>
#include <map>
#include <variant>
#include <optional>

#include "seismic_abstract_header.h"
#include "seismic_text_header.h"

#include <boost/preprocessor.hpp>
#include <boost/preprocessor/facilities/is_empty_variadic.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#ifdef PYTHON
#include <pybind11/pybind11.h>

namespace py = pybind11;
#endif


#define SyTH_FIELD_NAME(tuple)                                                      \
    BOOST_PP_TUPLE_ELEM(3, 0, tuple)

#define SyTH_FIELD_TEMPLATE(tuple)                                                  \
    BOOST_PP_TUPLE_ELEM(3, 1, tuple)

#define SyTH_FIELD_SUBS(tuple)                                                      \
    BOOST_PP_TUPLE_ELEM(3, 2, tuple)

#define DEFINE_SyBH_SET_METHODS_OP_TEXT(subs_name)                                  \
    void                                                                            \
    BOOST_PP_CAT(set_, subs_name)(const seismic_variant_value &val) {               \
        subs(std::string("%") + BOOST_PP_STRINGIZE(subs_name), val);                \
    }

#define DEFINE_SyBH_SET_METHODS_OP_II(z, data, i, el)                               \
    BOOST_PP_IF(BOOST_PP_IS_EMPTY(el), ,DEFINE_SyBH_SET_METHODS_OP_TEXT(el))

#define DEFINE_SyBH_SET_METHODS_OP_I(seq)                                           \
    BOOST_PP_SEQ_FOR_EACH_I(DEFINE_SyBH_SET_METHODS_OP_II, ~, seq)

#define DEFINE_SyBH_SET_METHODS_OP(z, data, el)                                     \
    DEFINE_SyBH_SET_METHODS_OP_I(SyTH_FIELD_SUBS(el))
    

#define DEFINE_SyTH_METHODS(seq)                                                    \
    BOOST_PP_SEQ_FOR_EACH(DEFINE_SyBH_SET_METHODS_OP, ~, seq)


class segy_text_header : public seismic_abstract_header {

/**
  * Segy text header fieds tuple like (name, basic_template, seq_of_subs)
  * where
  *     name            - field name
  *     basic template  - basic template
  *     seq_of_subs     - sequence of substitution which replace template
  *                       substring kind of %sub to value
  */
#define SyTH_FIELDS                                                                     \
    ((C1,       "SEG-Y OUTPUT FROM %ProductName, %DateTime", ()))                       \
    ((C2,       "NAME: %name\tType: %type", ()))                                        \
    ((C3,       "", ()))                                                                \
    ((C4,       "Additional text headers count:\t%add_text_headers_count",              \
        (add_text_headers_count)))                                                      \
    ((C5,       "", ()))                                                                \
    ((C6,       "AREA INFO", ()))                                                       \
    ((C7,       "First CDP:\t%first_cdp,\tLast CDP:\t%last_cdp",                        \
        (first_cdp) (last_cdp)))                                                        \
    ((C8,       "X min:\t\t%x_min,\tX max:\t\t%x_max,\tDelta:\t%x_delta"),              \
        (x_min) (x_max) (x_delta))                                                      \
    ((C9,       "Y min:\t\t%y_min,\tY max:\t\t%y_max,\tDelta:\t%y_delta",               \
        (y_min) (y_max) (y_delta)))                                                     \
    ((C10,       "Time min:\t%time_min,\tTime max:\t%time_max,\tDelta:\t%time_delta",   \
        (time_min) (time_max) (time_delta)))                                            \
    ((C11,       "Lat min:\t%lat_min,\tLat max:\t%lat_max,\tDelta:\t%lat_delta",        \
        (lat_min) (lat_max) (lat_delta)))                                               \
    ((C12,       "Long min:\t%long_min,\tLong max:\t%long_max,\tDelta:\t%long_delta",   \
        (long_min) (long_max) (long_delta)))                                            \
    ((C13,       "Trace min:\t%trace_min,\tTrace max:\t%trace_max,\tDelta:\t%trace_delta",\
        (trace_min) (trace_max) (trace_delta)))                                         \
    ((C14,       "", ()))                                                               \
    ((C15,       "BINARY HEADER INFO", ()))                                             \
    ((C16,       "Location:\t\t\t\t\t\t%bin_header_loc",                                \
        (bin_header_loc)))                                                              \
    ((C17,       "Sample interval:\t\t\t\t%bin_header_sample_interval_loc"),            \
        (bin_header_sample_interval_loc))                                               \
    ((C18,       "Number of samples per trace:\t%bin_header_samples_count_loc",         \
        (bin_header_samples_count_loc)))                                                \
    ((C19,       "Trace data format:\t\t\t\t%bin_header_data_format_loc",               \
        (bin_header_data_format_loc)))                                                  \
    ((C20,       "", ()))                                                               \
    ((C21,       "TRACE HEADER INFO", ()))                                              \
    ((C22,       "Location:\t\t\t\t\t\t%trace_header_loc",                              \
        (trace_header_loc)))                                                            \
    ((C23,       "Inline number:\t\t\t\t\t%trace_header_iline_loc",                     \
        (trace_header_il_loc)))                                                         \
    ((C24,       "Crossline number:\t\t\t\t%trace_header_crossline_loc",                \
        (trace_header_crossline_loc)))                                                  \
    ((C25,       "CDP X:\t\t\t\t\t\t\t%trace_header_cdpx_loc",                          \
        (trace_header_cdpx_loc)))                                                       \
    ((C26,       "CDP Y:\t\t\t\t\t\t\t%trace_header_cdpy_loc",                          \
        (trace_header_cdpy_loc)))                                                       \
    ((C27,       "Source coordinate X:\t\t\t%trace_header_srcx_loc",                    \
        (trace_header_srcx_loc)))                                                       \
    ((C28,       "Source coordinate Y:\t\t\t%trace_header_srcy_loc",                    \
        (trace_header_srcy_loc)))                                                       \
    ((C29,       "Coordinate scale factor:\t\t%trace_header_scale_factor_loc",          \
        (trace_header_scale_factor_loc)))                                               \
    ((C30,       "Number of samples per trace:\t%trace_header_samples_count_loc",       \
        (trace_header_samples_count_loc)))                                              \
    ((C31,       "Sample interval:\t\t\t\t%trace_header_sample_interval_loc",           \
        (trace_header_sample_interval_loc)))                                            \
    ((C32,       "Additional trace headers count: %add_trace_header_count",             \
        (add_trace_header_count)))                                                      \
    ((C33,       "", ()))                                                               \
    ((C34,       "", ()))                                                               \
    ((C35,       "", ()))                                                               \
    ((C36,       "", ()))                                                               \
    ((C37,       "", ()))                                                               \
    ((C38,       "", ()))                                                               \
    ((C39,       "", ()))                                                               \
    ((C40,       "END TEXTUAL HEADER", ()))                                             \

    static std::string m_parse_pattern;
    bool m_is_ebdic = true;


public:
    segy_text_header();
    segy_text_header(
        const std::vector<byte_t> &raw_data, 
        bool is_ebdic=true
    );
    segy_text_header(
        const std::map<std::string, seismic_variant_value> &map, 
        bool is_ebdic = true
    );

    virtual seismic_variant_value get(const std::string &name);
    virtual std::map<std::string, seismic_variant_value> to_map();

    virtual void set(const std::string &name, seismic_variant_value val);
    virtual void from_map(const std::map<std::string, seismic_variant_value> &map);

    virtual std::map<std::string, std::map<std::string, std::string>> fields_descr();
    
    std::vector<byte_t> raw_data();

    void set_product_name(const std::string &name);
    void set_object_name(const std::string &name);
    void set_seismic_type(const std::string &type);

protected:
    DEFINE_SyTH_METHODS(SyTH_FIELDS);

private:
    void init_map();
    void init_from_raw();
    void write_to_raw();
    void set_zero();
    void subs(const std::string &pattern, seismic_variant_value val);
};


#ifdef PYTHON
void py_segy_text_header_init(
    py::module &m,
    py::class_<segy_text_header, 
        std::shared_ptr<segy_text_header>> &py_segy_text_header
);
#endif

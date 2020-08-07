#include "data_types.h"
#include "data_conversion.h"

using namespace std;


template <class Int>
struct next_integer { 
    using next = int64_t;
};

template <>
struct next_integer<int8_t> {
    using next = int16_t;
};

template <>
struct next_integer<uint8_t> {
    using next = int16_t;
};

template <>
struct next_integer<int16_t> {
    using next = int32_t;
};

template <>
struct next_integer<uint16_t> {
    using next = int32_t;
};

template <>
struct next_integer<int32_t> {
    using next = int64_t;
};

template <>
struct next_integer<uint32_t> {
    using next = int64_t;
};

template <typename Type_first, typename Type_second>
struct containing_type {
    using type = conditional_t<
        is_arithmetic<Type_first>::value && is_arithmetic<Type_second>::value,

        conditional_t<
            is_integral<Type_first>::value && is_integral<Type_second>::value,

            conditional_t<
                is_unsigned<Type_first>::value && is_unsigned<Type_second>::value,

                conditional_t<
                    (sizeof(Type_first) > sizeof(Type_second)),
                    Type_first,
                    Type_second
                >,

                conditional_t<
                    (sizeof(Type_first) > sizeof(Type_second)),
                    typename next_integer<Type_first>::next,
                    typename next_integer<Type_second>::next
                >
            >,

            conditional_t<
                (sizeof(Type_first) > sizeof(Type_second)),
                Type_first,
                Type_second
            >
        >,
        
        string
    >;
};

bool is_integral_type(const seismic_variant_value &val) {
    bool res = false;

    visit(
        [&res](auto &&arg) {
            res = is_integral<decltype(arg)>::value;
        },
        val
    );

    return res;
}

segy_data_format segy_format_from_data(const seismic_variant_vector &val) {
    segy_data_format res;
    
    visit(
        [&res](auto &&arg) {
            using T = decay_t<decltype(arg)>::value_type;
            
            if (typeid(T).hash_code() == typeid(short).hash_code())
                res = segy_data_format::int16_2complement;
            else if (typeid(T).hash_code() == typeid(unsigned short).hash_code())
                res = segy_data_format::uint16;
            else if (typeid(T).hash_code() == typeid(int).hash_code())
                res = segy_data_format::int32_2complement;
            else if (typeid(T).hash_code() == typeid(uint32_t).hash_code())
                res = segy_data_format::uint32;
            else if (typeid(T).hash_code() == typeid(int64_t).hash_code())
                res = segy_data_format::int64_2complement;
            else if (typeid(T).hash_code() == typeid(uint64_t).hash_code())
                res = segy_data_format::uint64;
            else if (typeid(T).hash_code() == typeid(float).hash_code())
                res = segy_data_format::float32;
            else if (typeid(T).hash_code() == typeid(double).hash_code())
                res = segy_data_format::float64;
            else
                res = segy_data_format::float32;    ///< FIXME, throw exception
        },
        val
    );

    return res;

}

#define SEISMIC_VARIANT_VALUE_SIZE_OP(z, data, el)                              \
    case seismic_data_type:: ## BOOST_PP_TUPLE_ELEM(2, 0, el):                  \
        BOOST_PP_IF(BOOST_PP_IS_EMPTY(BOOST_PP_TUPLE_ELEM(2, 1, el)),           \
            return 0,                                                           \
            if (typeid(BOOST_PP_TUPLE_ELEM(2, 1, el)) == typeid(nullopt_t))     \
                return 0;                                                       \
            else                                                                \
                return sizeof(BOOST_PP_TUPLE_ELEM(2, 1, el));                   \
        );                                                                      \
        break;

#define SEISMIC_VARIANT_VALUE_SIZE_SWITCH(type)                                 \
    switch (type) {                                                             \
        BOOST_PP_SEQ_FOR_EACH(                                                  \
            SEISMIC_VARIANT_VALUE_SIZE_OP,                                      \
            ~,                                                                  \
            SEISMIC_DATA_TYPE                                                   \
        )                                                                       \
                                                                                \
        default:                                                                \
            return 0;                                                           \
            break;                                                              \
    }

int seismic_data_type_size(seismic_data_type dt) {
    SEISMIC_VARIANT_VALUE_SIZE_SWITCH(dt);
}

namespace seismic_variant_operations {

    template <typename T>
    inline int greater(const seismic_variant_value& left, T &right) {
        return visit([right](auto& val_1) -> int {
            using T1 = decay_t<decltype(val_1)>;
            using T3 = containing_type<T1, T>::type;
    
            T3 val_1_c;
            T3 val_2_c;
    
            caster_selector<T1, T3>::type::cast(val_1_c, val_1);
            caster_selector<T, T3>::type::cast(val_2_c, right);
    
            if (val_1_c > val_2_c)
                return 1;
            else if (val_1_c < val_2_c)
                return -1;
            return 0;
        }, left);
    }

    seismic_variant_value operator+(const seismic_variant_value& left, const seismic_variant_value& right) {

        containing_type<uint8_t, int32_t>::type;

        return visit([](auto& val_1, auto& val_2) -> seismic_variant_value {
            using T1 = decay_t<decltype(val_1)>;
            using T2 = decay_t<decltype(val_2)>;
            
            using T3 = containing_type<T1, T2>::type;
            T3 val_1_c;            
            T3 val_2_c;

            caster_selector<T1, T3>::type::cast(val_1_c, val_1);
            caster_selector<T2, T3>::type::cast(val_2_c, val_2);

            return val_1_c + val_2_c;

        }, left, right);
    }

    seismic_variant_value operator*(const seismic_variant_value& left, const seismic_variant_value& right) {
        return visit([](auto& val_1, auto& val_2) -> seismic_variant_value {
            using T1 = decay_t<decltype(val_1)>;
            using T2 = decay_t<decltype(val_2)>;

            using T3 = containing_type<T1, T2>::type;
            T3 val_1_c;
            T3 val_2_c;

            caster_selector<T1, T3>::type::cast(val_1_c, val_1);
            caster_selector<T2, T3>::type::cast(val_2_c, val_2);

            return val_1_c * val_2_c;

        }, left, right);
    }

    seismic_variant_value operator/(const seismic_variant_value& left, const seismic_variant_value& right) {
        return visit([](auto& val_1, auto& val_2) -> seismic_variant_value {
            using T1 = decay_t<decltype(val_1)>;
            using T2 = decay_t<decltype(val_2)>;

            using T3 = containing_type<T1, T2>::type;
            T3 val_1_c;
            T3 val_2_c;

            caster_selector<T1, T3>::type::cast(val_1_c, val_1);
            caster_selector<T2, T3>::type::cast(val_2_c, val_2);

            return val_1_c / val_2_c;

        }, left, right);
    }

    seismic_variant_value abs(const seismic_variant_value& val) {
        return visit([](auto& val) -> seismic_variant_value {
            using T1 = decay_t<decltype(val)>;
            return abs(val);

        }, val);
    }

    int size(const seismic_variant_vector& vec) {
        int res = 0;
        visit(
            [&res](auto& data) {
                res = data.size();
            },
            vec
        );
        return res;
    }

    template <>
    bool operator==(const seismic_variant_value &left, int right) {
        return greater<int>(left, right) == 0;
    }

    template <>
    bool operator==(const seismic_variant_value &left, int64_t right) {
        return greater<int64_t>(left, right) == 0;
    }

    template <>
    bool operator==(const seismic_variant_value &left, double right) {
        return greater<double>(left, right) == 0;
    }

    template <>
    bool operator>(const seismic_variant_value& val, int right) {
        return greater<int>(val, right) == 1;
    }
}
#include "data_types.h"
#include <iostream>

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
    using type = std::conditional_t<
        std::is_arithmetic<Type_first>::value && std::is_arithmetic<Type_second>::value,

        std::conditional_t<
            std::is_integral<Type_first>::value && std::is_integral<Type_second>::value,

            std::conditional_t<
                std::is_unsigned<Type_first>::value && std::is_unsigned<Type_second>::value,

                std::conditional_t<
                    (sizeof(Type_first) > sizeof(Type_second)),
                    Type_first,
                    Type_second
                >,

                std::conditional_t<
                    (sizeof(Type_first) > sizeof(Type_second)),
                    typename next_integer<Type_first>::next,
                    typename next_integer<Type_second>::next
                >
            >,

            std::conditional_t<
                (sizeof(Type_first) > sizeof(Type_second)),
                Type_first,
                Type_second
            >
        >,
        
        std::string
    >;
};

bool is_integral_type(const seismic_variant_value &val) {
    bool res = false;

    std::visit(
        [&res](auto &&arg) {
            res = std::is_integral<decltype(arg)>::value;
        },
        val
    );

    return res;
}

segy_data_format segy_format_from_data(const seismic_variant_vector &val) {
    segy_data_format res;
    
    std::visit(
        [&res](auto &&arg) {
            using T = std::decay_t<decltype(arg)>::value_type;
            
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

template <typename T>
inline int greater(const seismic_variant_value& left, T &right) {
    return std::visit([right](auto& val_1) -> int {
        using T1 = std::decay_t<decltype(val_1)>;
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

namespace seismic_variant_operations {

    seismic_variant_value operator+(const seismic_variant_value& left, const seismic_variant_value& right) {

        containing_type<uint8_t, int32_t>::type;

        return std::visit([](auto& val_1, auto& val_2) -> seismic_variant_value {
            using T1 = std::decay_t<decltype(val_1)>;
            using T2 = std::decay_t<decltype(val_2)>;
            
            using T3 = containing_type<T1, T2>::type;
            T3 val_1_c;            
            T3 val_2_c;

            caster_selector<T1, T3>::type::cast(val_1_c, val_1);
            caster_selector<T2, T3>::type::cast(val_2_c, val_2);

            return val_1_c + val_2_c;

        }, left, right);
    }

    seismic_variant_value operator*(const seismic_variant_value& left, const seismic_variant_value& right) {
        return std::visit([](auto& val_1, auto& val_2) -> seismic_variant_value {
            using T1 = std::decay_t<decltype(val_1)>;
            using T2 = std::decay_t<decltype(val_2)>;

            using T3 = containing_type<T1, T2>::type;
            T3 val_1_c;
            T3 val_2_c;

            caster_selector<T1, T3>::type::cast(val_1_c, val_1);
            caster_selector<T2, T3>::type::cast(val_2_c, val_2);

            return val_1_c * val_2_c;

        }, left, right);
    }

    int size(const seismic_variant_vector& vec) {
        int res = 0;
        std::visit(
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
}
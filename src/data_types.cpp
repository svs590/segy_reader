#include "data_types.h"

template <typename T>
inline int greater(const seismic_variant_value& left, T &right) {
    return std::visit([right](auto& val_1) -> int {
        if (val_1 > right)
            return 1;
        else if (val_1 < right)
            return -1;
        return 0;
    }, left);
}

namespace seismic_variant_operations {

    seismic_variant_value operator+(const seismic_variant_value& left, const seismic_variant_value& right) {
        return std::visit([](auto& val_1, auto& val_2) -> seismic_variant_value {
            using T1 = std::decay_t<decltype(val_1)>;
            using T2 = std::decay_t<decltype(val_2)>;
            if (std::is_integral<T1>::value && std::is_integral<T2>::value)
                if (std::is_unsigned<T1>::value == std::is_unsigned<T2>::value)
                    if (sizeof(T1) > sizeof(T2))
                        return val_1 + static_cast<T1>(val_2);
                    else
                        return static_cast<T2>(val_1) + val_2;
                else if (std::is_unsigned<T1>::value)
                    if (sizeof(T1) > sizeof(T2))
                        return static_cast<int64_t>(val_1) + static_cast<int64_t>(val_2);
                    else
                        return static_cast<T2>(val_1) + val_2;
                else
                    if (sizeof(T1) >= sizeof(T2))
                        return val_1 + static_cast<T1>(val_2);
                    else
                        return static_cast<int64_t>(val_1) + static_cast<int64_t>(val_2);
            return val_1 + val_2;
        }, left, right);
    }

    seismic_variant_value operator*(const seismic_variant_value& left, const seismic_variant_value& right) {
        return std::visit([](auto& val_1, auto& val_2) -> seismic_variant_value {
            using T1 = std::decay_t<decltype(val_1)>;
            using T2 = std::decay_t<decltype(val_2)>;
            if (std::is_integral<T1>::value && std::is_integral<T2>::value)
                if (std::is_unsigned<T1>::value == std::is_unsigned<T2>::value)
                    if (sizeof(T1) > sizeof(T2))
                        return val_1 * static_cast<T1>(val_2);
                    else
                        return static_cast<T2>(val_1) * val_2;
                else if (std::is_unsigned<T1>::value)
                    if (sizeof(T1) > sizeof(T2))
                        return static_cast<int64_t>(val_1) * static_cast<int64_t>(val_2);
                    else
                        return static_cast<T2>(val_1) * val_2;
                else
                    if (sizeof(T1) >= sizeof(T2))
                        return val_1 * static_cast<T1>(val_2);
                    else
                        return static_cast<int64_t>(val_1) * static_cast<int64_t>(val_2);
            return val_1 * val_2;
        }, left, right);
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
#include <array>
#include <algorithm>
#include <ostream>
#include <string>

#include <stdio.h>
#include <iostream>

#include "wlocale.h"

//Default units
//Metric base units
#define DIM_M(x) Value<x, 0, 1, {METER}, 0, {}>
#define DIM_G(x) Value<x, 0, 1, {GRAM}, 0, {}>
#define DIM_KG(x) Value<x, 3, 1, {GRAM}, 0, {}>
#define DIM_NS(x) Value<x, -9, 1, {SECOND}, 0, {}>
#define DIM_uS(x) Value<x, -6, 1, {SECOND}, 0, {}>
#define DIM_MS(x) Value<x, -3, 1, {SECOND}, 0, {}>
#define DIM_S(x) Value<x, 0, 1, {SECOND}, 0, {}>
#define DIM_A(x) Value<x, 0, 1, {AMPERE}, 0, {}>
#define DIM_K(x) Value<x, 0, 1, {KELVIN}, 0, {}>
#define DIM_MOL(x) Value<x, 0, 1, {MOL}, 0, {}>

//Measure type units
#define DIM_LEN(x) Value<x, 0, 1, {METER}, 0, {}>
#define DIM_MASS(x) Value<x, 3, 1, {GRAM}, 0, {}> //Use kg as base unit
#define DIM_TIME(x) Value<x, 0, 1, {SECOND}, 0, {}>
#define DIM_TEMP(x) Value<x, 0, 1, {KELVIN}, 0, {}>//NOTE: is in kelvin

//Common compound units
#define DIM_MPS(x) Value<x, 0, 1, {METER}, 1, {SECOND}>
#define DIM_MPSPS(x) Value<x, 0, 1, {METER}, 2, {SECOND, SECOND}>
#define DIM_N(x) Value<x, 3, 2, {METER, GRAM}, 2, {SECOND, SECOND}>//kgms^-2

//Common compound measure units
#define DIM_VEL(x) Value<x, 0, 1, {METER}, 1, {SECOND}>
#define DIM_ACC(x) Value<x, 0, 1, {METER}, 2, {SECOND, SECOND}>
#define DIM_FORCE(x) Value<x, 3, 2, {METER, GRAM}, 2, {SECOND, SECOND}>//kgms^-2

//Contains all metric base units
enum Unit {
    METER,
    GRAM,//Technically the kilogram is the base unit but this is better for consistency.
    SECOND,
    AMPERE,
    KELVIN,
    CANDELA,
    MOL,
};

//Helper functions
namespace {
    const std::string BASE_UNIT_SYMBOLS[7] = {
        "m",
        "g",
        "s",
        "A",
        "K",
        "cd",
        "mol"
    };

    const wchar_t SUPER_SCRIPT_SYMBOLS[13] {
        L'⁻',
        L' ',
        L' ',
        L'⁰',
        L'¹',
        L'²',
        L'³',
        L'⁴',
        L'⁵',
        L'⁶',
        L'⁷',
        L'⁸',
        L'⁹'
    };

    const std::string POSITIVE_METRIC_PREFIXES[12] {
        "k",
        "M",
        "G",
        "T",
        "P",
        "E",
        "Z",
        "Y",
        "R",
        "Q"
    };

    const std::string NEGATIVE_METRIC_PREFIXES[12] {
        "m",
        "u",//With unicode, should μ, will get fixed in function
        "n",
        "p",
        "f",
        "a",
        "z",
        "y",
        "r",
        "q"
    };

    template<class T, int exponent>
    constexpr std::string get_value_string(T v) {
        T value = v;
        std::string unit_string = "";
        if constexpr (exponent == 0) {
            unit_string = "";
        } else if constexpr (exponent > 0) {
            int index = exponent / 3;
            int factor = exponent % 3;
            if(index - 1 >= 0) {
                unit_string = POSITIVE_METRIC_PREFIXES[index - 1];
            }
            if(factor == 1) {
                value *= 10;
            }
            if(factor == 2) {
                value *= 100;
            }
        } else {
            int temp = -exponent;
            int index = temp / 3;
            int factor = temp % 3;
            if(factor != 0) {
                ++index;
            }
            if(index - 1 >= 0) {
                unit_string = NEGATIVE_METRIC_PREFIXES[index - 1];
            }
            if(factor == 1) {
                value *= 100;
            }
            if(factor == 2) {
                value *= 10;
            }
        }
        
        return std::to_string(value) + unit_string;
    }

    template<class T, int exponent>
    constexpr std::wstring wget_value_string(T v) {
        std::string tmp = get_value_string<T, exponent>(v);
        std::wstring result = std::wstring(tmp.begin(), tmp.end());
        for (std::size_t i = 0; i < result.size(); i++) {
            if(result[i] == L'u') {
                result[i] = L'μ';
            }
        }
        return result;
    }

    constexpr std::wstring wget_symbol(Unit u) {
        std::string tmp = BASE_UNIT_SYMBOLS[u];
        return std::wstring(tmp.begin(), tmp.end());
    }

    constexpr std::string get_symbol(Unit u) {
        return BASE_UNIT_SYMBOLS[u];
    }

    std::wstring wmake_superscript(int input) {
        std::wstring result = std::to_wstring(-input);
        for (std::size_t i = 0; i < result.size(); i++) {
            if(result[i] >= L'-' && result[i] <= L'9') {
                result[i] = SUPER_SCRIPT_SYMBOLS[result[i] - L'-'];
            }
        }
        return result;
    }

    template<std::size_t pos, std::array<Unit, pos> pos_arr, std::size_t neg, std::array<Unit, neg> neg_arr>
    constexpr auto get_unit_string() {
        std::string result = "";
        for(auto& el : pos_arr) {
            result += get_symbol(el);
        }
        if constexpr (neg > 0) {
            result += "/";
            for(auto& el : neg_arr) {
                result += get_symbol(el);
            }
        }
        return result;
    }

    template<std::size_t pos, std::array<Unit, pos> pos_arr, std::size_t neg, std::array<Unit, neg> neg_arr>
    constexpr auto wget_unit_string() {
        std::wstring result = L"";
        std::array<int, 7> pos_buckets = {};
        for(auto& el : pos_arr) {
            ++pos_buckets[el];
        }
        std::array<int, 7> neg_buckets = {};
        for(auto& el : neg_arr) {
            ++neg_buckets[el];
        }

        int index = 0;
        for(auto& el : pos_buckets) {
            if(el > 0) {
                result += wget_symbol(Unit(index));
                if(el != 1) result += wmake_superscript(el);
            }
            ++index;
        }
        index = 0;
        for(auto& el : neg_buckets) {
            if(el > 0) {
                result += wget_symbol(Unit(index));
                result += wmake_superscript(-el);
            }
            ++index;
        }
        return result;
    }

    template<std::size_t pos, std::array<Unit, pos> pos_arr, std::size_t neg, std::array<Unit, neg> neg_arr>
    constexpr int get_unit_exponent_offset() {
        return 0;
    }

    template <typename T, std::size_t N1, std::size_t N2>
    constexpr std::array<T, N1 + N2> concat(std::array<T, N1> lhs, std::array<T, N2> rhs)
    {
        std::array<T, N1 + N2> result{};
        std::size_t index = 0;

        for (auto& el : lhs) {
            result[index] = std::move(el);
            ++index;
        }
        for (auto& el : rhs) {
            result[index] = std::move(el);
            ++index;
        }

        return result;
    }

    template <std::size_t main_size, std::size_t other_size>
    constexpr std::size_t new_len(const std::array<Unit, main_size> main, const std::array<Unit, other_size> other) {
        std::array<bool, other_size> mask = {};
        std::size_t result = 0;
        for(auto& el : main) {
            std::size_t index = 0;
            bool flag = true;
            for(auto& el2 : other) {
                if (el == el2 && mask[index] == false) {
                    mask[index] = true;
                    flag = false;
                    break;
                }

                ++index;
            }
            if(flag) {
                ++result;
            }
        }
        return result;
    }

    template <std::size_t size, std::size_t main_size, std::size_t other_size>
    constexpr auto fix_arr(const std::array<Unit, main_size> main, const std::array<Unit, other_size> other) {
        std::array<Unit, size> result = {};
        std::array<bool, other_size> mask = {};
        std::size_t index = 0;
        for(auto& el : main) {
            std::size_t mask_index = 0;
            bool flag = true;
            for(auto& el2 : other) {
                if (el == el2 && mask[mask_index] == false) {
                    mask[mask_index] = true;
                    flag = false;
                    break;
                }
                ++mask_index;
            }
            if(flag) {
                result[index] = std::move(el);
                ++index;
            }
        }
        
        std::sort(result.begin(), result.end());

        return result;
    }

    //Special cases
    template <>
    constexpr auto get_unit_string<2, {METER, GRAM}, 2, {SECOND, SECOND}>() {
        return "N";
    }

    template <>
    constexpr auto wget_unit_string<2, {METER, GRAM}, 2, {SECOND, SECOND}>() {
        return L"N";
    }

    template <>
    constexpr int get_unit_exponent_offset<2, {METER, GRAM}, 2, {SECOND, SECOND}>() {
        return -3;
    }
}

template<class T, int exponent, std::size_t pos, std::array<Unit, pos> pos_arr, std::size_t neg, std::array<Unit, neg> neg_arr>
struct Value {
    Value(T v) { value = v; }

    T value;

    constexpr Value operator+ (Value const &obj) {
        return Value(value + obj.value);
    }

    constexpr Value operator- (Value const &obj) {
        return Value(value - obj.value);
    }

    constexpr auto rationalize() {
        constexpr auto new_len_pos = new_len(pos_arr, neg_arr);
        constexpr auto new_len_neg = new_len(neg_arr, pos_arr);
        constexpr auto new_arr_pos = fix_arr<new_len_pos, pos, neg>(pos_arr, neg_arr);
        constexpr auto new_arr_neg = fix_arr<new_len_neg, neg, pos>(neg_arr, pos_arr);
        return Value<T, exponent, new_len_pos, new_arr_pos, new_len_neg, new_arr_neg>(value);
    }

    constexpr auto reciprocal() {
        return Value<T, -exponent, neg, neg_arr, pos, pos_arr>((T)(1.0/value));
    }

    template<int exponent2, std::size_t pos2, std::array<Unit, pos2> pos_arr2, std::size_t neg2, std::array<Unit, neg2> neg_arr2>
    constexpr auto operator* (Value<T, exponent2, pos2, pos_arr2, neg2, neg_arr2> const &obj) {
        constexpr auto new_len_pos = pos + pos2;
        constexpr auto new_len_neg = neg + neg2;
        constexpr auto new_arr_pos = concat(pos_arr, pos_arr2);
        constexpr auto new_arr_neg = concat(neg_arr, neg_arr2);
        constexpr auto new_exponent = exponent + exponent2;
        auto temp = Value<T, new_exponent, new_len_pos, new_arr_pos, new_len_neg, new_arr_neg>(value * obj.value);
        return temp.rationalize();
    }

    template<int exponent2, std::size_t pos2, std::array<Unit, pos2> pos_arr2, std::size_t neg2, std::array<Unit, neg2> neg_arr2>
    constexpr auto operator/ (Value<T, exponent2, pos2, pos_arr2, neg2, neg_arr2> const &obj) {
        constexpr auto new_len_pos = pos + neg2;
        constexpr auto new_len_neg = neg + pos2;
        constexpr auto new_arr_pos = concat(pos_arr, neg_arr2);
        constexpr auto new_arr_neg = concat(neg_arr, pos_arr2);
        constexpr auto new_exponent = exponent - exponent2;
        auto temp = Value<T, new_exponent, new_len_pos, new_arr_pos, new_len_neg, new_arr_neg>(value / obj.value);
        return temp.rationalize();
    }
};

template<class T, int exponent, std::size_t pos, std::array<Unit, pos> pos_arr, std::size_t neg, std::array<Unit, neg> neg_arr>
std::wostream &operator<<(std::wostream &os, Value<T, exponent, pos, pos_arr, neg, neg_arr> const &m) { 
    init_locale();
    return os << wget_value_string<T, exponent + get_unit_exponent_offset<pos, pos_arr, neg, neg_arr>()>(m.value) << wget_unit_string<pos, pos_arr, neg, neg_arr>();
}

template<class T, int exponent, std::size_t pos, std::array<Unit, pos> pos_arr, std::size_t neg, std::array<Unit, neg> neg_arr>
std::ostream &operator<<(std::ostream &os, Value<T, exponent, pos, pos_arr, neg, neg_arr> const &m) { 
    return os << get_value_string<T, exponent + get_unit_exponent_offset<pos, pos_arr, neg, neg_arr>()>(m.value) << get_unit_string<pos, pos_arr, neg, neg_arr>();
}
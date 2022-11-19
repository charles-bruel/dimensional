#include <array>
#include <algorithm>
#include <ostream>
#include <string>

#include "wlocale.h"

//Default units
#define M(x) Value<x, 0, 1, {METER}, 0, {}>
#define S(x) Value<x, 0, 1, {SECOND}, 0, {}>

#define MPS(x) Value<x, 0, 1, {METER}, 1, {SECOND}>

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
    std::wstring wget_symbol(Unit u) {
        switch(u) {
            case METER:
                return L"m";
            case GRAM:
                return L"g";
            case SECOND:
                return L"s";
            case AMPERE:
                return L"A";
            case KELVIN:
                return L"K";
            case CANDELA:
                return L"cd";
            case MOL:
                return L"mol";
            default:
                return L"?";
        }
    }

    std::string get_symbol(Unit u) {
        switch(u) {
            case METER:
                return "m";
            case GRAM:
                return "g";
            case SECOND:
                return "s";
            case AMPERE:
                return "A";
            case KELVIN:
                return "K";
            case CANDELA:
                return "cd";
            case MOL:
                return "mol";
            default:
                return "?";
        }
    }

    constexpr std::wstring wmake_superscript(std::wstring input) {
        for (std::size_t i = 0; i < input.size(); i++) {
            switch(input[i]) {
                case '0':
                    input[i] = L'⁰';
                    break;
                case '1':
                    input[i] = L'¹';
                    break;
                case '2':
                    input[i] = L'²';
                    break;
                case '3':
                    input[i] = L'³';
                    break;
                case '4':
                    input[i] = L'⁴';
                    break;
                case '5':
                    input[i] = L'⁵';
                    break;
                case '6':
                    input[i] = L'⁶';
                    break;
                case '7':
                    input[i] = L'⁷';
                    break;
                case '8':
                    input[i] = L'⁸';
                    break;
                case '9':
                    input[i] = L'⁹';
                    break;
                case '-':
                    input[i] = L'⁻';
                    break;
            }
        }
        return input;
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
                if(el != 1) result += wmake_superscript(std::to_wstring(el));
            }
            ++index;
        }
        index = 0;
        for(auto& el : neg_buckets) {
            if(el > 0) {
                result += wget_symbol(Unit(index));
                result += wmake_superscript(std::to_wstring(-el));
            }
            ++index;
        }
        return result;
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
}

template<class T, std::size_t exponent, std::size_t pos, std::array<Unit, pos> pos_arr, std::size_t neg, std::array<Unit, neg> neg_arr>
struct Value {
    Value(T v) { value = v; }

    T value;

    constexpr Value operator+ (Value const &obj) {
        return Value(value + obj.value);
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

    template<std::size_t exponent2, std::size_t pos2, std::array<Unit, pos2> pos_arr2, std::size_t neg2, std::array<Unit, neg2> neg_arr2>
    constexpr auto operator* (Value<T, exponent2, pos2, pos_arr2, neg2, neg_arr2> const &obj) {
        constexpr auto new_len_pos = pos + pos2;
        constexpr auto new_len_neg = neg + neg2;
        constexpr auto new_arr_pos = concat(pos_arr, pos_arr2);
        constexpr auto new_arr_neg = concat(neg_arr, neg_arr2);
        constexpr auto new_exponent = exponent + exponent2;
        auto temp = Value<T, new_exponent, new_len_pos, new_arr_pos, new_len_neg, new_arr_neg>(value * obj.value);
        return temp.rationalize();
    }

    
};

template<class T, std::size_t exponent, std::size_t pos, std::array<Unit, pos> pos_arr, std::size_t neg, std::array<Unit, neg> neg_arr>
std::wostream &operator<<(std::wostream &os, Value<T, exponent, pos, pos_arr, neg, neg_arr> const &m) { 
    init_locale();
    return os << m.value << wget_unit_string<pos, pos_arr, neg, neg_arr>();
}

template<class T, std::size_t exponent, std::size_t pos, std::array<Unit, pos> pos_arr, std::size_t neg, std::array<Unit, neg> neg_arr>
std::ostream &operator<<(std::ostream &os, Value<T, exponent, pos, pos_arr, neg, neg_arr> const &m) { 
    return os << m.value << get_unit_string<pos, pos_arr, neg, neg_arr>();
}
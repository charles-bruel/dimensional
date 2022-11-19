#include <array>

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

    constexpr Value operator* (Value const &obj) {
        return Value(value * obj.value);
    }

    constexpr auto rationalize() {
        constexpr auto new_len_pos = new_len(pos_arr, neg_arr);
        constexpr auto new_len_neg = new_len(neg_arr, pos_arr);
        constexpr auto new_arr_pos = fix_arr<new_len_pos, pos, neg>(pos_arr, neg_arr);
        constexpr auto new_arr_neg = fix_arr<new_len_neg, neg, pos>(neg_arr, pos_arr);
        return Value<T, exponent, new_len_pos, new_arr_pos, new_len_neg, new_arr_neg>(value);
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
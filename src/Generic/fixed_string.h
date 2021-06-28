#pragma once

#include "stringutil.h"

namespace Engine {

template <size_t Size>
struct fixed_string {

    constexpr fixed_string(const char (&data)[Size + 1])
    {
        strcpy_s(mData, Size + 1, data);
    }

    template <size_t Size2>
    constexpr fixed_string<Size + Size2 - 1> operator+(const char (&data)[Size2]) const
    {
        char result[Size + Size2];
        strcpy_s(result, Size, mData);
        strcpy_s(result + Size, Size2, data);
        return result;
    }

    constexpr operator std::string_view() const& {
        return { mData, Size };
    }

private:
    char mData[Size + 1];
};

template <size_t Size>
fixed_string(const char (&data)[Size]) -> fixed_string<Size - 1>;

}
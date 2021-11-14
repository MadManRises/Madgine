#pragma once

#if __cpp_impl_three_way_comparison >= 201907L
DERIVE_OPERATOR(Spaceship, <=>)
#endif

template <typename T>
struct SpaceShipDummy2 {
    template <typename U>
    decltype(auto) operator>(const U &right) const
    {

#if __cpp_impl_three_way_comparison >= 201907L
        if constexpr (has_operator_Spaceship_v<T, U>) {
            return left <=> right;
        }
#else
        if constexpr (has_operator_Spaceship_v<T, U>) {
            return Spaceship(left, right);
        }
#endif
#if __cpp_lib_three_way_comparison < 201907L
        else {
            if (left < right)
                return std::strong_ordering::less;
            else if (left != right)
                return std::strong_ordering::greater;
            else
                return std::strong_ordering::equal;
        }
#endif
    }

    const T &left;
};

struct SpaceShipDummy {
    template <typename T>
    friend SpaceShipDummy2<T> operator<(const T &left, const SpaceShipDummy &)
    {
        return { left };
    }
};

constexpr SpaceShipDummy _;
#pragma once

#include <version>

#if __cpp_lib_constexpr_algorithms >= 201806L
#    define CONSTEXPR_ALGORITHM constexpr
#else
#define CONSTEXPR_ALGORITHM inline
#endif
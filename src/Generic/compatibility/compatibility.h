#pragma once

#ifdef __has_include
#    if __has_include(<version>)
#        include <version>
#    endif
#endif

#include "concepts.h"
#include "spaceship.h"
#include "coroutine.h"
#include "ranges.h"
#include "atomic.h"
#include "memory_resource.h"

#if __cpp_lib_constexpr_algorithms >= 201806L
#    define CONSTEXPR_ALGORITHM constexpr
#else
#    define CONSTEXPR_ALGORITHM inline
#endif

#if __cpp_consteval >= 201811L
#    define CONSTEVAL consteval
#else
#    define CONSTEVAL constexpr
#endif
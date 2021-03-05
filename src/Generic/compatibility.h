#pragma once

#ifdef __has_include
#    if __has_include(<version>)
#        include <version>
#    endif
#endif 


#if __cpp_lib_constexpr_algorithms >= 201806L
#    define CONSTEXPR_ALGORITHM constexpr
#else
#    define CONSTEXPR_ALGORITHM inline
#endif
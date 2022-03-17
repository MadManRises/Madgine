#pragma once

#if defined(__clang__)
#    define MADGINE_BREAKPOINT() __builtin_debugtrap()
#elif defined(_MSC_VER)
#    define MADGINE_BREAKPOINT() __debugbreak()
#else
#    define MADGINE_BREAKPOINT() assert(false) // It is expected that you define IM_DEBUG_BREAK() into something that will break nicely in a debugger!
#endif
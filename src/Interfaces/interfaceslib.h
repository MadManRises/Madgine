#pragma once

#include "Generic/genericlib.h"

#include "interfacesconfig.h"

/// @cond

#if defined(Interfaces_EXPORTS)
#    define INTERFACES_EXPORT DLL_EXPORT
#else
#    define INTERFACES_EXPORT DLL_IMPORT
#endif

#if WINDOWS
#    pragma warning(disable : 4251)
#    pragma warning(disable : 4275)
#    pragma warning(disable : 4661)
#endif

#ifdef _MSC_VER
#    define _CRT_SECURE_NO_WARNINGS
#endif



#include "debug/memory/memoryglobal.h"


#include <array>
#include <fstream>
#include <unordered_map>
#include <optional>
#include <cstring>

#if WINDOWS
#    include <memory_resource>
#else
#    include <experimental/memory_resource>
namespace std {
namespace experimental {
    namespace pmr {
        template <typename T>
        using vector = std::vector<T, polymorphic_allocator<T>>;
        using string = std::basic_string<char, char_traits<char>, polymorphic_allocator<char>>;
        template <typename Key,
            typename T,
            typename Hash = hash<Key>,
            typename Pred = equal_to<Key>>
        using unordered_map = std::unordered_map<Key, T, Hash, Pred,
            polymorphic_allocator<pair<const Key, T>>>;
    }
}
namespace pmr = experimental::pmr;
}
#endif


#include "interfacesforward.h"
#include "util/utilmethods.h"

using namespace std::literals::chrono_literals;
using namespace std::literals::string_literals;

/// @endcond

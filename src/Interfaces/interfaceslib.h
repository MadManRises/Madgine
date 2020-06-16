#pragma once

/// @cond

#include "interfacesconfig.h"

#include "commonlib.h"

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

#include <algorithm>
#include <array>
#include <atomic>
#include <cstring>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <regex>
#include <set>
#include <sstream>
#include <stack>
#include <stdlib.h>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

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

#include <optional>
#include <string_view>

#include <chrono>

#include <math.h>

#include <assert.h>

#include "interfacesforward.h"

#include "database/exceptionmessages.h"

using namespace std::literals::chrono_literals;
using namespace std::literals::string_literals;

/// @endcond

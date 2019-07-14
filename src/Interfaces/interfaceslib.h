#pragma once

/// @cond

#include "interfacesconfig.h"

#include "commonlib.h"

#if defined(STATIC_BUILD)
#define INTERFACES_EXPORT
#else
#if defined(Interfaces_EXPORTS)
#define INTERFACES_EXPORT DLL_EXPORT
#else
#define INTERFACES_EXPORT DLL_IMPORT
#endif
#endif

#if WINDOWS
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#pragma warning(disable : 4661)
#endif

#include "debug/memory/memoryglobal.h"

#include <cstring>
#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <array>
#include <vector>
#include <functional>
#include <memory>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <stack>
#include <thread>
#include <queue>
#include <mutex>
#include <variant>
#include <fstream>
#include <iomanip>
#include <type_traits>
#include <optional>
#include <mutex>
#include <variant>
#include <atomic>
#include <future>
#include <regex>

#if WINDOWS
#include <memory_resource>
#else
#include <experimental/memory_resource>
namespace std {
namespace experimental {
    namespace pmr {
        template <class T>
        using vector = std::vector<T, polymorphic_allocator<T>>;
        using string = std::basic_string<char, char_traits<char>, polymorphic_allocator<char>>;
        template <class Key,
            class T,
            class Hash = hash<Key>,
            class Pred = equal_to<Key>>
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

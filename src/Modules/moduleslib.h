#pragma once

/// @cond

#include "Interfaces/interfaceslib.h"

#include "modulesconfig.h"

#if defined(Modules_EXPORTS)
#    define MODULES_EXPORT DLL_EXPORT
#else
#    define MODULES_EXPORT DLL_IMPORT
#endif

#if WINDOWS
#    pragma warning(disable : 4251)
#    pragma warning(disable : 4275)
#    pragma warning(disable : 4661)
#endif

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
#include <shared_mutex>

#include <optional>
#include <string_view>

#include <chrono>

#include <math.h>

#include <assert.h>

#include "modulesforward.h"

#include "util/utilmethods.h"

#include "generic/container/container_traits.h"
#include "generic/templates.h"
#include "generic/derive.h"
#include "generic/container/iterator_traits.h"
#include "generic/callable_traits.h"
#include "generic/tupleunpacker.h"

#include "reflection/decay.h"
#include "reflection/classname.h"

using namespace std::literals::chrono_literals;
using namespace std::literals::string_literals;

/// @endcond

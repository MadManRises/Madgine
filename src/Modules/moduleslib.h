#pragma once

/// @cond

#include "Interfaces/interfaceslib.h"

#include "modulesconfig.h"

#if defined(STATIC_BUILD)
#define MODULES_EXPORT
#else
#if defined(Modules_EXPORTS)
#define MODULES_EXPORT DLL_EXPORT
#else
#define MODULES_EXPORT DLL_IMPORT
#endif
#endif

#if WINDOWS
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#pragma warning(disable : 4661)
#endif

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

#include <optional>
#include <string_view>

#include <chrono>

#include <math.h>

#include <assert.h>


#include "modulesforward.h"

#include "util/utilmethods.h"


using namespace std::literals::chrono_literals;
using namespace std::literals::string_literals;

/// @endcond

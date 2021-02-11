#pragma once

#include "commonlib.h"

/// @cond

#if defined(Generic_EXPORTS)
#    define GENERIC_EXPORT DLL_EXPORT
#else
#    define GENERIC_EXPORT DLL_IMPORT
#endif

#include <algorithm>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <stdint.h>
#include <string>
#include <vector>
#include <istream>
#include <chrono>
#include <future>
#include <array>
#include <sstring>

#include <assert.h>

#include "genericforward.h"

#include "callable_traits.h"
#include "classname.h"
#include "derive.h"
#include "container/container_traits.h"
#include "container/iterator_traits.h"
#include "decay.h"
#include "templates.h"
#include "tupleunpacker.h"

using namespace std::literals::chrono_literals;
using namespace std::literals::string_literals;

/// @endcond

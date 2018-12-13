#pragma once

/// @cond


#ifdef _WIN32
#ifdef __GNUC__
#define DLL_EXPORT __attribute__((dllexport))
#else
#define DLL_EXPORT __declspec(dllexport)
#endif
#else
#define DLL_EXPORT __attribute__((visibility("default")))
#endif

#ifdef _WIN32
#ifdef __GNUC__
#define DLL_IMPORT __attribute__((dllimport))
#else
#define DLL_IMPORT __declspec(dllimport)
#endif
#else
#define DLL_IMPORT
#endif

#if defined(STATIC_BUILD)
#define INTERFACES_EXPORT
#else
#if defined(Interfaces_EXPORTS)
#define INTERFACES_EXPORT DLL_EXPORT
#else
#define INTERFACES_EXPORT DLL_IMPORT
#endif
#endif

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#pragma warning(disable : 4661)

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
//#include <memory_resource>
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
#include <experimental/filesystem>
#include <type_traits>
#include <optional>
#include <mutex>
#include <variant>
#include <atomic>
#ifdef _WIN32
#include <memory_resource>
#else
#include <experimental/memory_resource>
namespace std {
	namespace experimental {
		namespace pmr {
			template <class T>
			using vector = std::vector<T, polymorphic_allocator<T>>;
			using string = std::basic_string<char, char_traits<char>, polymorphic_allocator<char>>;
		}
	}
	namespace pmr = experimental::pmr;	
}
#endif
#include <optional>

#include <chrono>

#include <math.h>

#include <assert.h>


#include "interfacesforward.h"

#include "scripting/scriptingexception.h"
#include "util/utilmethods.h"
#include "database/exceptionmessages.h"


using namespace std::literals::chrono_literals;
using namespace std::literals::string_literals;

/// @endcond

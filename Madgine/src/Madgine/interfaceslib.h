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

#if defined(Interfaces_EXPORTS)
#define INTERFACES_EXPORT DLL_EXPORT
#else
#define INTERFACES_EXPORT DLL_IMPORT
#endif

#define TEMPLATE_INSTANCE DLL_EXPORT


#ifdef __GNUC__
	#define TEMPLATE_EXPORT DLL_EXPORT
#else
	#define TEMPLATE_EXPORT
#endif

#pragma warning(disable : 4251)
#pragma warning(disable : 4661)


#include <cstring>
#include <string>
#include <list>
#include <map>
#include <array>
#include <vector>
#include <functional>
#include <memory>
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

#include <math.h>

#include <assert.h>


#include "interfacesforward.h"

#include "scripting/scriptingexception.h"
#include "util/utilmethods.h"
#include "database/exceptionmessages.h"

/// @endcond

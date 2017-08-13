#pragma once

/// @cond

#if defined(Interfaces_EXPORTS)
#ifdef __GNUC__
#define INTERFACES_EXPORT //__attribute__((dllexport))
#else
#define INTERFACES_EXPORT __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define INTERFACES_EXPORT //__attribute__((dllimport))
#else
#define INTERFACES_EXPORT __declspec(dllimport)
#endif
#endif


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
#include <stack>
#include <thread>
#include <queue>
#include <mutex>
#include <variant>
#include <fstream>
#include <iomanip>
#include <experimental/filesystem>

#include <math.h>

#include <assert.h>



#include "interfacesforward.h"

#include "singleton.h"

#include "scripting/scriptingexception.h"
#include "util/utilmethods.h"
#include "exceptionmessages.h"

/// @endcond

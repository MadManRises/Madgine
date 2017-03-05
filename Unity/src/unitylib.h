#pragma once

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#if defined(UnityMadgine_EXPORTS)
#define UNITYMADGINE_EXPORT __declspec(dllexport)
#else
#define UNITYMADGINE_EXPORT __declspec(dllimport)
#endif

#define _WIN32_WINNT 0x0500

#include "baselib.h"

#include <stack>
#include <memory>
#include <map>
#include <array>
#include <sstream>
#include <vector>
#include <typeindex>
#include <chrono>

#include "unityforward.h"



#ifdef _MSC_VER
#pragma warning(pop)
#endif



#pragma once


#if defined(Base_EXPORTS)
#define MADGINE_BASE_EXPORT __declspec(dllexport)
#else
#define MADGINE_BASE_EXPORT __declspec(dllimport)
#endif

#include "baseforward.h"

#include <set>

#include "interfaceslib.h"

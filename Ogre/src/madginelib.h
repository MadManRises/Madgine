#pragma once

/// @cond

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#if defined(OgreMadgine_EXPORTS)
#define OGREMADGINE_EXPORT __declspec(dllexport)
#else
#define OGREMADGINE_EXPORT __declspec(dllimport)
#endif

#include "baselib.h"

#include <stack>
#include <memory>
#include <map>
#include <array>
#include <sstream>
#include <vector>
#include <typeindex>
#include <chrono>

#include "forward.h"

#include <Ogre.h>

#include <Overlay/OgreFont.h>
#include <Overlay/OgreFontManager.h>

#include <OgreTerrainGroup.h>


#ifdef _MSC_VER
#pragma warning(pop)
#endif

/// @endcond

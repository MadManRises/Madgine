#pragma once

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#ifdef Madgine_EXPORTS
#define MADGINE_EXPORT __declspec(dllexport)
#else
#define MADGINE_EXPORT __declspec(dllimport)
#endif


#include "forward.h"

#include <Ogre.h>

#include <OgreFont.h>
#include <OgreFontManager.h>

#include <OgreRTShaderSystem.h>

#include <OgreTerrainGroup.h>

#include <stack>
#include <memory>
#include <map>
#include <array>
#include <sstream>
#include <vector>
#include <typeindex>

#include "Ogrememory.h"

#include "singletonimpl.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif



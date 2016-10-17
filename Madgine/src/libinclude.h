#pragma once

#pragma warning(push, 0)

#ifdef Madgine_EXPORTS
#define MADGINE_EXPORT __declspec(dllexport)
#else
#define MADGINE_EXPORT __declspec(dllimport)
#endif


#include "forward.h"

#include <Ogre.h>

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

#pragma warning(pop)



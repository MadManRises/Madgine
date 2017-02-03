#pragma once

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#if defined(Madgine_EXPORTS)
#define MADGINE_EXPORT __declspec(dllexport)
#else
#define MADGINE_EXPORT __declspec(dllimport)
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

#include <OgreRTShaderSystem.h>

#include <OgreTerrainGroup.h>



#include "Ogrememory.h"

namespace Ogre {
	template <class T>
	T *Singleton<T>::msSingleton = 0;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif



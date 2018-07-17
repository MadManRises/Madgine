#pragma once

/// @cond

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#if defined(OgreMadgine_EXPORTS)
#define OGREMADGINE_EXPORT DLL_EXPORT
#else
#define OGREMADGINE_EXPORT DLL_IMPORT
#endif

#include "Madgine/clientlib.h"

#include "ogreforward.h"

#include <OGRE/Ogre.h>

#include <OGRE/Overlay/OgreFont.h>
#include <OGRE/Overlay/OgreFontManager.h>

#include <MYGUI/MyGUI.h>
#include <MYGUI/MyGUI_OgreRenderManager.h>
#include <MYGUI/MyGUI_OgrePlatform.h>

#undef NO_ERROR


/// @endcond

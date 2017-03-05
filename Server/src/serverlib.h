#pragma once


#if defined(MadgineServer_EXPORTS)
#define MADGINE_SERVER_EXPORT __declspec(dllexport)
#else
#define MADGINE_SERVER_EXPORT __declspec(dllimport)
#endif

#include "serverforward.h"

#include "baselib.h"

#include <thread>

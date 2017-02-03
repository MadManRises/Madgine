#pragma once

#if defined(Interfaces_EXPORTS)
#define INTERFACES_EXPORT __declspec(dllexport)
#else
#define INTERFACES_EXPORT __declspec(dllimport)
#endif



#include <string>
#include <list>
#include <map>
#include <array>
#include <vector>
#include <functional>
#include <memory>
#include <sstream>
#include <algorithm>

#include <assert.h>

#include <WinSock2.h>
#include <Windows.h>
#undef min

#include "interfacesforward.h"

#include "singleton.h"


namespace Ogre {
	enum LogMessageLevel;
}
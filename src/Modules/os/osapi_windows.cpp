#include "../moduleslib.h"

#if WINDOWS

#define NOMINMAX
#include <Windows.h>

#include "osapi.h"

namespace Engine {
namespace OS {

	void setEnvironmentVariable(const std::string& name, const std::string& value) {
        SetEnvironmentVariable(name.c_str(), value.c_str());
	}

}
}

#endif
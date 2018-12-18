#include "../interfaceslib.h"

#include "runtime.h"

#if _WIN32
#	define NOMINMAX 1
#	include <Windows.h>
#elif __linux__
#	include <dlfcn.h>
#else
#	error "Unsupported Platform!"
#endif

namespace Engine {

	std::experimental::filesystem::path runtimePath()
	{
		char buffer[512];
#ifdef _WIN32
		auto result = GetModuleFileName(nullptr, buffer, sizeof(buffer));
#elif __linux__
		auto result = readlink("/proc/self/exe", buffer, sizeof(buffer));
#endif
		assert(result > 0);
		return std::experimental::filesystem::path(buffer).parent_path();
	}

}
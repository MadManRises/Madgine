#include "../interfaceslib.h"

#include "runtime.h"
#include "../generic/templates.h"

#if _WIN32
#	define NOMINMAX 1
#	include <Windows.h>
#	include <Psapi.h>
#elif __linux__
#	include <unistd.h>
#	include <link.h>
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


#if __linux__
	static int VisitModule(struct dl_phdr_info *info, size_t size, void *data) {
		std::set<std::string> *modules = static_cast<std::set<std::string> *>(data);

		modules->insert(info->dlpi_name);
		return 0;
	}
#endif

	std::set<std::string> enumerateLoadedLibraries()
	{
		std::set<std::string> result;

#if _WIN32
		DWORD count;
		HMODULE modules[512];
		auto check = EnumProcessModules(GetCurrentProcess(), modules, array_size(modules), &count);
		assert(check);
		count /= sizeof(HMODULE);
		assert(count < array_size(modules));

		for (DWORD i = 0; i < count; ++i)
		{
			char buffer[512];
			auto check2 = GetModuleFileName(modules[i], buffer, sizeof(buffer));
			assert(check2);
			std::experimental::filesystem::path path = buffer;
			result.insert(path.stem().generic_string());
		}
#elif __linux__
		dl_iterate_phdr(VisitModule, &result);
#else
#	error "Unsupported Platform!"
#endif

		return result;
	}

}
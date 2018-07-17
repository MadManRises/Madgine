#include "../interfaceslib.h"

#include "plugin.h"

#ifdef _WIN32
#include <Windows.h>
#elif defined(__linux__) || defined(__EMSCRIPTEN__)
#include <dlfcn.h>
#else
#error "Unsupported Platform"
#endif

namespace Engine
{
	namespace Plugins
	{

		Plugin::Plugin(const std::experimental::filesystem::path &path) :
			mModule(nullptr),
			mPath(path)
		{
		}

		Plugin::~Plugin()
		{
			unload();
		}

		bool Plugin::isLoaded()
		{
			return mModule != nullptr;
		}

		bool Plugin::load()
		{
			if (isLoaded())
				return true;

#ifdef _WIN32
			UINT errorMode = GetErrorMode();
			//SetErrorMode(SEM_FAILCRITICALERRORS);
#endif

			try {
#ifdef _WIN32
				mModule = LoadLibrary(mPath.string().c_str());
#elif __linux__
				mModule = dlopen(mPath.string().c_str(), RTLD_LAZY);
#endif
			}
			catch (const std::exception &e) {
				mModule = nullptr;
			}

#ifdef _WIN32
			SetErrorMode(errorMode);
#endif

			return isLoaded();
		}

		bool Plugin::unload() {
			if (!isLoaded())
				return true;

#ifdef _WIN32
			bool result = (FreeLibrary((HINSTANCE)mModule) != 0);
#elif defined(__linux__) || defined(__EMSCRIPTEN__)
			bool result = (dlclose(mModule) == 0);
#endif
			assert(result);
			
			mModule = nullptr;
			
			return result;
		}


		void *Plugin::getSymbol(const std::string &name) const {
#ifdef _WIN32
			return GetProcAddress((HINSTANCE)mModule, name.c_str());
#elif __linux__
			return dlsym(mModule, name.c_str());
#endif
		}

		std::experimental::filesystem::path Plugin::fullPath()
		{
			std::experimental::filesystem::path path;

			if (!isLoaded())
				return path;

#ifdef _WIN32
			char buffer[512];
			assert(GetModuleFileName((HMODULE)mModule, buffer, sizeof(buffer)) != 0);
			path = buffer;
#endif
			return path;
		}

		std::experimental::filesystem::path Plugin::runtimePath()
		{
#ifdef _WIN32
			char buffer[512];
			assert(GetModuleFileName(nullptr, buffer, sizeof(buffer)) != 0);
			return std::experimental::filesystem::path(buffer).parent_path();
#endif
		}
	}
}
#include "../interfaceslib.h"

#include "plugin.h"

#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
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

		bool Plugin::isLoaded() const
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
				if (mPath.empty())
					mModule = GetModuleHandle(nullptr);
				else
					mModule = LoadLibrary(mPath.string().c_str());
				SymRefreshModuleList(GetCurrentProcess());
#elif __linux__
				mModule = dlopen(mPath.string().c_str(), RTLD_LAZY);
#endif
			}
			catch (const std::exception &e) {
				LOG_ERROR(Database::message("Load of plugin \"", "\" failed with error: ", "")(mPath, e.what()));
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
			bool result = mPath.empty() || (FreeLibrary((HINSTANCE)mModule) != 0);
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
			auto result = GetModuleFileName((HMODULE)mModule, buffer, sizeof(buffer));
			assert(result);
			path = buffer;
#endif
			return path;
		}

		std::experimental::filesystem::path Plugin::runtimePath()
		{
#ifdef _WIN32
			char buffer[512];
			auto result = GetModuleFileName(nullptr, buffer, sizeof(buffer));
			assert(result);
			return std::experimental::filesystem::path(buffer).parent_path();
#endif
		}
	}
}
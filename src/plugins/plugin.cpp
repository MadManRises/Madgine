#include "../interfaceslib.h"

#include "plugin.h"

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <dlfcn.h>
#else
#error "Unsupported Platform"
#endif

namespace Engine
{
	namespace Plugins
	{

		Plugin::Plugin(const std::string &path) :
			mModule(nullptr),
			mPath(path)
		{
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
				mModule = LoadLibrary(mPath.c_str());
#elif __linux__
				mModule = dlopen(mPath.c_str(), RTLD_LAZY);
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
#elif __linux__
			bool result = (dlclose(mModule) == 0);
#endif
			assert(result);
			
			mModule = nullptr;
			
			return result;
		}


		void *Plugin::getSymbol(const std::string &name) {
#ifdef _WIN32
			return GetProcAddress((HINSTANCE)mModule, name.c_str());
#elif __linux__
			return dlsym(mModule, name.c_str());
#endif
		}


	}
}
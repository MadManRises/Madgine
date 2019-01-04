#include "../interfaceslib.h"

#include "plugin.h"

#include "../generic/templates.h"

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>
#elif defined(__linux__) || defined(__EMSCRIPTEN__)
#include <unistd.h>
#include <dlfcn.h>
#else
#error "Unsupported Platform"
#endif

namespace Engine
{
	namespace Plugins
	{

		Plugin::Plugin(std::string name, Filesystem::Path path) :
			mModule(nullptr),
			mPath(std::move(path)),
			mName(std::move(name))
		{
			if (mPath.empty() && !mName.empty())
			{
#if _WIN32
				mPath = mName;
#elif __linux__
				mPath = "lib" + mName + ".so";
#endif
			}
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

			LOG(Database::message("Loading Plugin \"", "\"...")(mName));

#ifdef _WIN32
			UINT errorMode = GetErrorMode();
			//SetErrorMode(SEM_FAILCRITICALERRORS);
#endif

			std::string errorMsg;

			try {
#ifdef _WIN32
				if (mPath.empty())
					mModule = GetModuleHandle(nullptr);
				else
					mModule = LoadLibrary(mPath.c_str());
				SymRefreshModuleList(GetCurrentProcess());
#elif __linux__
				if (mPath.empty())
					mModule = dlopen(nullptr, RTLD_LAZY);
				else
					mModule = dlopen(mPath.c_str(), RTLD_NOW);
				if (!isLoaded())
					errorMsg = dlerror();
#endif
			}
			catch (const std::exception &e) {
				errorMsg = e.what();
				mModule = nullptr;
			}

#ifdef _WIN32
			SetErrorMode(errorMode);
#endif

			if (!isLoaded())
			{
				LOG_ERROR(Database::message("Load of plugin \"", "\" failed with error: ", "")(mName, errorMsg));
			}
			else
			{
				LOG("Success");
			}

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


		const void *Plugin::getSymbol(const std::string &name) const {
			std::string fullName = name + "_" + mName;
#ifdef _WIN32
			return GetProcAddress((HINSTANCE)mModule, fullName.c_str());
#elif __linux__
			return dlsym(mModule, fullName.c_str());
#endif
		}

		Filesystem::Path Plugin::fullPath() const
		{
			Filesystem::Path path;

			if (!isLoaded())
				return path;

#ifdef _WIN32
			char buffer[512];
			auto result = GetModuleFileName((HMODULE)mModule, buffer, sizeof(buffer));
			assert(result);
			path = buffer;
#elif __linux__
			Dl_info info;
			auto result = dladdr(getSymbol("binaryInfo"), &info);
			assert(result);
			path = info.dli_fname;
#endif
			return path;
		}



	}
}
#include "../interfaceslib.h"

#include "plugin.h"

#include "../generic/templates.h"

#if WINDOWS
#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>
#elif UNIX
#include <unistd.h>
#include <dlfcn.h>
#else
#error "Unsupported Platform"
#endif

namespace Engine
{
	namespace Plugins
	{

		Plugin::Plugin(std::string name, std::string project, Filesystem::Path path) :
			mModule(nullptr),
			mPath(std::move(path)),
			mProject(std::move(project)),
			mName(std::move(name))
		{
			if (mPath.empty() && !mName.empty())
			{
#if WINDOWS
				mPath = mName;
#elif UNIX
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

			LOG("Loading Plugin \"" << mName << "\"...");

#if WINDOWS
			UINT errorMode = GetErrorMode();
			//SetErrorMode(SEM_FAILCRITICALERRORS);
#endif

			std::string errorMsg;

			try {
#if WINDOWS
				if (mPath.empty())
					mModule = GetModuleHandle(nullptr);
				else
					mModule = LoadLibrary(mPath.c_str());
				SymRefreshModuleList(GetCurrentProcess());
#elif UNIX
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

#if WINDOWS
			SetErrorMode(errorMode);
#endif

			if (!isLoaded())
			{
				LOG_ERROR("Load of plugin \"" << mName << "\" failed with error: " << errorMsg);
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

#if WINDOWS
			bool result = mPath.empty() || (FreeLibrary((HINSTANCE)mModule) != 0);
#elif UNIX
			bool result = (dlclose(mModule) == 0);
#endif
			assert(result);
			
			mModule = nullptr;
			
			return result;
		}


		const void *Plugin::getSymbol(const std::string &name) const {
			std::string fullName = name + "_" + mName;
#if WINDOWS
			return GetProcAddress((HINSTANCE)mModule, fullName.c_str());
#elif UNIX
			return dlsym(mModule, fullName.c_str());
#endif
		}

		Filesystem::Path Plugin::fullPath() const
		{
			Filesystem::Path path;

			if (!isLoaded())
				return path;

#if WINDOWS
			char buffer[512];
			auto result = GetModuleFileName((HMODULE)mModule, buffer, sizeof(buffer));
			assert(result);
			path = buffer;
#elif UNIX
			Dl_info info;
			auto result = dladdr(getSymbol("binaryInfo"), &info);
			assert(result);
			path = info.dli_fname;
#endif
			return path;
		}

		const std::string & Plugin::project() const
		{
			return mProject;
		}



	}
}